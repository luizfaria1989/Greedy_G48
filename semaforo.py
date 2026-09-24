from dataclasses import dataclass, field
from cidade import Aresta
from algoritmos import Candidato, escolher_fixo, escolher_guloso, escolher_prazo

VERDE = 10.0
TRANSICAO = 2.0
INTERVALO_SAIDA = 2.0
LIMITE_ESPERA = 45.0


@dataclass
class Semaforo:
    fases: list[Aresta]
    politica: str
    ativa: Aresta | None = None
    proxima: Aresta | None = None
    motivo: str = "Aguardando primeira decisão"
    candidatos_decisao: list[Candidato] = field(default_factory=list)
    ultimo: dict[Aresta, float] = field(default_factory=dict)
    ciclo: int = 0
    geracao: int = 0
    fim_verde: float = 0.0
    instante_decisao: float = 0.0
    prazo_em_atendimento: float | None = None

    def __post_init__(self):
        self.ultimo = {fase: 0.0 for fase in self.fases}

    def escolher(self, candidatos: list[Candidato]):
        self.candidatos_decisao = candidatos
        if self.politica == "guloso":
            fase, self.motivo = escolher_guloso(candidatos, LIMITE_ESPERA)
        elif self.politica == "prazo":
            fase, self.motivo = escolher_prazo(candidatos, VERDE)
        else:
            fase, self.motivo = escolher_fixo(self.fases, self.ciclo)
        self.ciclo += 1
        return fase
