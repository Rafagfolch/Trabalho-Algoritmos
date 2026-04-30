# Trabalho - Algoritmos e Estrutura de Dados II

## Complexidade dos algoritmos

| **Algoritmo** | **Melhor caso** | **Caso médio** | **Pior caso** |
|---|---|---|---|
| **bolhainteligente** | O(n) | O(n²) | O(n²) |
| **selecao** | O(n²) | O(n²) | O(n²) |
| **insercao** | O(n) | O(n²) | O(n²) |
| **mergesort** | O(n log n) | O(n log n) | O(n log n) |
| **quicksort** | O(n log n) | O(n log n) | O(n²) |
| **shellsort** | O(n log n) | O(n log n) | O(n²) |
| **heapsort** | O(n log n) | O(n log n) | O(n log n) |
| **bozosort** | O(n) | O(n × n!) | O(∞) |

## Arquitetura do benchmarking

```text
.
└── benchmarking/
    ├── main.c              # Orquestrador do sistema
    ├── benchmarking.c      # Implementação das métricas e threads
    ├── benchmarking.h      # Protótipos das funções de teste
    ├── ordenacao.h         # Definições fornecidas 
    └── lib/                # Bibliotecas estáticas de ordenação
```

### Como rodar (via Wine)
Para compilar utilizando o cross-compiler para Windows e gerar o arquivo de resultados:

```bash
# Compilação
x86_64-w64-mingw32-gcc main.c benchmarking.c -L./lib -lordenacao18 -o executavel.exe 

# Execução e geração do CSV
wine ./executavel.exe > trabalho.csv
```
