"""Malha dirigida; filas ocupam espaço na própria rua de chegada."""

from dataclasses import dataclass, field
from collections import deque
import networkx as nx

No = tuple[int, int]
Aresta = tuple[No, No]


@dataclass
class Rua:
    origem: No
    destino: No
    comprimento: float = 120.0  # metros
    velocidade: float = 48.0  # km/h
    capacidade: int = 9
    nova: bool = False
    ocupantes: set[int] = field(default_factory=set)
    fila: deque[int] = field(default_factory=deque)

    @property
    def tempo(self) -> float:
        return self.comprimento / (self.velocidade / 3.6)

    @property
    def livre(self) -> bool:
        return len(self.ocupantes) < self.capacidade


class Cidade:
    def __init__(self, nova_via: bool = False):
        self.grafo = nx.DiGraph()
        self.ruas: dict[Aresta, Rua] = {}
        for y in range(4):
            for x in range(4):
                self.grafo.add_node((x, y), pos=(x, y))
        for x, y in self.grafo.nodes:
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                vizinho = x + dx, y + dy
                if vizinho in self.grafo:
                    self._adicionar((x, y), vizinho)
        # Via expressa elevada, sem cruzamentos intermediários, em dois sentidos.
        if nova_via:
            self._adicionar((0, 1), (3, 1), comprimento=300.0, velocidade=72.0, capacidade=15, nova=True)
            self._adicionar((3, 1), (0, 1), comprimento=300.0, velocidade=72.0, capacidade=15, nova=True)
        self.entradas = sorted(n for n in self.grafo if n[0] in (0, 3) or n[1] in (0, 3))

    def _adicionar(self, origem: No, destino: No, **opcoes):
        rua = Rua(origem, destino, **opcoes)
        self.ruas[origem, destino] = rua
        self.grafo.add_edge(origem, destino, weight=rua.tempo)

    def rota(self, origem: No, destino: No) -> list[No]:
        return nx.dijkstra_path(self.grafo, origem, destino, weight="weight")

    def fases(self, no: No) -> list[Aresta]:
        # Uma aproximação por fase: conversões de entradas diferentes não conflitam.
        return sorted((origem, no) for origem in self.grafo.predecessors(no))


def nome(no: No) -> str:
    return f"{chr(65 + no[1])}{no[0] + 1}"
