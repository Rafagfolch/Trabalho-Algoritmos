# Trabalho - Algoritmos e Estrutura de Dados II

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
