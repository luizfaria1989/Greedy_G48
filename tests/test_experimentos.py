"""Verifica o pareamento das comparações e a validação dos argumentos."""

from contextlib import redirect_stderr
import io
import unittest
from unittest.mock import patch

from experimentos import COMPARACOES, resumir_pares
from main import main


def linha(cenario, semente, demanda, concluidos, fila, atraso=0):
    return {"cenario": cenario, "semente": semente, "demanda": demanda,
            "concluidos": concluidos, "fila_media_total": fila,
            "atraso_maximo_fase_s": atraso}


class ResumoTest(unittest.TestCase):
    def test_compara_cenarios_dentro_da_mesma_semente_e_demanda(self):
        resultados = [
            linha("B", 2, 20, 19, 6), linha("A", 1, 20, 10, 4),
            linha("D", 1, 20, 14, 1), linha("C", 2, 20, 23, 5),
            linha("A", 2, 20, 20, 8), linha("C", 1, 20, 11, 2),
            linha("B", 1, 20, 12, 3), linha("D", 2, 20, 22, 3),
            linha("A", 1, 40, 5, 8), linha("B", 1, 40, 9, 4),
        ]
        resumo = list(resumir_pares(resultados))
        self.assertEqual(len(resumo), 5)
        pares_20 = {r["comparacao"]: r for r in resumo if r["demanda"] == 20}
        self.assertEqual(set(pares_20), {"B - A", "C - A", "D - B", "D - C"})
        self.assertEqual(pares_20["B - A"]["sementes"], 2)
        self.assertEqual(pares_20["B - A"]["delta_concluidos"], 0.5)
        self.assertEqual(pares_20["B - A"]["min_concluidos"], -1)
        self.assertEqual(pares_20["B - A"]["max_concluidos"], 2)
        self.assertEqual(pares_20["B - A"]["delta_fila_media"], -1.5)
        self.assertEqual(resumo[-1]["comparacao"], "B - A")
        self.assertEqual(resumo[-1]["demanda"], 40)
        self.assertEqual(resumo[-1]["sementes"], 1)

    def test_politica_por_prazo_e_comparada_em_ambas_as_malhas(self):
        linhas = [
            linha("A", 7, 45, 10, 8, 20), linha("B", 7, 45, 12, 7, 18),
            linha("C", 7, 45, 14, 6, 15), linha("D", 7, 45, 16, 5, 14),
            linha("E", 7, 45, 13, 6, 10), linha("F", 7, 45, 17, 4, 9),
        ]
        pares = {r["comparacao"]: r for r in resumir_pares(linhas)}
        self.assertEqual(pares["E - B"]["delta_concluidos"], 1)
        self.assertEqual(pares["F - D"]["delta_concluidos"], 1)
        self.assertEqual(pares["F - E"]["delta_fila_media"], -2)
        self.assertEqual(pares["E - B"]["delta_atraso_maximo_fase_s"], -8)
        self.assertEqual(len(pares), len(COMPARACOES))

    def test_recusa_resultados_duplicados(self):
        duplicada = linha("A", 42, 45, 10, 2)
        with self.assertRaises(ValueError):
            list(resumir_pares([duplicada, duplicada]))


class ArgumentosTest(unittest.TestCase):
    def test_recusa_multiplos_valores_sem_comparacao(self):
        for argumentos in (("--sementes", "1", "2"),
                           ("--demandas", "20", "45"),
                           ("--saida", "teste.csv"),
                           ("--comparar", "--sementes", "1", "1"),
                           ("--comparar", "--demandas", "20", "20")):
            with self.subTest(argumentos=argumentos):
                with patch("sys.argv", ["main.py", *argumentos]), redirect_stderr(io.StringIO()):
                    with self.assertRaises(SystemExit) as encerramento:
                        main()
                self.assertEqual(encerramento.exception.code, 2)


if __name__ == "__main__":
    unittest.main()
