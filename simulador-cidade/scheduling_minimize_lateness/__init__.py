"""Scheduling to Minimize Lateness (EDD) em uma máquina.

O teorema de otimalidade vale quando todas as tarefas estão disponíveis no
instante inicial, a máquina executa uma tarefa por vez e não há interrupções.
O trânsito usa a mesma ordem por prazo como política local; essas hipóteses
não descrevem a simulação inteira.
"""

from dataclasses import dataclass
from math import isfinite
from typing import Hashable, Iterable


def _numero_finito(valor: float, nome: str) -> None:
    if isinstance(valor, bool):
        raise ValueError(f"{nome} deve ser um número finito")
    try:
        finito = isfinite(valor)
    except (TypeError, ValueError, OverflowError) as erro:
        raise ValueError(f"{nome} deve ser um número finito") from erro
    if not finito:
        raise ValueError(f"{nome} deve ser um número finito")


@dataclass(frozen=True)
class Tarefa:
    """Trabalho indivisível com duração e prazo de conclusão."""

    identificador: Hashable
    duracao: float
    prazo: float

    def __post_init__(self) -> None:
        _numero_finito(self.duracao, "duração")
        _numero_finito(self.prazo, "prazo")
        if self.duracao <= 0:
            raise ValueError("duração deve ser maior que zero")


@dataclass(frozen=True)
class Execucao:
    """Resultado de uma tarefa; atraso pode ser negativo se terminar cedo."""

    tarefa: Tarefa
    inicio: float
    fim: float
    atraso: float

    @property
    def atraso_positivo(self) -> float:
        """Tempo de entrega após o prazo, limitado inferiormente a zero."""
        return max(0.0, self.atraso)


@dataclass(frozen=True)
class Agendamento:
    execucoes: tuple[Execucao, ...]
    atraso_maximo: float

    @property
    def atraso_maximo_positivo(self) -> float:
        return max(0.0, self.atraso_maximo)


def ordenar_por_prazo(tarefas: Iterable[Tarefa]) -> tuple[Tarefa, ...]:
    """Ordena por prazo de conclusão crescente (EDD), preservando empates."""
    recebidas = tuple(tarefas)
    if any(not isinstance(tarefa, Tarefa) for tarefa in recebidas):
        raise TypeError("todas as entradas devem ser Tarefa")
    return tuple(sorted(recebidas, key=lambda tarefa: tarefa.prazo))


def agendar(tarefas: Iterable[Tarefa], inicio: float = 0.0) -> Agendamento:
    """Calcula o cronograma EDD e L_max = max(C_j - d_j).

    Uma sequência vazia tem atraso máximo definido como zero. Quando todas
    as tarefas terminam antes do prazo, L_max é negativo. Use
    ``atraso_maximo_positivo`` para obter a demora não negativa.
    """
    _numero_finito(inicio, "instante inicial")
    tempo = inicio
    execucoes = []
    for tarefa in ordenar_por_prazo(tarefas):
        fim = tempo + tarefa.duracao
        atraso = fim - tarefa.prazo
        execucoes.append(Execucao(tarefa, tempo, fim, atraso))
        tempo = fim
    return Agendamento(
        tuple(execucoes),
        max((execucao.atraso for execucao in execucoes), default=0.0),
    )


__all__ = ["Tarefa", "Execucao", "Agendamento", "ordenar_por_prazo", "agendar"]
