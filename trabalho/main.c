#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>     // Substitui a biblioteca de tempo do Linux
#include <windows.h>  // Necessário para o timeout do BozoSort e Threads
#include "ordenacao.h"

// ADAPTADORES DE FUNÇÕES PARA O BENCHMARKING
met* testaMergeSort(r *vet, int tam) {
    met *m = alocaMetricas();
    mergeSort(vet, 0, tam - 1, m);
    return m;
}

met* testaQuickSort(r *vet, int tam) {
    met *m = alocaMetricas();
    quickSort(vet, 0, tam - 1, m);
    return m;
}

// Adaptador para o gerador de Quase Ordenados (90% ordenado e 10% fora de ordem) [cite: 80]
r* geradorQuaseOrdenado10(int tam, unsigned int seed) {
    return geraQuaseOrdenados(tam, seed, 10);
}

typedef met* (*Algoritmo)(r*, int);
typedef r* (*Gerador)(int, unsigned int);

//VERIFICACAO DE ESTABILIDADE
// O sistema deve checar se, em caso de notas iguais, a ordem original dos IDs foi preservada[cite: 87].
int verificaEstabilidade(r *vet, int tam) {
    for (int i = 0; i < tam - 1; i++) {
        if (vet[i].user_id == vet[i+1].user_id) {
            if (vet[i].chegada > vet[i+1].chegada) {
                return 0; // FALHOU: Algoritmo Instável
            }
        }
    }
    return 1; // PASSOU: Algoritmo Estável
}

void rodarTeste(const char* nomeAlg, Algoritmo alg, const char* nomeCenario, Gerador gerador, int tam, int reps) {
    // Trava de segurança: Evita travar a máquina com algoritmos O(n^2) em vetores massivos.
    if (tam >= 100000 && (strcmp(nomeAlg, "Bolha Inteligente") == 0 ||
                          strcmp(nomeAlg, "Seleção") == 0 ||
                          strcmp(nomeAlg, "Inserção") == 0)) {
        printf("%s;%s;%d;IGNORADO;-;-;-\n", nomeAlg, nomeCenario, tam);
        return;
    }

    long long tempo_total_ms = 0;
    unsigned long long comp_total = 0, mov_total = 0;
    int estavel = 1;

    for (int rep = 0; rep < reps; rep++) {
        r *vetor = gerador(tam, rep + 1);

        // Medição nativa em C
        clock_t inicio = clock();
        met *m = alg(vetor, tam);
        clock_t fim = clock();

        tempo_total_ms += (long long)(((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000);
        comp_total += m->comparacoes;
        mov_total += m->movimentacoes;

        if (rep == 0 && !verificaEstabilidade(vetor, tam)) {
            estavel = 0;
        }

        liberaMetricas(m);
        liberaVetor(vetor);
    }

    printf("%s;%s;%d;%lld;%llu;%llu;%s\n",
           nomeAlg,
           nomeCenario,
           tam,
           tempo_total_ms / reps,
           comp_total / reps,
           mov_total / reps,
           estavel ? "Estavel" : "INSTAVEL");
}

// Estrutura para passar parâmetros para a Thread do Windows
typedef struct {
    r* vetor;
    int tam;
    met* m_result;
} BozoArgs;

// Função que a Thread vai executar paralelamente
DWORD WINAPI threadBozo(LPVOID arg) {
    BozoArgs* args = (BozoArgs*)arg;
    args->m_result = bozoSort(args->vetor, args->tam);
    return 0;
}

void benchmarkingBozoSort() {
    printf("\n=== RESULTADOS BOZOSORT ===\n");
    printf("Algoritmo;Tamanho;Repeticao;Tempo(ms);Comparacoes;Movimentacoes;Status\n");

    int tamanhos_bozo[] = {4, 8, 10, 12}; // Exigência de tamanhos [cite: 125]
    int num_tamanhos_bozo = 4;

    for (int t = 0; t < num_tamanhos_bozo; t++) {
        int tam = tamanhos_bozo[t];

        for (int rep = 0; rep < 5; rep++) { // Realizar 5 repetições para cada tamanho [cite: 126]
            r *vetor = geraAleatorios(tam, rep + 100);
            BozoArgs args = {vetor, tam, NULL};

            clock_t inicio = clock();

            // Cria a Thread e executa o BozoSort
            HANDLE hThread = CreateThread(NULL, 0, threadBozo, &args, 0, NULL);

            // Pausa o sistema e aguarda a Thread por exatos 30.000 milissegundos
            DWORD waitResult = WaitForSingleObject(hThread, 30000);

            if (waitResult == WAIT_TIMEOUT) {
                // Se o tempo estourou de 30 segundos, força a interrupção
                TerminateThread(hThread, 0);
                printf("BozoSort;%d;%d;>30000;-;-;TIMEOUT\n", tam, rep+1);
            } else {
                // Se a Thread terminou antes do tempo
                clock_t fim = clock();
                long long tempo_ms = (long long)(((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000);

                printf("BozoSort;%d;%d;%lld;%llu;%llu;Concluido\n",
                       tam, rep+1, tempo_ms, args.m_result->comparacoes, args.m_result->movimentacoes);

                liberaMetricas(args.m_result);
            }

            CloseHandle(hThread);
            liberaVetor(vetor);
        }
    }
}

int main() {
    int tamanhos[] = {1000, 10000, 100000, 1000000, 10000000};
    int num_tamanhos = 5;

    const char* nomesAlg[] = {"Bolha Inteligente", "Seleção", "Inserção", "MergeSort", "QuickSort Tradicional", "ShellSort", "HeapSort"};
    Algoritmo algs[] = {bolhaInteligente, selecao, insercao, testaMergeSort, testaQuickSort, shellSort, heapSort};
    int num_algs = 7;

    printf("=== BENCHMARKING ALGORITMOS DE ORDENACAO ===\n");
    printf("Algoritmo;Cenario;Tamanho;Tempo_ms;Comparacoes;Movimentacoes;Estabilidade\n");

    for (int t = 0; t < num_tamanhos; t++) {
        int tam = tamanhos[t];

        for (int a = 0; a < num_algs; a++) {
            // Aleatório e Quase Ordenado exigem média de 30 repetições [cite: 93]
            rodarTeste(nomesAlg[a], algs[a], "Aleatório", geraAleatorios, tam, 30);
            rodarTeste(nomesAlg[a], algs[a], "Quase Ordenado", geradorQuaseOrdenado10, tam, 30);

            // Crescente e Decrescente rodam apenas 1 vez (a geração com a mesma seed dá o mesmo vetor)
            rodarTeste(nomesAlg[a], algs[a], "Crescente", geraOrdenados, tam, 1);
            rodarTeste(nomesAlg[a], algs[a], "Decrescente", geraDecrescente, tam, 1);
        }
    }

    benchmarkingBozoSort();

    return 0;
}