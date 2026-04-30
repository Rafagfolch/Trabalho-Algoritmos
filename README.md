# Trabalho - Algoritmos e Estrutura de Dados II

## Complexidade dos algoritmos

| **Algoritmo** | **Melhor caso** | **Caso médio** | **Pior caso** | **Estabilidade**
|---|---|---|---|
| **bolhainteligente** | O(n) | O(n²) | O(n²) | Estável
| **selecao** | O(n²) | O(n²) | O(n²) | Instável
| **insercao** | O(n) | O(n²) | O(n²) | Estável
| **mergesort** | O(n log n) | O(n log n) | O(n log n) | Estável
| **quicksort** | O(n log n) | O(n log n) | O(n²) | Instável
| **shellsort** | O(n log n) | O(n log n) | O(n²) | Instável
| **heapsort** | O(n log n) | O(n log n) | O(n log n) | Instável
| **bozosort** | O(n) | O(n × n!) | O(∞) | Instável

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
