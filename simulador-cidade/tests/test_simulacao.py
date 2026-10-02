import csv
from pathlib import Path
import tempfile
import unittest

from algoritmos import Candidato, escolher_guloso, escolher_prazo
from cidade import Cidade
from experimentos import comparar, exportar
from simulacao import Simulacao, gerar_demanda
from veiculo import Pedido


class PoliticaTest(unittest.TestCase):
    def candidato(self, id, fila, espera=0, ultimo=0, elegivel=True, prazo=None):
        return Candidato(((id, 0), (1, 1)), fila, espera, ultimo, elegivel, prazo)

    def test_maior_fila_e_bloqueio(self):
        pequena = self.candidato(0, 4)
        grande = self.candidato(1, 10)
        bloqueada = self.candidato(2, 20, elegivel=False)
        self.assertEqual(escolher_guloso([pequena, grande, bloqueada])[0], grande.fase)

    def test_limite_de_espera(self):
        antiga = self.candidato(0, 1, espera=46)
        grande = self.candidato(1, 12, espera=12)
        self.assertEqual(escolher_guloso([grande, antiga])[0], antiga.fase)

    def test_desempate_e_ausencia_de_candidatos(self):
        a = self.candidato(0, 3, ultimo=20)
        b = self.candidato(1, 3, ultimo=10)
        self.assertEqual(escolher_guloso([a, b])[0], b.fase)
        self.assertEqual(escolher_guloso([a, self.candidato(1, 3, ultimo=20)])[0], a.fase)
        self.assertIsNone(escolher_guloso([self.candidato(0, 0)])[0])

    def test_prazo_prioriza_vencimento_e_ignora_rua_bloqueada(self):
        pequena_urgente = self.candidato(0, 1, prazo=50)
        grande = self.candidato(1, 9, prazo=70)
        bloqueada = self.candidato(2, 12, elegivel=False, prazo=40)
        fase, motivo = escolher_prazo([grande, bloqueada, pequena_urgente], 10)
        self.assertEqual(fase, pequena_urgente.fase)
        self.assertIn("EDD", motivo)
        self.assertIsNone(escolher_prazo([bloqueada], 10)[0])


