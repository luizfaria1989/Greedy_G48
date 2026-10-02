"""Execute sem argumentos para abrir a interface, ou use --comparar."""

import argparse
import os

os.environ.setdefault("PYGAME_HIDE_SUPPORT_PROMPT", "1")


def positivo(valor):
    import math
    numero = float(valor)
    if not math.isfinite(numero) or numero <= 0:
        raise argparse.ArgumentTypeError("Informe um número positivo e finito.")
    return numero


def main():
    parser = argparse.ArgumentParser(description="Simulador de cidade — algoritmos ambiciosos")
    parser.add_argument("--comparar", action="store_true", help="Comparar A/B/C/D/E/F sem abrir janela")
    parser.add_argument("--sementes", nargs="+", type=int, default=[42],
                        help="Uma semente na interface; várias com --comparar (padrão: 42)")
    parser.add_argument("--demandas", nargs="+", type=positivo, default=[45.0],
                        help="Veículos por minuto; várias taxas com --comparar (padrão: 45)")
    parser.add_argument("--duracao", type=positivo, default=300.0, help="Segundos simulados")
    parser.add_argument("--saida", help="CSV de --comparar (padrão: resultados/comparacao.csv)")
    args = parser.parse_args()
    if args.comparar:
        if len(set(args.sementes)) != len(args.sementes):
            parser.error("Cada semente deve aparecer apenas uma vez em --sementes.")
        if len(set(args.demandas)) != len(args.demandas):
            parser.error("Cada demanda deve aparecer apenas uma vez em --demandas.")
        from experimentos import comparar, exportar, resumir_pares
        resultados = list(comparar(args.sementes, args.demandas, args.duracao))
        print("Cen | Semente | Dem/min | Gerados | Concl. | Na cidade | Externos | Viagem(s)* | Fila média | Atraso máx fase(s)")
        for r in resultados:
            viagem = "—" if r["viagem_media_s"] is None else f'{r["viagem_media_s"]:.1f}'
            print(f'{r["cenario"]:^3} | {r["semente"]:^7} | {r["demanda"]:^7g} | '
                  f'{r["gerados"]:^7} | {r["concluidos"]:^6} | {r["em_circulacao"]:^9} | '
                  f'{r["aguardando_entrada"]:^8} | {viagem:^10} | {r["fila_media_total"]:.1f} | '
                  f'{r["atraso_maximo_fase_s"]:.1f}')
        print("* Viagem média considera somente veículos que chegaram ao destino.")
        print("\nDiferenças pareadas por demanda: média [mínimo; máximo] entre sementes")
        for resumo in resumir_pares(resultados):
            print(f'{resumo["demanda"]:g}/min | {resumo["comparacao"]} | '
                  f'n={resumo["sementes"]} | '
                  f'concluídos {resumo["delta_concluidos"]:+.1f} '
                  f'[{resumo["min_concluidos"]:+d}; {resumo["max_concluidos"]:+d}] | '
                  f'fila média {resumo["delta_fila_media"]:+.1f} '
                  f'[{resumo["min_fila_media"]:+.1f}; {resumo["max_fila_media"]:+.1f}] | '
                  f'atraso máx fase {resumo["delta_atraso_maximo_fase_s"]:+.1f}s '
                  f'[{resumo["min_atraso_maximo_fase_s"]:+.1f}; '
                  f'{resumo["max_atraso_maximo_fase_s"]:+.1f}]')
        print(f"CSV salvo em: {exportar(resultados, args.saida or 'resultados/comparacao.csv')}")
    else:
        if len(args.sementes) != 1 or len(args.demandas) != 1:
            parser.error("Use --comparar para informar várias sementes ou demandas.")
        if args.saida is not None:
            parser.error("--saida só pode ser usado com --comparar; na interface, use Exportar CSV.")
        from render.interface import Aplicacao
        Aplicacao(args.sementes[0], args.demandas[0], args.duracao).executar()


if __name__ == "__main__":
    main()
