"""Interface didática: configuração, mapa, inspeção e comparação pareada."""

import math
from datetime import datetime
from pathlib import Path
import pygame

from cidade import nome
from experimentos import CENARIOS, exportar
from simulacao import Simulacao, gerar_demanda

LARGURA, ALTURA = 1280, 840
FUNDO = (12, 20, 32)
PAINEL = (21, 33, 49)
BORDA = (43, 59, 76)
TEXTO = (231, 238, 244)
SUAVE = (157, 177, 196)
VERDE = (64, 218, 161)
AZUL = (91, 170, 253)
AMARELO = (255, 204, 105)
VERMELHO = (245, 109, 112)
POLITICAS = ("guloso", "prazo", "fixo")
ROTULOS_POLITICA = {"guloso": "Gulosa (fila)", "prazo": "Prazo (EDD)", "fixo": "Ciclo fixo"}


class Aplicacao:
    def __init__(self, semente=42, demanda=45.0, duracao=300.0):
        pygame.init()
        # O desenho usa coordenadas lógicas. A janela cabe no monitor e pode ser
        # redimensionada sem deslocar os controles ou as áreas clicáveis.
        monitor = pygame.display.Info()
        escala_inicial = min(1.0, (monitor.current_w - 64) / LARGURA,
                             (monitor.current_h - 80) / ALTURA)
        escala_inicial = max(0.25, escala_inicial)
        tamanho = (round(LARGURA * escala_inicial), round(ALTURA * escala_inicial))
        self.janela = pygame.display.set_mode(tamanho, pygame.RESIZABLE)
        self.tela = pygame.Surface((LARGURA, ALTURA)).convert()
        pygame.display.set_caption("Cidade em Fluxo | Algoritmos Ambiciosos — G48")
        self.fontes = {t: pygame.font.SysFont("segoeui", t) for t in (13, 15, 17, 20, 24, 32)}
        self.semente, self.demanda, self.duracao = semente, demanda, duracao
        self.politica, self.nova_via = "prazo", False
        self.velocidade = 4
        self.selecionado = (1, 1)
        self.rodando = False
        self.resultados = []
        self.comparacao = None
        self.mostrar_resultados = False
        self.botoes = {}
        self.reiniciar()

    def reiniciar(self):
        self.sim = Simulacao(self.politica, self.nova_via, self.semente, self.demanda, self.duracao)
        self.rodando = False
        self.resultados = []
        self.comparacao = None
        self.mostrar_resultados = False
        self.status = "Configuração pronta. Pressione Iniciar para simular."

    def texto(self, conteudo, x, y, tamanho=17, cor=TEXTO):
        superficie = self.fontes[tamanho].render(str(conteudo), True, cor)
        self.tela.blit(superficie, (x, y))
        return superficie.get_width()

    def texto_limitado(self, conteudo, x, y, largura, tamanho=17, cor=TEXTO):
        """Evita que uma justificativa longa atravesse a borda do painel."""
        conteudo = str(conteudo)
        fonte = self.fontes[tamanho]
        if fonte.size(conteudo)[0] > largura:
            while conteudo and fonte.size(conteudo + "…")[0] > largura:
                conteudo = conteudo[:-1]
            conteudo += "…"
        self.texto(conteudo, x, y, tamanho, cor)

    def caixa(self, rect, cor=PAINEL, raio=12):
        pygame.draw.rect(self.tela, cor, rect, border_radius=raio)

    def area_visual(self):
        """Retângulo ocupado pelo desenho dentro da janela, com proporção fixa."""
        largura, altura = self.janela.get_size()
        escala = min(largura / LARGURA, altura / ALTURA)
        visual_w, visual_h = round(LARGURA * escala), round(ALTURA * escala)
        return pygame.Rect((largura - visual_w) // 2, (altura - visual_h) // 2,
                           visual_w, visual_h)

    def para_logico(self, pos):
        area = self.area_visual()
        if not area.collidepoint(pos):
            return None
        return ((pos[0] - area.x) * LARGURA / area.width,
                (pos[1] - area.y) * ALTURA / area.height)

    def para_tela(self, pos):
        """Converte uma posição lógica para a janela (útil também nos testes)."""
        area = self.area_visual()
        return (round(area.x + pos[0] * area.width / LARGURA),
                round(area.y + pos[1] * area.height / ALTURA))

    def apresentar(self):
        area = self.area_visual()
        self.janela.fill(FUNDO)
        if area.size == self.tela.get_size():
            self.janela.blit(self.tela, area)
        else:
            self.janela.blit(pygame.transform.smoothscale(self.tela, area.size), area)
        pygame.display.flip()

    def botao(self, chave, rotulo, rect, destaque=False):
        rect = pygame.Rect(rect)
        mouse = self.para_logico(pygame.mouse.get_pos())
        sobre = mouse is not None and rect.collidepoint(mouse)
        cor = (37, 83, 79) if destaque else ((48, 67, 88) if sobre else (32, 48, 67))
        self.caixa(rect, cor, 7)
        pygame.draw.rect(self.tela, VERDE if destaque else BORDA, rect, 1, border_radius=7)
        superficie = self.fontes[15].render(rotulo, True, TEXTO)
        self.tela.blit(superficie, superficie.get_rect(center=rect.center))
        self.botoes[chave] = rect

    @staticmethod
    def pos(no):
        return pygame.Vector2(107 + no[0] * 177, 292 + no[1] * 120)

    def pontos_rua(self, aresta):
        rua = self.sim.cidade.ruas[aresta]
        a, b = (self.pos(no) for no in aresta)
        if rua.nova:
            # Arco elevado para distinguir a via expressa da malha inferior.
            direcao = 1 if a.x < b.x else -1
            return [pygame.Vector2(a.x + (b.x - a.x) * t / 30,
                                  a.y - 165 * math.sin(math.pi * t / 30) + direcao * 5)
                    for t in range(31)]
        vetor = b - a
        normal = pygame.Vector2(-vetor.y, vetor.x).normalize() * 6
        return [a + normal, b + normal]

    @staticmethod
    def ao_longo(pontos, fracao):
        indice = min(len(pontos) - 2, int(fracao * (len(pontos) - 1)))
        local = fracao * (len(pontos) - 1) - indice
        return pontos[indice].lerp(pontos[indice + 1], local)

    def posicoes_rua(self, rua):
        """Posiciona cada carro atrás do anterior, inclusive antes de entrar na fila.

        O motor guarda fila FIFO e capacidade, mas não calcula coordenadas físicas.
        A interpolação visual precisa respeitar a cauda para não fazer o carro azul
        atravessar os amarelos enquanto ainda percorre o trecho.
        """
        intervalo = 0.68 / max(rua.capacidade - 1, 1)
        posicoes = {}
        frente = 0.78 + intervalo
        for id_veiculo in rua.fila:
            frente -= intervalo
            posicoes[id_veiculo] = frente
        em_movimento = sorted(
            (self.sim.veiculos[id_veiculo] for id_veiculo in rua.ocupantes
             if self.sim.veiculos[id_veiculo].estado == "movimento"),
            key=lambda veiculo: (veiculo.inicio_trecho, veiculo.id),
        )
        for indice, veiculo in enumerate(em_movimento):
            progresso = (self.sim.agora - veiculo.inicio_trecho) / (
                veiculo.fim_trecho - veiculo.inicio_trecho)
            livre = 0.1 + 0.68 * min(1.0, max(0.0, progresso))
            frente -= intervalo
            # O motor limita quantidade, mas pode admitir vários carros em
            # sequência curta. Reservamos espaço para todos os que vêm atrás.
            minimo = 0.1 + intervalo * (len(em_movimento) - indice - 1)
            posicoes[veiculo.id] = min(max(livre, minimo), frente)
            frente = posicoes[veiculo.id]
        return posicoes

    def desenhar_mapa(self):
        self.caixa((24, 220, 728, 526))
        self.texto("MALHA VIÁRIA", 44, 236, 15, SUAVE)
        self.texto("Clique em um cruzamento para inspecionar", 44, 681, 13, SUAVE)
        # Quadras verdes reforçam a leitura espacial sem cobrir as ruas.
        for y in range(3):
            for x in range(3):
                p = self.pos((x, y))
                self.caixa((p.x + 27, p.y + 25, 123, 70), (24, 49, 48), 9)
                for k in range(3):
                    self.caixa((p.x + 38 + k * 34, p.y + 39, 22, 39), (32, 62, 58), 3)
        caminhos = {a: self.pontos_rua(a) for a in self.sim.cidade.ruas}
        for aresta, rua in self.sim.cidade.ruas.items():
            pontos = caminhos[aresta]
            carga = len(rua.ocupantes) / rua.capacidade
            cor = (49, 68, 85) if carga < 0.5 else (123, 101, 62) if carga < 0.85 else (148, 65, 75)
            pygame.draw.lines(self.tela, cor, False, pontos, 8)
            if rua.nova:
                pygame.draw.lines(self.tela, AZUL, False, pontos, 2)
            meio = self.ao_longo(pontos, 0.5)
            antes = self.ao_longo(pontos, 0.46)
            sentido = (meio - antes).normalize()
            normal = pygame.Vector2(-sentido.y, sentido.x)
            pygame.draw.polygon(self.tela, SUAVE, [meio + sentido * 4, meio - sentido * 4 + normal * 3,
                                                   meio - sentido * 4 - normal * 3])
            sinal = self.sim.semaforos[aresta[1]]
            # Ao lado da faixa, antes do nó: o círculo do cruzamento não cobre
            # a lâmpada e os carros não passam desenhados por cima dela.
            lampada = self.ao_longo(pontos, 0.82) + normal * 11
            aberto = sinal.ativa == aresta
            # A lâmpada indica a fase do semáforo. Espaço na próxima rua é
            # mostrado separadamente no painel e não equivale a sinal verde.
            pygame.draw.circle(self.tela, FUNDO, lampada, 8 if aberto else 6)
            pygame.draw.circle(self.tela, VERDE if aberto else VERMELHO,
                               lampada, 6 if aberto else 4)
            if aberto:
                pygame.draw.circle(self.tela, VERDE, lampada, 9, 1)
        for aresta, rua in self.sim.cidade.ruas.items():
            pontos = caminhos[aresta]
            for id_veiculo, fracao in self.posicoes_rua(rua).items():
                veiculo = self.sim.veiculos[id_veiculo]
                p = self.ao_longo(pontos, fracao)
                pygame.draw.circle(self.tela, AMARELO if veiculo.estado == "fila" else AZUL, p, 4)
        for no in self.sim.cidade.grafo:
            p = self.pos(no)
            pygame.draw.circle(self.tela, FUNDO, p, 17)
            pygame.draw.circle(self.tela, VERDE if no == self.selecionado else BORDA, p, 18, 2)
            superficie = self.fontes[13].render(nome(no), True, TEXTO)
            self.tela.blit(superficie, superficie.get_rect(center=p))
        if self.nova_via:
            self.texto("Via expressa elevada B1 ↔ B4", 44, 263, 13, AZUL)
        for x, cor, rotulo in ((46, AZUL, "Em movimento"), (215, AMARELO, "Na fila"),
                               (334, VERDE, "Sinal aberto"), (500, VERMELHO, "Sinal fechado")):
            pygame.draw.circle(self.tela, cor, (x, 718), 4)
            self.texto(rotulo, x + 12, 707, 15, SUAVE)

    def desenhar_painel(self):
        r = self.sim.resultado()
        self.caixa((772, 220, 484, 151))
        self.texto("RESULTADOS AO VIVO", 794, 234, 15, SUAVE)
        valores = [("Concluídos", str(r["concluidos"]), VERDE),
                   ("Na cidade", str(r["em_circulacao"]), AZUL),
                   ("Fora da rede", str(r["aguardando_entrada"]), AMARELO)]
        for i, (rotulo, valor, cor) in enumerate(valores):
            x = 794 + i * 150
            self.texto(valor, x, 257, 32, cor)
            self.texto(rotulo, x, 297, 15, SUAVE)
        viagem = "—" if r["viagem_media_s"] is None else f'{r["viagem_media_s"]:.1f} s'
        self.texto(f'Viagem média: {viagem}    |    Fila total: {r["fila_atual_total"]}', 794, 325, 15)
        atraso = r.get("atraso_maximo_fase_s", 0.0)
        atendidas = r.get("fases_atendidas", 0)
        atrasadas = r.get("fases_atrasadas", 0)
        self.texto(f'Atraso máximo: {atraso:.1f}s  •  Fases atrasadas: {atrasadas}/{atendidas}',
                   794, 349, 13, VERMELHO if atraso > 0 else SUAVE)
        self.caixa((772, 383, 484, 363))
        self.texto(f"CRUZAMENTO {nome(self.selecionado)}", 794, 396, 20)
        sinal = self.sim.semaforos[self.selecionado]
        fase = "Todos fechados" if sinal.ativa is None else f"Entrada {nome(sinal.ativa[0])} aberta"
        self.texto(fase, 794, 425, 17, VERDE if sinal.ativa else AMARELO)
        self.texto("Entrada", 795, 459, 13, SUAVE)
        self.texto("Fila", 865, 459, 13, SUAVE)
        self.texto("Espera", 913, 459, 13, SUAVE)
        self.texto("Vence em", 974, 459, 13, SUAVE)
        self.texto("Sinal", 1060, 459, 13, SUAVE)
        self.texto("Saída", 1156, 459, 13, SUAVE)
        atuais = self.sim.candidatos(self.selecionado)
        for i, candidato in enumerate(atuais):
            y = 486 + i * 27
            self.texto(nome(candidato.fase[0]), 795, y, 15)
            self.texto(candidato.fila, 865, y, 15, AMARELO)
            self.texto(f"{candidato.espera:.0f}s", 913, y, 15)
            prazo = candidato.prazo
            restante = prazo - self.sim.agora if prazo is not None else None
            self.texto(self.rotulo_prazo(prazo, self.sim.agora), 974, y, 15,
                       VERMELHO if restante is not None and restante < 0 else SUAVE)
            aberto = sinal.ativa == candidato.fase
            self.texto("Verde" if aberto else "Vermelho", 1060, y, 13,
                       VERDE if aberto else VERMELHO)
            self.texto("Livre" if candidato.elegivel else "Cheia" if candidato.fila else "—", 1156, y, 13,
                       VERDE if candidato.elegivel else SUAVE)
        explicacao = {"prazo": "EDD: atende a entrada elegível com prazo mais próximo.",
                      "guloso": "Guloso: maior fila; esperas longas ganham prioridade.",
                      "fixo": "Ciclo fixo: ordem definida antes da simulação."}
        self.texto(explicacao[self.politica], 794, 602, 13, AZUL if self.politica == "prazo" else SUAVE)
        self.texto("ÚLTIMA DECISÃO", 794, 631, 13, SUAVE)
        self.texto_limitado(sinal.motivo, 794, 652, 440, 17)
        instante = sinal.instante_decisao
        escolhas = "  ".join(
            f"{nome(c.fase[0])}:{c.fila}/"
            f"{'BLOQ' if c.fila and not c.elegivel else self.rotulo_prazo(c.prazo, instante)}"
            for c in sinal.candidatos_decisao
        )
        self.texto_limitado(f"Fila/prazo na decisão: {escolhas}", 794, 680, 440, 13, SUAVE)
        self.texto("BLOQ = saída cheia na decisão • prazo negativo = atrasado", 794, 711, 13, SUAVE)

    @staticmethod
    def rotulo_prazo(prazo, instante):
        """Tempo restante para o prazo de atendimento, com sinal explícito."""
        return "—" if prazo is None else f"{prazo - instante:+.0f}s"

    def desenhar(self):
        self.botoes.clear()
        self.tela.fill(FUNDO)
        self.texto("CIDADE EM FLUXO", 24, 18, 32)
        self.texto("G48  /  Scheduling to Minimize Lateness • grafos • agentes", 26, 62, 17, SUAVE)
        self.texto(f"{self.sim.agora:05.1f} / {self.duracao:g} s", 1040, 24, 24, VERDE)
        self.texto("ENCERRADA" if self.sim.encerrada else "SIMULANDO" if self.rodando else "PAUSADA",
                   1120, 62, 13, SUAVE)
        self.caixa((24, 99, 1232, 104))
        self.botao("politica", f"Política: {ROTULOS_POLITICA[self.politica]}", (38, 112, 198, 34),
                   self.politica == "prazo")
        self.botao("via", "Nova via: " + ("Sim" if self.nova_via else "Não"), (246, 112, 162, 34))
        self.botao("demanda", f"Demanda: {self.demanda:g}/min", (418, 112, 196, 34))
        self.botao("semente", f"Semente: {self.semente}", (624, 112, 151, 34))
        self.botao("duracao", f"Duração: {self.duracao:g}s", (785, 112, 175, 34))
        self.botao("velocidade", f"Velocidade: {self.velocidade}x", (970, 112, 270, 34))
        self.botao("iniciar", "Pausar" if self.rodando else "Iniciar", (38, 155, 142, 34), True)
        self.botao("passo", "Próximo evento", (190, 155, 165, 34))
        self.botao("reiniciar", "Reiniciar", (365, 155, 135, 34))
        completo = len(self.resultados) == len(CENARIOS)
        self.botao("comparar", "Ver A–F" if completo else "Comparar A–F",
                   (510, 155, 245, 34))
        self.botao("csv", "Exportar CSV", (765, 155, 171, 34))
        self.texto("Alterar configuração reinicia a execução", 957, 164, 13, SUAVE)
        self.desenhar_mapa()
        self.desenhar_painel()
        self.texto(self.status, 26, 760, 15, SUAVE)
        self.texto("ESPAÇO iniciar/pausar   •   N evento   •   R reiniciar   •   C comparar   •   E exportar   •   ESC fechar painel",
                   26, 791, 15, SUAVE)
        if self.comparacao is not None:
            self.desenhar_progresso()
        elif self.mostrar_resultados:
            self.desenhar_resultados()

    def sombra(self):
        escuro = pygame.Surface((LARGURA, ALTURA), pygame.SRCALPHA)
        escuro.fill((0, 0, 0, 175))
        self.tela.blit(escuro, (0, 0))
        self.botoes.clear()

    def desenhar_progresso(self):
        self.sombra()
        self.caixa((340, 285, 600, 215))
        self.texto("Executando os seis cenários", 372, 317, 24)
        self.texto(f"{len(self.resultados)} de {len(CENARIOS)} concluídos • mesma demanda e semente",
                   372, 363, 17, SUAVE)
        self.botao("cancelar", "Cancelar", (372, 424, 180, 38))

    def desenhar_resultados(self):
        self.sombra()
        self.caixa((75, 150, 1130, 570))
        self.texto("COMPARAÇÃO DOS SEIS CENÁRIOS", 100, 173, 24)
        self.texto(f"Semente {self.semente} • {self.demanda:g} veículos/min • {self.duracao:g}s • mesmos pedidos de viagem",
                   100, 211, 15, SUAVE)
        self.texto("Prazo = chegada à fila + 45 s. EDD escolhe o menor prazo elegível a cada decisão.",
                   100, 237, 15, AZUL)
        xs = (100, 148, 284, 365, 476, 589, 695, 830, 946)
        cabecalho = ("ID", "Controle", "Nova via", "Chegaram", "Viagem¹", "Fila média",
                     "Atraso máx.²", "Fases atr.", "Pendentes")
        for x, rotulo in zip(xs, cabecalho):
            self.texto(rotulo, x, 273, 13, SUAVE)
        for i, r in enumerate(self.resultados):
            y = 309 + i * 37
            self.caixa((94, y - 5, 1084, 32), (28, 43, 60), 5)
            pendentes = r["em_circulacao"] + r["aguardando_entrada"]
            valores = (r["cenario"], ROTULOS_POLITICA[r["politica"]],
                       "Sim" if r["nova_via"] else "Não", r["concluidos"],
                       "—" if r["viagem_media_s"] is None else f'{r["viagem_media_s"]:.1f}s',
                       f'{r["fila_media_total"]:.1f}', f'{r.get("atraso_maximo_fase_s", 0.0):.1f}s',
                       f'{r.get("fases_atrasadas", 0)}/{r.get("fases_atendidas", 0)}', pendentes)
            for x, valor in zip(xs, valores):
                cor = AZUL if r["politica"] == "prazo" and x == xs[0] else VERDE if x == xs[0] else TEXTO
                self.texto(valor, x, y, 15, cor)
        self.texto("¹ Viagem média considera somente quem chegou; inclui espera fora da rede. Fila média é temporal.",
                   100, 542, 13, SUAVE)
        self.texto("² Atraso máximo das fases atendidas; pendentes = veículos na cidade + fora da rede.",
                   100, 566, 13, SUAVE)
        self.texto("Compare chegadas e pendentes junto com as médias para evitar conclusões enganosas.",
                   100, 590, 13, SUAVE)
        self.botao("csv", "Exportar resultados em CSV", (100, 635, 276, 36), True)
        self.botao("fechar", "Voltar à cidade", (968, 635, 207, 36))
        self.texto_limitado(self.status, 100, 689, 1070, 13, SUAVE)

    def iniciar_comparacao(self):
        self.rodando = False
        self.resultados = []
        self.mostrar_resultados = False
        pedidos = gerar_demanda(self.semente, self.demanda, self.duracao)

        def etapas():
            for cenario, politica, nova in CENARIOS:
                sim = Simulacao(politica, nova, self.semente, self.demanda, self.duracao, pedidos)
                while not sim.encerrada:
                    for _ in range(250):
                        if not sim.passo():
                            break
                    yield
                self.resultados.append({"cenario": cenario, **sim.resultado()})
        self.comparacao = etapas()

    def acao(self, chave):
        if chave == "iniciar":
            if self.sim.encerrada:
                self.reiniciar()
            self.rodando = not self.rodando
        elif chave == "passo":
            self.rodando = False
            self.sim.passo()
            self.status = self.sim.ultimo_evento
        elif chave == "reiniciar":
            self.reiniciar()
            self.status = "Execução reiniciada com a mesma semente."
        elif chave == "politica":
            indice = POLITICAS.index(self.politica)
            self.politica = POLITICAS[(indice + 1) % len(POLITICAS)]
            self.reiniciar()
        elif chave == "via":
            self.nova_via = not self.nova_via
            self.reiniciar()
        elif chave == "demanda":
            self.demanda = next((v for v in (20, 45, 80) if v > self.demanda), 20)
            self.reiniciar()
        elif chave == "semente":
            self.semente += 1
            self.reiniciar()
        elif chave == "duracao":
            self.duracao = next((v for v in (120, 300, 600) if v > self.duracao), 120)
            self.reiniciar()
        elif chave == "velocidade":
            self.velocidade = {1: 4, 4: 10, 10: 20, 20: 1}[self.velocidade]
        elif chave == "comparar":
            if len(self.resultados) == len(CENARIOS) and self.comparacao is None:
                self.rodando = False
                self.mostrar_resultados = True
            else:
                self.iniciar_comparacao()
        elif chave == "cancelar":
            self.comparacao = None
            self.resultados = []
            self.status = "Comparação cancelada."
        elif chave == "fechar":
            self.mostrar_resultados = False
        elif chave == "csv":
            linhas = self.resultados or [{"cenario": "atual", **self.sim.resultado()}]
            destino = Path(__file__).resolve().parents[1] / "resultados" / f"comparacao_{datetime.now():%Y%m%d_%H%M%S_%f}.csv"
            try:
                exportar(linhas, destino)
                self.status = f"CSV salvo em resultados/{destino.name}"
            except OSError as erro:
                self.status = f"Não foi possível salvar o CSV: {erro.strerror}"

    def tratar_evento(self, evento):
        if evento.type == pygame.QUIT:
            return False
        if evento.type == pygame.VIDEORESIZE:
            self.janela = pygame.display.set_mode((max(1, evento.w), max(1, evento.h)),
                                                  pygame.RESIZABLE)
            return True
        if evento.type == pygame.KEYDOWN:
            if evento.key == pygame.K_ESCAPE:
                if self.comparacao is not None:
                    self.acao("cancelar")
                self.mostrar_resultados = False
            elif self.comparacao is None:
                if self.mostrar_resultados:
                    if evento.key == pygame.K_e:
                        self.acao("csv")
                else:
                    teclas = {pygame.K_SPACE: "iniciar", pygame.K_n: "passo", pygame.K_r: "reiniciar",
                              pygame.K_c: "comparar", pygame.K_e: "csv"}
                    if evento.key in teclas:
                        self.acao(teclas[evento.key])
        if evento.type == pygame.MOUSEBUTTONDOWN and evento.button == 1:
            pos = self.para_logico(evento.pos)
            if pos is None:
                return True
            for chave, rect in tuple(self.botoes.items()):
                if rect.collidepoint(pos):
                    self.acao(chave)
                    return True
            if not self.mostrar_resultados and self.comparacao is None:
                for no in self.sim.cidade.grafo:
                    if self.pos(no).distance_to(pos) < 25:
                        self.selecionado = no
        return True

    def atualizar(self, segundos):
        if self.comparacao is not None:
            try:
                next(self.comparacao)
            except StopIteration:
                self.comparacao = None
                self.mostrar_resultados = True
                self.status = "Comparação concluída. Exporte o CSV para consultar todas as métricas."
        elif self.rodando:
            self.sim.avancar(self.sim.agora + segundos * self.velocidade)
            self.status = self.sim.ultimo_evento
            if self.sim.encerrada:
                self.rodando = False
                self.status = "Simulação concluída. Compare os seis cenários ou exporte as métricas."

    def executar(self):
        clock = pygame.time.Clock()
        aberta = True
        self.desenhar()
        while aberta:
            dt = min(clock.tick(60) / 1000, 0.1)
            for evento in pygame.event.get():
                if not self.tratar_evento(evento):
                    aberta = False
            self.atualizar(dt)
            self.desenhar()
            self.apresentar()
        pygame.quit()
