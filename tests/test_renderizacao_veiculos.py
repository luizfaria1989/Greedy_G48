"""Regressões visuais do mapa em SDL virtual."""

import os
os.environ.setdefault("SDL_VIDEODRIVER", "dummy")
os.environ.setdefault("SDL_AUDIODRIVER", "dummy")
os.environ.setdefault("PYGAME_HIDE_SUPPORT_PROMPT", "1")

import unittest
import pygame

from render.interface import Aplicacao, VERDE, VERMELHO


class RenderizacaoVeiculosTest(unittest.TestCase):
    def test_movimento_fica_atras_da_fila_na_mesma_rua(self):
        app = Aplicacao()
        try:
            app.politica = "guloso"
            app.reiniciar()
            app.sim.avancar(150)
            rua = app.sim.cidade.ruas[((0, 1), (1, 1))]
            self.assertEqual(len(rua.fila), 4)  # situação congestionada da captura
            self.assertEqual(len(rua.ocupantes), 9)
            posicoes = app.posicoes_rua(rua)
            ordem = list(rua.fila) + sorted(
                (id_veiculo for id_veiculo in rua.ocupantes if id_veiculo not in rua.fila),
                key=lambda id_veiculo: (
                    app.sim.veiculos[id_veiculo].inicio_trecho, id_veiculo))
            fracao = [posicoes[id_veiculo] for id_veiculo in ordem]
            self.assertTrue(all(a > b for a, b in zip(fracao, fracao[1:])))
            self.assertGreaterEqual(fracao[-1], 0.1 - 1e-9)

            primeiro_azul = app.sim.veiculos[ordem[len(rua.fila)]]
            progresso_livre = 0.1 + 0.68 * (
                (app.sim.agora - primeiro_azul.inicio_trecho) /
                (primeiro_azul.fim_trecho - primeiro_azul.inicio_trecho))
            self.assertGreater(progresso_livre, fracao[len(rua.fila) - 1])
            self.assertLess(posicoes[primeiro_azul.id], fracao[len(rua.fila) - 1])
        finally:
            pygame.quit()

    def test_lampada_reflete_fase_aberta_e_fechada(self):
        app = Aplicacao()
        try:
            app.sim.avancar(150)
            app.desenhar()
            no, sinal = next((no, sinal) for no, sinal in app.sim.semaforos.items()
                             if sinal.ativa is not None)
            for fase in sinal.fases:
                pontos = app.pontos_rua(fase)
                meio = app.ao_longo(pontos, 0.5)
                antes = app.ao_longo(pontos, 0.46)
                sentido = (meio - antes).normalize()
                normal = pygame.Vector2(-sentido.y, sentido.x)
                lampada = app.ao_longo(pontos, 0.82) + normal * 11
                pixel = app.tela.get_at((round(lampada.x), round(lampada.y)))[:3]
                self.assertEqual(pixel, VERDE if fase == sinal.ativa else VERMELHO)
        finally:
            pygame.quit()

    def test_rua_cheia_mantem_todos_visiveis_em_ordem(self):
        app = Aplicacao(semente=43, demanda=80)
        try:
            app.politica = "guloso"
            app.reiniciar()
            app.sim.avancar(32)
            rua = app.sim.cidade.ruas[((3, 1), (2, 1))]
            self.assertEqual(len(rua.ocupantes), rua.capacidade)
            posicoes = app.posicoes_rua(rua)
            ordem = list(rua.fila) + sorted(
                (id_veiculo for id_veiculo in rua.ocupantes if id_veiculo not in rua.fila),
                key=lambda id_veiculo: (
                    app.sim.veiculos[id_veiculo].inicio_trecho, id_veiculo))
            posicoes_ordenadas = [posicoes[id_veiculo] for id_veiculo in ordem]
            self.assertTrue(all(a > b for a, b in zip(posicoes_ordenadas, posicoes_ordenadas[1:])))
            self.assertGreaterEqual(posicoes_ordenadas[-1], 0.1 - 1e-9)
        finally:
            pygame.quit()


if __name__ == "__main__":
    unittest.main()
