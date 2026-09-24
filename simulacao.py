"""Motor determinístico de eventos discretos, utilizável sem Pygame."""

from collections import deque
import heapq
import itertools
import random
import math

from algoritmos import Candidato
from cidade import Cidade
from metricas import Metricas
from semaforo import Semaforo, VERDE, TRANSICAO, INTERVALO_SAIDA, LIMITE_ESPERA
from veiculo import Pedido, Veiculo


# No mesmo instante, mudanças físicas da rede precedem a leitura dos sinais.
# A admissão externa ocorre após as liberações, aproveitando espaço recém aberto.
PRIORIDADE_EVENTO = {
    "entrada": 0,
    "chegada": 1,
    "decisao": 2,
    "verde": 3,
    "liberacao": 4,
    "admissao": 5,
}


def gerar_demanda(semente: int, demanda: float, duracao: float) -> tuple[Pedido, ...]:
    """Poisson (veículos/minuto), mesma lista para todas as malhas e políticas."""
    if not math.isfinite(demanda) or demanda <= 0:
        raise ValueError("A demanda deve ser positiva e finita.")
    if not math.isfinite(duracao) or duracao <= 0:
        raise ValueError("A duração deve ser positiva e finita.")
    rng = random.Random(semente)
    bordas = Cidade().entradas
    pedidos = []
    instante = 0.0
    while True:
        instante += rng.expovariate(demanda / 60)
        if instante > duracao:
            break
        # Corredor pendular predominante, com tráfego distribuído no restante.
        if rng.random() < 0.6:
            origem, destino = (0, 1), (3, 1)
            if rng.random() < 0.3:
                origem, destino = destino, origem
        else:
            origem, destino = rng.sample(bordas, 2)
        pedidos.append(Pedido(instante, origem, destino))
    return tuple(pedidos)


