"""Exemplo reproduzível: python -m scheduling_minimize_lateness."""

from . import Tarefa, agendar


def main() -> None:
    tarefas = [
        Tarefa("A", duracao=4, prazo=6),
        Tarefa("B", duracao=3, prazo=4),
        Tarefa("C", duracao=2, prazo=9),
    ]
    edd = agendar(tarefas)
    ordem_original = " -> ".join(str(tarefa.identificador) for tarefa in tarefas)
    print("Scheduling to Minimize Lateness - Earliest Due Date (EDD)")
    print(f"Ordem de entrada: {ordem_original}")
    print("Ordem EDD: " + " -> ".join(str(e.tarefa.identificador) for e in edd.execucoes))
    print("Tarefa | duracao | prazo | inicio | conclusao | atraso (conclusao - prazo)")
    for e in edd.execucoes:
        print(
            f"{str(e.tarefa.identificador):>7} | {e.tarefa.duracao:>7g} | "
            f"{e.tarefa.prazo:>5g} | {e.inicio:>6g} | {e.fim:>9g} | {e.atraso:>7g}"
        )
    print(f"Atraso maximo (L_max): {edd.atraso_maximo:g}")
    print(f"Atraso maximo na ordem de entrada: {agendar_na_ordem(tarefas):g}")


def agendar_na_ordem(tarefas: list[Tarefa]) -> float:
    """Calcula L_max de uma ordem dada, apenas para ilustrar a comparação."""
    tempo = 0.0
    atrasos = []
    for tarefa in tarefas:
        tempo += tarefa.duracao
        atrasos.append(tempo - tarefa.prazo)
    return max(atrasos, default=0.0)


if __name__ == "__main__":
    main()