class MotorTest(unittest.TestCase):
    def test_prazo_da_fila_e_decisao_edd_no_cruzamento(self):
        pedidos = [Pedido(0, (1, 0), (1, 2))]
        pedidos += [Pedido(t, (0, 1), (2, 1)) for t in (0.2, 0.4, 0.6)]
        sim = Simulacao("prazo", duracao=40, pedidos=pedidos)
        sim.avancar(10)
        sinal = sim.semaforos[(1, 1)]
        self.assertEqual(sinal.instante_decisao, 10)
        self.assertEqual(sinal.proxima, ((1, 0), (1, 1)))
        self.assertEqual(sinal.prazo_em_atendimento, 54)
        self.assertAlmostEqual(sinal.candidatos_decisao[0].prazo, 54.2)
        self.assertEqual(sinal.candidatos_decisao[1].prazo, 54)
        sim.executar()
        resultado = sim.resultado()
        self.assertEqual(resultado["politica"], "prazo")
        self.assertGreaterEqual(resultado["fases_atendidas"], 1)
        self.assertIn("atraso_maximo_fase_s", resultado)

    def test_prazo_mede_fases_vencidas_em_simulacao_completa(self):
        resultado = Simulacao("prazo", semente=42, demanda=45, duracao=180).executar()
        self.assertGreater(resultado["fases_atendidas"], 0)
        self.assertGreater(resultado["fases_atrasadas"], 0)
        self.assertLessEqual(resultado["fases_atrasadas"], resultado["fases_atendidas"])
        self.assertGreater(resultado["atraso_maximo_fase_s"], 0)

    def test_rota_da_nova_via(self):
        self.assertEqual(len(Cidade().rota((0, 1), (3, 1))), 4)
        self.assertEqual(Cidade(True).rota((0, 1), (3, 1)), [(0, 1), (3, 1)])

    def test_viagem_com_espera_conhecida(self):
        sim = Simulacao(duracao=30, pedidos=[Pedido(0, (0, 1), (2, 1))])
        r = sim.executar()
        self.assertEqual(r["concluidos"], 1)
        self.assertEqual(r["viagem_media_s"], 21)
        self.assertEqual(r["espera_media_s"], 3)
        self.assertEqual(r["fila_media_total"], 0.1)

    def test_todos_os_eventos_no_horizonte_sao_processados(self):
        pedidos = [Pedido(0, (0, 0), (1, 0)), Pedido(0, (3, 3), (2, 3))]
        sim = Simulacao(duracao=9, pedidos=pedidos)
        self.assertEqual(sim.executar()["concluidos"], 2)
        self.assertTrue(sim.encerrada)
        self.assertEqual(sim.agora, 9)

    def test_espera_ainda_aberta_entra_na_metrica(self):
        sim = Simulacao(duracao=10, pedidos=[Pedido(0, (0, 1), (2, 1))])
        r = sim.executar()
        self.assertEqual(r["espera_media_s"], 1)
        self.assertEqual(r["espera_maxima_s"], 1)
        self.assertEqual(r["concluidos"], 0)
        self.assertIsNone(r["viagem_media_s"])

    def test_determinismo_independente_da_renderizacao(self):
        a = Simulacao(duracao=90)
        b = Simulacao(duracao=90)
        c = Simulacao(duracao=90)
        a.executar()
        for t in range(1, 91):
            b.avancar(t)
        while not c.encerrada:
            c.passo()
        self.assertEqual(a.resultado(), b.resultado())
        self.assertEqual(a.resultado(), c.resultado())

    def test_conservacao_capacidade_filas_e_relogio(self):
        for politica in ("fixo", "guloso", "prazo"):
            for nova in (False, True):
                sim = Simulacao(politica, nova, demanda=140, duracao=90)
                anterior = 0
                while not sim.encerrada:
                    sim.passo()
                    self.assertGreaterEqual(sim.agora, anterior)
                    self.assertLessEqual(sim.agora, sim.duracao)
                    anterior = sim.agora
                    ocupantes = []
                    for rua in sim.cidade.ruas.values():
                        self.assertLessEqual(len(rua.ocupantes), rua.capacidade)
                        self.assertTrue(set(rua.fila).issubset(rua.ocupantes))
                        ocupantes.extend(rua.ocupantes)
                        for id in rua.fila:
                            self.assertEqual(sim.veiculos[id].estado, "fila")
                    self.assertEqual(len(ocupantes), len(set(ocupantes)))
                    dentro = {v.id for v in sim.veiculos.values() if v.estado in ("fila", "movimento")}
                    self.assertEqual(set(ocupantes), dentro)
                r = sim.resultado()
                self.assertEqual(r["gerados"], r["concluidos"] + r["em_circulacao"] + r["aguardando_entrada"])

    def test_saida_bloqueada_preserva_fila(self):
        sim = Simulacao(duracao=30, pedidos=[Pedido(0, (0, 1), (2, 1))])
        destino = sim.cidade.ruas[((1, 1), (2, 1))]
        destino.capacidade = 0
        r = sim.executar()
        self.assertEqual(r["concluidos"], 0)
        self.assertEqual(r["fila_atual_total"], 1)
        self.assertEqual(r["espera_maxima_s"], 21)

    def test_carros_na_mesma_rua_saem_na_ordem_da_fila(self):
        pedidos = [Pedido(t, (0, 1), (2, 1)) for t in (0, 1, 2)]
        sim = Simulacao(duracao=30, pedidos=pedidos)
        origem = sim.cidade.ruas[((0, 1), (1, 1))]
        destino = sim.cidade.ruas[((1, 1), (2, 1))]
        destino.capacidade = 0
        sim.avancar(15)
        self.assertEqual(list(origem.fila), [0, 1, 2])
        self.assertEqual(origem.ocupantes, {0, 1, 2})

        destino.capacidade = 9
        for instante, fila, indice_movido in (
            (22, [1, 2], 0), (24, [2], 1), (26, [], 2)
        ):
            sim.avancar(instante)
            self.assertEqual(list(origem.fila), fila)
            self.assertEqual(origem.ocupantes, set(fila))
            self.assertEqual(sim.veiculos[indice_movido].aresta, ((1, 1), (2, 1)))

    def test_transicao_sem_liberacao_prematura(self):
        sim = Simulacao(duracao=30, pedidos=[Pedido(0, (0, 1), (2, 1))])
        sim.avancar(11)
        self.assertIsNone(sim.semaforos[(1, 1)].ativa)
        self.assertEqual(sim.veiculos[0].estado, "fila")
        sim.avancar(12)
        self.assertEqual(sim.veiculos[0].estado, "movimento")

    def test_chegada_simultanea_a_liberacao_usa_o_verde(self):
        sim = Simulacao("fixo", duracao=30,
                        pedidos=[Pedido(1, (0, 1), (2, 1))])
        resultado = sim.executar()
        self.assertEqual(resultado["viagem_media_s"], 18)
        self.assertEqual(resultado["espera_media_s"], 0)

    def test_chegada_simultanea_a_decisao_e_considerada(self):
        sim = Simulacao("guloso", duracao=30,
                        pedidos=[Pedido(1, (0, 1), (2, 1))])
        resultado = sim.executar()
        self.assertEqual(resultado["viagem_media_s"], 20)
        self.assertEqual(resultado["espera_media_s"], 2)

    def test_admissao_usa_espaco_aberto_no_mesmo_instante(self):
        sim = Simulacao(duracao=30,
                        pedidos=[Pedido(0, (0, 1), (2, 1))] * 2)
        sim.cidade.ruas[((0, 1), (1, 1))].capacidade = 1
        sim.avancar(12)
        self.assertEqual(sim.veiculos[1].entrou, 12)

    def test_sem_pedidos_e_parametros_invalidos(self):
        sim = Simulacao(duracao=10, pedidos=[])
        self.assertEqual(sim.executar()["gerados"], 0)
        for demanda in (0, -1, float("nan"), float("inf")):
            with self.assertRaises(ValueError):
                gerar_demanda(42, demanda, 30)
        invalidos = (
            Pedido(float("nan"), (0, 1), (3, 1)),
            Pedido(0, (1, 1), (3, 1)),  # origem interior, sem entrada externa
            Pedido(0, (0, 1), (9, 9)),
        )
        for pedido in invalidos:
            with self.subTest(pedido=pedido), self.assertRaises(ValueError):
                Simulacao(duracao=10, pedidos=[pedido])
        with self.assertRaises(ValueError):
            Simulacao("inexistente", duracao=10, pedidos=[])


class ExperimentosTest(unittest.TestCase):
    def test_demanda_pareada_e_exportacao(self):
        resultados = list(comparar((42, 43), (20,), 40))
        self.assertEqual(len(resultados), 12)
        for bloco in (resultados[:6], resultados[6:]):
            self.assertEqual(len({r["gerados"] for r in bloco}), 1)
            self.assertEqual([r["cenario"] for r in bloco], list("ABCDEF"))
        with tempfile.TemporaryDirectory() as pasta:
            caminho = exportar(resultados, Path(pasta) / "resultados.csv")
            with caminho.open(encoding="utf-8-sig", newline="") as arquivo:
                linhas = list(csv.DictReader(arquivo, delimiter=";"))
            self.assertEqual(len(linhas), 12)
            self.assertEqual(linhas[0]["cenario"], "A")


if __name__ == "__main__":
    unittest.main()