class Simulacao:
    def __init__(self, politica="guloso", nova_via=False, semente=42,
                 demanda=45.0, duracao=300.0, pedidos=None):
        if politica not in ("fixo", "guloso", "prazo"):
            raise ValueError("Política deve ser fixo, guloso ou prazo.")
        if not math.isfinite(duracao) or duracao <= 0:
            raise ValueError("A duração deve ser positiva e finita.")
        self.politica, self.nova_via = politica, nova_via
        self.semente, self.demanda, self.duracao = semente, demanda, duracao
        self.cidade = Cidade(nova_via)
        self.pedidos = (gerar_demanda(semente, demanda, duracao)
                        if pedidos is None else tuple(pedidos))
        self.agora = 0.0
        self.eventos = []
        self.sequencia = itertools.count()
        self.veiculos: dict[int, Veiculo] = {}
        self.externas = {n: deque() for n in self.cidade.entradas}
        self.semaforos = {n: Semaforo(self.cidade.fases(n), politica)
                          for n in self.cidade.grafo}
        self.metricas = Metricas()
        self.ultimo_evento = "Simulação pronta"
        self.historico = deque(maxlen=5)
        for i, pedido in enumerate(self.pedidos):
            if (not math.isfinite(pedido.instante)
                    or not 0 <= pedido.instante <= duracao
                    or pedido.origem not in self.externas
                    or pedido.destino not in self.cidade.grafo
                    or pedido.origem == pedido.destino):
                raise ValueError(
                    "Pedido inválido: instante fora do intervalo, origem fora da borda, "
                    "destino inexistente ou origem igual ao destino."
                )
            self.agendar(pedido.instante, "entrada", i)
        for no in self.semaforos:
            self.agendar(0.0, "decisao", no)

    @property
    def encerrada(self):
        return self.agora >= self.duracao and (not self.eventos or self.eventos[0][0] > self.duracao)

    def agendar(self, instante, tipo, dado):
        if instante < self.agora:
            raise ValueError("Não é possível agendar um evento no passado.")
        heapq.heappush(self.eventos, (instante, PRIORIDADE_EVENTO[tipo],
                                      next(self.sequencia), tipo, dado))

    def _relogio(self, instante):
        total = sum(len(r.fila) for r in self.cidade.ruas.values())
        self.metricas.integrar(instante, total)
        self.agora = instante

    def passo(self):
        if self.encerrada:
            return False
        if not self.eventos or self.eventos[0][0] > self.duracao:
            self._relogio(self.duracao)
            return False
        instante, _, _, tipo, dado = heapq.heappop(self.eventos)
        self._relogio(instante)
        getattr(self, "_" + tipo)(dado)
        self.metricas.observar(len(r.fila) for r in self.cidade.ruas.values())
        self.ultimo_evento = f"{instante:6.1f}s | {tipo}"
        self.historico.append(self.ultimo_evento)
        return True

    def avancar(self, ate):
        alvo = min(max(ate, self.agora), self.duracao)
        while self.eventos and self.eventos[0][0] <= alvo and not self.encerrada:
            self.passo()
        self._relogio(alvo)

    def executar(self):
        self.avancar(self.duracao)
        return self.resultado()

    def candidatos(self, no):
        sinal = self.semaforos[no]
        candidatos = []
        for fase in sinal.fases:
            rua = self.cidade.ruas[fase]
            primeiro = self.veiculos[rua.fila[0]] if rua.fila else None
            livre = bool(primeiro and (primeiro.proxima is None or
                         self.cidade.ruas[primeiro.proxima].livre))
            espera = primeiro.espera_atual(self.agora) if primeiro else 0.0
            prazo = primeiro.inicio_espera + LIMITE_ESPERA if primeiro else None
            candidatos.append(Candidato(fase, len(rua.fila), espera,
                                        sinal.ultimo[fase], livre, prazo))
        return candidatos

    def _entrada(self, id_veiculo):
        pedido = self.pedidos[id_veiculo]
        v = Veiculo(id_veiculo, pedido, self.cidade.rota(pedido.origem, pedido.destino))
        self.veiculos[v.id] = v
        fila = self.externas[pedido.origem]
        fila.append(v.id)
        if len(fila) == 1:
            self.agendar(self.agora, "admissao", pedido.origem)

    def _admissao(self, origem):
        fila = self.externas[origem]
        if not fila:
            return
        v = self.veiculos[fila[0]]
        if self.cidade.ruas[v.aresta].livre:
            fila.popleft()
            v.entrou = self.agora
            self._iniciar_trecho(v)
        if fila:
            self.agendar(self.agora + INTERVALO_SAIDA, "admissao", origem)

    def _iniciar_trecho(self, v):
        rua = self.cidade.ruas[v.aresta]
        assert rua.livre, "Capacidade da via excedida"
        rua.ocupantes.add(v.id)
        v.estado = "movimento"
        v.inicio_trecho, v.fim_trecho = self.agora, self.agora + rua.tempo
        self.agendar(v.fim_trecho, "chegada", v.id)

    def _chegada(self, id_veiculo):
        v = self.veiculos[id_veiculo]
        rua = self.cidade.ruas[v.aresta]
        if v.proxima is None:
            rua.ocupantes.remove(v.id)
            v.estado, v.terminou = "concluido", self.agora
        else:
            v.estado, v.inicio_espera = "fila", self.agora
            rua.fila.append(v.id)

    def _decisao(self, no):
        sinal = self.semaforos[no]
        # Uma fase só é medida depois que seus 10 s de verde terminaram.
        if sinal.prazo_em_atendimento is not None:
            self.metricas.registrar_fase(sinal.prazo_em_atendimento, self.agora)
            sinal.prazo_em_atendimento = None
        sinal.instante_decisao = self.agora
        candidatos = self.candidatos(no)
        fase = sinal.escolher(candidatos)
        if fase is not None:
            sinal.prazo_em_atendimento = next(c.prazo for c in candidatos if c.fase == fase)
        anterior = sinal.ativa
        sinal.geracao += 1
        sinal.ativa = None
        sinal.proxima = fase
        # Mesmo verde pode continuar. Mudanças incluem dois segundos de vermelho geral.
        atraso = TRANSICAO if fase != anterior else 0.0
        sinal.fim_verde = self.agora + atraso + VERDE
        self.agendar(self.agora + atraso, "verde", (no, sinal.geracao))
        self.agendar(sinal.fim_verde, "decisao", no)

    def _verde(self, dado):
        no, geracao = dado
        sinal = self.semaforos[no]
        if geracao != sinal.geracao:
            return
        sinal.ativa = sinal.proxima
        if sinal.ativa is not None:
            if sinal.prazo_em_atendimento is None:
                rua = self.cidade.ruas[sinal.ativa]
                if rua.fila:
                    primeiro = self.veiculos[rua.fila[0]]
                    sinal.prazo_em_atendimento = primeiro.inicio_espera + LIMITE_ESPERA
            sinal.ultimo[sinal.ativa] = self.agora
            self.agendar(self.agora, "liberacao", dado)

    def _liberacao(self, dado):
        no, geracao = dado
        sinal = self.semaforos[no]
        if geracao != sinal.geracao or sinal.ativa is None or self.agora >= sinal.fim_verde:
            return
        rua = self.cidade.ruas[sinal.ativa]
        if rua.fila:
            v = self.veiculos[rua.fila[0]]
            if sinal.prazo_em_atendimento is None:
                sinal.prazo_em_atendimento = v.inicio_espera + LIMITE_ESPERA
            if self.cidade.ruas[v.proxima].livre:
                rua.fila.popleft()
                rua.ocupantes.remove(v.id)
                espera = v.espera_atual(self.agora)
                v.espera += espera
                v.maior_espera = max(v.maior_espera, espera)
                v.inicio_espera = None
                v.indice += 1
                self._iniciar_trecho(v)
        proximo = self.agora + INTERVALO_SAIDA
        if proximo < sinal.fim_verde:
            self.agendar(proximo, "liberacao", dado)

    def resultado(self):
        todos = list(self.veiculos.values())
        concluidos = [v for v in todos if v.terminou is not None]
        entraram = [v for v in todos if v.entrou is not None]
        esperas = [v.espera + v.espera_atual(self.agora) for v in entraram]
        tempos = [v.terminou - v.pedido.instante for v in concluidos]
        externa = [((v.entrou if v.entrou is not None else self.agora) - v.pedido.instante)
                   for v in todos]
        return {
            "politica": self.politica, "nova_via": self.nova_via,
            "semente": self.semente, "demanda": self.demanda,
            "tempo_simulado": round(self.agora, 3), "gerados": len(todos),
            "concluidos": len(concluidos),
            "em_circulacao": sum(v.estado in ("movimento", "fila") for v in todos),
            "aguardando_entrada": sum(v.estado == "externo" for v in todos),
            "viagem_media_s": round(sum(tempos) / len(tempos), 3) if tempos else None,
            "espera_media_s": round(sum(esperas) / len(esperas), 3) if esperas else 0.0,
            "espera_maxima_s": round(max((max(v.maior_espera, v.espera_atual(self.agora))
                                           for v in entraram), default=0.0), 3),
            "espera_externa_media_s": round(sum(externa) / len(externa), 3) if externa else 0.0,
            "fila_media_total": round(self.metricas.area_filas / self.agora, 3) if self.agora else 0.0,
            "fila_maxima_via": self.metricas.max_fila,
            "fila_atual_total": sum(len(r.fila) for r in self.cidade.ruas.values()),
            "atraso_maximo_fase_s": round(self.metricas.atraso_maximo_fase_s, 3),
            "fases_atrasadas": self.metricas.fases_atrasadas,
            "fases_atendidas": self.metricas.fases_atendidas,
        }
