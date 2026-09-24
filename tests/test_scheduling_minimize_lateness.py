"""Propriedades do EDD clássico, além dos testes da simulação."""

from itertools import permutations
import random
import subprocess
import sys
import unittest

from scheduling_minimize_lateness import Tarefa, agendar, ordenar_por_prazo


def atraso_maximo_da_ordem(tarefas, inicio=0):
    tempo = inicio
    atrasos = []
    for tarefa in tarefas:
        tempo += tarefa.duracao
        atrasos.append(tempo - tarefa.prazo)
    return max(atrasos, default=0)


class AgendamentoEDDTest(unittest.TestCase):
    def test_exemplo_terminos_e_atrasos_assinados(self):
        tarefas = [Tarefa("A", 4, 10), Tarefa("B", 2, 5), Tarefa("C", 3, 8)]
        resultado = agendar(tarefas)
        self.assertEqual(
            [(e.tarefa.identificador, e.inicio, e.fim, e.atraso)
             for e in resultado.execucoes],
            [("B", 0, 2, -3), ("C", 2, 5, -3), ("A", 5, 9, -1)],
        )
        self.assertEqual(resultado.atraso_maximo, -1)
        self.assertEqual(resultado.atraso_maximo_positivo, 0)

    def test_empate_preserva_ordem_e_nao_depende_do_identificador(self):
        tarefas = [
            Tarefa((2, "x"), 1, 5),
            Tarefa((1, "y"), 2, 5),
            Tarefa("primeira", 1, 2),
        ]
        self.assertEqual(
            [t.identificador for t in ordenar_por_prazo(iter(tarefas))],
            ["primeira", (2, "x"), (1, "y")],
        )

    def test_inicio_arbitrario_e_entrada_vazia(self):
        resultado = agendar([Tarefa("X", 3, 11)], inicio=10)
        self.assertEqual((resultado.execucoes[0].inicio, resultado.execucoes[0].fim), (10, 13))
        self.assertEqual(resultado.atraso_maximo, 2)
        self.assertEqual(resultado.execucoes[0].atraso_positivo, 2)
        self.assertEqual(agendar([]).atraso_maximo, 0)

    def test_edd_atinge_menor_atraso_maximo_em_instancias_pequenas(self):
        sorteio = random.Random(84307)
        for quantidade in range(1, 7):
            for _ in range(12):
                tarefas = [
                    Tarefa(indice, sorteio.randint(1, 8), sorteio.randint(-2, 15))
                    for indice in range(quantidade)
                ]
                obtido = agendar(tarefas).atraso_maximo
                otimo = min(atraso_maximo_da_ordem(ordem) for ordem in permutations(tarefas))
                self.assertEqual(obtido, otimo, tarefas)

    def test_valores_invalidos(self):
        for duracao in (0, -1, float("nan"), float("inf"), "2"):
            with self.assertRaises(ValueError):
                Tarefa("x", duracao, 3)
        for prazo in (float("nan"), float("-inf"), "3"):
            with self.assertRaises(ValueError):
                Tarefa("x", 2, prazo)
        for inicio in (float("nan"), float("inf"), "0"):
            with self.assertRaises(ValueError):
                agendar([Tarefa("x", 2, 3)], inicio)
        with self.assertRaises(TypeError):
            ordenar_por_prazo([Tarefa("x", 2, 3), ("y", 1, 2)])

    def test_demonstracao_executavel(self):
        resultado = subprocess.run(
            [sys.executable, "-m", "scheduling_minimize_lateness"],
            capture_output=True,
            text=True,
            check=True,
        )
        self.assertIn("Ordem EDD: B -> A -> C", resultado.stdout)
        self.assertIn("Atraso maximo (L_max): 1", resultado.stdout)
        self.assertIn("Atraso maximo na ordem de entrada: 3", resultado.stdout)


if __name__ == "__main__":
    unittest.main()
