"""Experimentos pareados: a mesma demanda é reutilizada nos seis cenários."""

import csv
from collections import defaultdict
from pathlib import Path
from statistics import fmean
from simulacao import Simulacao, gerar_demanda

CENARIOS = (("A", "fixo", False), ("B", "guloso", False),
            ("C", "fixo", True), ("D", "guloso", True),
            ("E", "prazo", False), ("F", "prazo", True))
COMPARACOES = (("B", "A"), ("E", "A"), ("E", "B"),
               ("C", "A"), ("D", "B"), ("F", "E"),
               ("D", "C"), ("F", "C"), ("F", "D"))


def comparar(sementes=(42,), demandas=(45.0,), duracao=300.0):
    for demanda in demandas:
        for semente in sementes:
            pedidos = gerar_demanda(semente, demanda, duracao)
            for nome, politica, nova in CENARIOS:
                simulacao = Simulacao(politica, nova, semente, demanda, duracao, pedidos)
                yield {"cenario": nome, **simulacao.executar()}


def resumir_pares(resultados):
    """Calcula diferenças dentro de cada semente, agrupadas por demanda.

    Valores positivos em `delta_concluidos` e negativos em `delta_fila_media`
    ou `delta_atraso_maximo_fase_s` indicam melhora na respectiva métrica.
    Cada comparação usa apenas pares completos da mesma semente e demanda.
    """
    grupos = defaultdict(dict)
    for resultado in resultados:
        chave = (resultado["demanda"], resultado["semente"])
        cenario = resultado["cenario"]
        if cenario in grupos[chave]:
            raise ValueError(f"Cenário {cenario} repetido para demanda e semente {chave}.")
        grupos[chave][cenario] = resultado

    for demanda in sorted({chave[0] for chave in grupos}):
        repeticoes = [grupo for (taxa, _), grupo in grupos.items() if taxa == demanda]
        for atual, referencia in COMPARACOES:
            pares = [(grupo[atual], grupo[referencia]) for grupo in repeticoes
                     if atual in grupo and referencia in grupo]
            if not pares:
                continue
            concluidos = [novo["concluidos"] - base["concluidos"] for novo, base in pares]
            filas = [novo["fila_media_total"] - base["fila_media_total"] for novo, base in pares]
            atrasos = [novo["atraso_maximo_fase_s"] - base["atraso_maximo_fase_s"]
                       for novo, base in pares]
            yield {
                "demanda": demanda,
                "comparacao": f"{atual} - {referencia}",
                "sementes": len(pares),
                "delta_concluidos": fmean(concluidos),
                "min_concluidos": min(concluidos),
                "max_concluidos": max(concluidos),
                "delta_fila_media": fmean(filas),
                "min_fila_media": min(filas),
                "max_fila_media": max(filas),
                "delta_atraso_maximo_fase_s": fmean(atrasos),
                "min_atraso_maximo_fase_s": min(atrasos),
                "max_atraso_maximo_fase_s": max(atrasos),
            }


def exportar(resultados, caminho):
    resultados = list(resultados)
    if not resultados:
        raise ValueError("Não há resultados para exportar.")
    caminho = Path(caminho)
    caminho.parent.mkdir(parents=True, exist_ok=True)
    with caminho.open("w", newline="", encoding="utf-8-sig") as arquivo:
        escritor = csv.DictWriter(arquivo, fieldnames=list(resultados[0]), delimiter=";")
        escritor.writeheader()
        escritor.writerows(resultados)
    return caminho.resolve()
