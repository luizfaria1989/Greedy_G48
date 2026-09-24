# Scheduling to Minimize Lateness

Esta pasta contém a implementação pura do algoritmo **Earliest Due Date (EDD)**,
ou *primeiro o prazo mais próximo*. Cada tarefa possui duração de processamento
`p_j` e prazo de conclusão `d_j`. O algoritmo ordena as tarefas por `d_j`
crescente, calcula o término `C_j` e o atraso `L_j = C_j - d_j`. A medida
minimizada é `L_max = max(L_j)`; valores negativos indicam conclusão antecipada.

```python
from scheduling_minimize_lateness import Tarefa, agendar

tarefas = [
    Tarefa("A", duracao=4, prazo=10),
    Tarefa("B", duracao=2, prazo=5),
    Tarefa("C", duracao=3, prazo=8),
]
resultado = agendar(tarefas)
print([e.tarefa.identificador for e in resultado.execucoes])  # B, C, A
print(resultado.atraso_maximo)  # -1
```

Uma demonstração com a tabela de início, conclusão e atraso de cada tarefa
está disponível com `py -3.13 -m scheduling_minimize_lateness`. Nesse exemplo,
o atraso máximo cai de 3 na ordem de entrada para 1 na ordem EDD.

**Hipóteses do resultado clássico:** todas as tarefas estão disponíveis no
início; há uma única máquina; só uma tarefa é processada por vez; uma tarefa
iniciada não é interrompida; as durações são positivas e conhecidas. Nessas
condições, a ordem EDD minimiza o maior atraso. A ordenação preserva a ordem
original quando os prazos são iguais. Sua complexidade é `O(n log n)` em
tempo e `O(n)` em espaço.

Na cidade, uma fase de semáforo funciona como uma tarefa, e o prazo é calculado
a partir da chegada do primeiro veículo à fila. A política reaplica a ordem
EDD a cada decisão porque as filas mudam. Isso torna o algoritmo uma heurística
local na simulação: chegadas futuras, bloqueios de ruas e cruzamentos simultâneos
não satisfazem as hipóteses da prova clássica, então não há garantia de tempo
de viagem mínimo para toda a cidade.
