"""Políticas puras, independentes da interface e do motor de eventos."""

from dataclasses import dataclass
from cidade import Aresta
from scheduling_minimize_lateness import Tarefa, ordenar_por_prazo


@dataclass(frozen=True)
class Candidato:
    fase: Aresta
    fila: int
    espera: float
    ultimo_atendimento: float
    elegivel: bool
    prazo: float | None = None


def escolher_guloso(candidatos: list[Candidato], limite: float = 45.0):
    elegiveis = [c for c in candidatos if c.elegivel and c.fila > 0]
    if not elegiveis:
        return None, "Sem fila com espaço na saída"
    urgentes = [c for c in elegiveis if c.espera >= limite]
    if urgentes:
        escolhido = min(urgentes, key=lambda c: (-c.espera, c.ultimo_atendimento, c.fase))
        return escolhido.fase, f"Limite de espera: {escolhido.espera:.0f} s"
    # Maior fila primeiro; depois a fase há mais tempo sem atendimento e seu ID.
    escolhido = min(elegiveis, key=lambda c: (-c.fila, c.ultimo_atendimento, c.fase))
    return escolhido.fase, f"Maior fila elegível: {escolhido.fila} veículos"


def escolher_fixo(fases: list[Aresta], indice: int):
    return fases[indice % len(fases)], "Ciclo fixo: ordem predefinida"


def escolher_prazo(candidatos: list[Candidato], duracao_verde: float):
    """EDD: entre fases viáveis, escolhe a que vence primeiro.

    O algoritmo clássico considera todas as tarefas disponíveis e sem bloqueios.
    Aqui, as filas chegam durante a simulação e ruas cheias podem impedir uma fase.
    """
    elegiveis = [c for c in candidatos if c.elegivel and c.fila > 0]
    if not elegiveis:
        return None, "Sem fila com espaço na saída"
    if any(c.prazo is None for c in elegiveis):
        raise ValueError("Fila elegível sem prazo de atendimento.")
    tarefas = (Tarefa(c.fase, duracao_verde, c.prazo) for c in elegiveis)
    escolhida = ordenar_por_prazo(tarefas)[0]
    return escolhida.identificador, f"Menor prazo (EDD): {escolhida.prazo:.1f} s"
