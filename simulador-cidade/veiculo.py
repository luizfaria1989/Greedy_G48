"""Um agente veicular percorre sua rota e respeita filas FIFO e capacidade."""

from dataclasses import dataclass
from cidade import No, Aresta


@dataclass(frozen=True)
class Pedido:
    instante: float
    origem: No
    destino: No


@dataclass
class Veiculo:
    id: int
    pedido: Pedido
    rota: list[No]
    indice: int = 0
    estado: str = "externo"
    entrou: float | None = None
    terminou: float | None = None
    inicio_trecho: float = 0.0
    fim_trecho: float = 0.0
    inicio_espera: float | None = None
    espera: float = 0.0
    maior_espera: float = 0.0

    @property
    def aresta(self) -> Aresta:
        return self.rota[self.indice], self.rota[self.indice + 1]

    @property
    def proxima(self) -> Aresta | None:
        if self.indice + 2 < len(self.rota):
            return self.rota[self.indice + 1], self.rota[self.indice + 2]
        return None

    def espera_atual(self, agora: float) -> float:
        return 0.0 if self.inicio_espera is None else agora - self.inicio_espera
