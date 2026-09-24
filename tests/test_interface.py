"""Smoke test da interface com SDL virtual, sem abrir uma janela no desktop."""

import os
os.environ.setdefault("SDL_VIDEODRIVER", "dummy")
os.environ.setdefault("SDL_AUDIODRIVER", "dummy")
os.environ.setdefault("PYGAME_HIDE_SUPPORT_PROMPT", "1")

import unittest
import pygame
from experimentos import CENARIOS
from render.interface import Aplicacao


class InterfaceTest(unittest.TestCase):
    def test_controles_renderizacao_e_comparacao(self):
        app = Aplicacao(duracao=30)
        try:
            self.assertEqual(app.politica, "prazo")
            app.desenhar()
            clique = pygame.event.Event(pygame.MOUSEBUTTONDOWN, button=1,
                                        pos=app.para_tela(app.botoes["iniciar"].center))
            self.assertTrue(app.tratar_evento(clique))
            self.assertTrue(app.rodando)
            app.atualizar(1)
            self.assertEqual(app.sim.agora, 4)
            app.status = "Mensagem da configuração anterior"
            app.acao("via")
            self.assertTrue(app.sim.nova_via)
            self.assertEqual(app.sim.agora, 0)
            self.assertIn("Configuração pronta", app.status)
            app.sim.avancar(20)
            app.desenhar()
            app.acao("comparar")
            for _ in range(1000):
                app.atualizar(0)
                if app.comparacao is None:
                    break
            self.assertTrue(app.mostrar_resultados)
            self.assertEqual(len(app.resultados), len(CENARIOS))
            self.assertEqual([r["cenario"] for r in app.resultados], list("ABCDEF"))
            self.assertEqual([r["politica"] for r in app.resultados[-2:]], ["prazo", "prazo"])
            app.desenhar()
            self.assertEqual(set(app.botoes), {"csv", "fechar"})
            app.tratar_evento(pygame.event.Event(pygame.KEYDOWN, key=pygame.K_ESCAPE))
            self.assertFalse(app.mostrar_resultados)
            app.acao("comparar")
            self.assertTrue(app.mostrar_resultados)
            self.assertIsNone(app.comparacao)
        finally:
            pygame.quit()

    def test_alternancia_das_tres_politicas_e_rotulo_de_prazo(self):
        app = Aplicacao(duracao=30)
        try:
            self.assertEqual(app.politica, "prazo")
            for esperada in ("fixo", "guloso", "prazo"):
                app.acao("politica")
                self.assertEqual(app.politica, esperada)
                self.assertEqual(app.sim.politica, esperada)
                self.assertEqual(app.sim.agora, 0)
                app.desenhar()
            self.assertEqual(app.rotulo_prazo(None, 10), "—")
            self.assertEqual(app.rotulo_prazo(22, 10), "+12s")
            self.assertEqual(app.rotulo_prazo(7, 10), "-3s")
        finally:
            pygame.quit()

    def test_redimensionamento_preserva_cliques_e_proporcao(self):
        app = Aplicacao(duracao=30)
        try:
            for largura, altura in ((1366, 768), (1280, 720)):
                app.tratar_evento(pygame.event.Event(pygame.VIDEORESIZE,
                                                      w=largura, h=altura))
                area = app.area_visual()
                self.assertLessEqual(area.right, largura)
                self.assertLessEqual(area.bottom, altura)
                self.assertAlmostEqual(area.width / area.height, 1280 / 840, places=2)
            app.desenhar()
            app.apresentar()
            self.assertEqual(app.para_logico((0, 360)), None)
            self.assertFalse(app.rodando)
            app.tratar_evento(pygame.event.Event(pygame.MOUSEBUTTONDOWN, button=1,
                                                  pos=(0, 360)))
            self.assertFalse(app.rodando)
            app.tratar_evento(pygame.event.Event(pygame.MOUSEBUTTONDOWN, button=1,
                                                  pos=app.para_tela(app.botoes["iniciar"].center)))
            self.assertTrue(app.rodando)
            cruzamento = (3, 3)
            app.tratar_evento(pygame.event.Event(pygame.MOUSEBUTTONDOWN, button=1,
                                                  pos=app.para_tela(app.pos(cruzamento))))
            self.assertEqual(app.selecionado, cruzamento)
        finally:
            pygame.quit()


if __name__ == "__main__":
    unittest.main()
