"""Integrais no tempo, sem depender da taxa de quadros da interface."""

from dataclasses import dataclass


@dataclass
class Metricas:
    instante: float = 0.0
    area_filas: float = 0.0
    max_fila: int = 0
    atraso_maximo_fase_s: float = 0.0
    fases_atrasadas: int = 0
    fases_atendidas: int = 0

    def integrar(self, agora: float, total_fila: int):
        self.area_filas += (agora - self.instante) * total_fila
        self.instante = agora

    def observar(self, tamanhos):
        self.max_fila = max(self.max_fila, max(tamanhos, default=0))

    def registrar_fase(self, prazo: float, conclusao: float):
        """Registra o término de um verde reservado a uma fila.

        O prazo é o da cabeça da fila quando ela entrou neste atendimento.
        A métrica considera o fim do verde, não o instante de saída do carro.
        """
        atraso = max(0.0, conclusao - prazo)
        self.fases_atendidas += 1
        if atraso > 1e-9:
            self.fases_atrasadas += 1
        self.atraso_maximo_fase_s = max(self.atraso_maximo_fase_s, atraso)
