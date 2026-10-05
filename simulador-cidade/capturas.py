"""Gera capturas reais e reproduzíveis do renderizador para o README."""

import os
os.environ.setdefault("SDL_VIDEODRIVER", "dummy")
os.environ.setdefault("SDL_AUDIODRIVER", "dummy")
os.environ.setdefault("PYGAME_HIDE_SUPPORT_PROMPT", "1")

from pathlib import Path
import pygame
from render.interface import Aplicacao
from experimentos import comparar


def main():
    destino = Path(__file__).resolve().parent / "assets"
    destino.mkdir(exist_ok=True)
    app = Aplicacao()
    try:
        app.sim.avancar(146)
        app.status = "Política EDD: observe os prazos e a entrada escolhida pelo semáforo."
        app.desenhar()
        pygame.image.save(app.tela, str(destino / "cidade.png"))
        app.acao("via")
        app.sim.avancar(150)
        app.selecionado = (3, 1)
        app.status = "Malha com a via expressa elevada entre B1 e B4."
        app.desenhar()
        pygame.image.save(app.tela, str(destino / "nova-via.png"))
        app.resultados = list(comparar())
        app.mostrar_resultados = True
        app.status = "Comparação concluída com os mesmos pedidos nos seis cenários."
        app.desenhar()
        pygame.image.save(app.tela, str(destino / "comparacao.png"))
    finally:
        pygame.quit()
    print(f"Capturas salvas em {destino}")


if __name__ == "__main__":
    main()
