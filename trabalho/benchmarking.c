#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <windows.h> // Substitui o <signal.h> para lidar com o timeout no MinGW
#include "benchmarking.h"

// Retorna o tempo decorrido em milissegundos
double calcula_tempo_ms(struct timeval inicio, struct timeval fim) {
    return (double)(fim.tv_sec - inicio.tv_sec) * 1000.0 +
           (double)(fim.tv_usec - inicio.tv_usec) / 1000.0;
}

// O sistema deve checar se, em caso de notas iguais, a ordem original dos IDs foi preservada [cite: 91]
int verifica_estabilidade(r *vet, int tam) {
    for (int i = 0; i < tam - 1; i++) {
        if (vet[i].user_id == vet[i+1].user_id) {
            if (vet[i].chegada > vet[i+1].chegada) return 0; // Instável
        }
    }
    return 1; // Estável
}

void executar_teste(int id_algo, const char* nome_algo, int tam, int cenario) {
    double tempo_total = 0.0;
    unsigned long long comps_totais = 0;
    unsigned long long movs_totais = 0;
    int estavel = 1;

    // Aleatório (0) e Quase Ordenado (2) exigem 30 repetições [cite: 97]
    int repeticoes = (cenario == 0 || cenario == 2) ? 30 : 1;
    const char* nome_cenario = "";

    if (cenario == 0) nome_cenario = "Aleatorio";
    else if (cenario == 1) nome_cenario = "Crescente";
    else if (cenario == 2) nome_cenario = "Quase_Ordenado";
    else nome_cenario = "Decrescente";

    for (int i = 0; i < repeticoes; i++) {
        r *vetor;
        unsigned int seed = i + 1;

        if (cenario == 0) vetor = geraAleatorios(tam, seed);
        else if (cenario == 1) vetor = geraOrdenados(tam, seed);
        else if (cenario == 2) vetor = geraQuaseOrdenados(tam, seed, 10);
        else vetor = geraDecrescente(tam, seed);

        met *metricas = NULL;
        struct timeval inicio, fim;

        gettimeofday(&inicio, NULL);

        switch (id_algo) {
            case 1: metricas = bolhaInteligente(vetor, tam); break;
            case 2: metricas = selecao(vetor, tam); break;
            case 3: metricas = insercao(vetor, tam); break;
            case 4:
                metricas = alocaMetricas();
                mergeSort(vetor, 0, tam - 1, metricas);
                break;
            case 5:
                metricas = alocaMetricas();
                quickSort(vetor, 0, tam - 1, metricas);
                break;
            case 6: metricas = shellSort(vetor, tam); break;
            case 7: metricas = heapSort(vetor, tam); break;
        }

        gettimeofday(&fim, NULL);

        tempo_total += calcula_tempo_ms(inicio, fim);
        comps_totais += metricas->comparacoes;
        movs_totais += metricas->movimentacoes;

        if (!verifica_estabilidade(vetor, tam)) estavel = 0;

        liberaMetricas(metricas);
        liberaVetor(vetor);
    }

    printf("%s;%s;%d;%.4f;%llu;%llu;%s\n",
           nome_algo, nome_cenario, tam,
           tempo_total / repeticoes,
           comps_totais / repeticoes,
           movs_totais / repeticoes,
           estavel ? "Sim" : "Nao");
}

void iniciar_testes_principais() {
    int tamanhos[] = {1000, 5000, 10000};
    int num_tamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]);

    printf("ALGORITMO;CENARIO;TAMANHO;TEMPO_MEDIO_MS;COMPARACOES_MEDIAS;MOVIMENTACOES_MEDIAS;ESTAVEL\n");

    for (int t = 0; t < num_tamanhos; t++) {
        int tam = tamanhos[t];
        for (int cenario = 0; cenario < 4; cenario++) {
            executar_teste(1, "BolhaInteligente", tam, cenario);
            executar_teste(2, "Selecao", tam, cenario);
            executar_teste(3, "Insercao", tam, cenario);
            executar_teste(4, "MergeSort", tam, cenario);
            executar_teste(5, "QuickSort", tam, cenario);
            executar_teste(6, "ShellSort", tam, cenario);
            executar_teste(7, "HeapSort", tam, cenario);
        }
    }
}

// --- INTEGRAÇÃO DA SUA LÓGICA DE THREADS PARA O BOZOSORT ---

// Estrutura para passar parâmetros para a Thread do Windows
typedef struct {
    r* vetor;
    int tam;
    met* metricas_retorno;
} BozoArgs;

// Função que a Thread vai executar paralelamente
DWORD WINAPI thread_bozosort(LPVOID lpParam) {
    BozoArgs* args = (BozoArgs*)lpParam;
    args->metricas_retorno = bozoSort(args->vetor, args->tam);
    return 0;
}

void iniciar_testes_bozo() {
    // Execute o BozoSort para vetores de tamanho 4, 8, 10 e 12 elementos [cite: 129]
    int tamanhos[] = {4, 8, 10, 12};
    int num_tamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]);

    printf("\n--- TESTES BOZOSORT ---\n");
    printf("ALGORITMO;CENARIO;TAMANHO;TEMPO_MS;COMPARACOES;MOVIMENTACOES;STATUS\n");

    for (int t = 0; t < num_tamanhos; t++) {
        int tam = tamanhos[t];

        // Para cada tamanho, realizar 5 repetições [cite: 130]
        for (int rep = 0; rep < 5; rep++) {
            r *vetor = geraAleatorios(tam, rep + 1);
            struct timeval inicio, fim;
            BozoArgs args = {vetor, tam, NULL};

            gettimeofday(&inicio, NULL);

            // Cria a Thread e executa o BozoSort
            HANDLE hThread = CreateThread(NULL, 0, thread_bozosort, &args, 0, NULL);

            // Estabeleça um tempo limite de 30 segundos
            DWORD waitResult = WaitForSingleObject(hThread, 30000);

            gettimeofday(&fim, NULL);

            if (waitResult == WAIT_TIMEOUT) {
                // Se o tempo estourou, força a interrupção
                TerminateThread(hThread, 0);
                printf("BozoSort;Aleatorio;%d;TIMEOUT(>30s);-;-;TIMEOUT\n", tam);
            } else {
                double tempo_ms = calcula_tempo_ms(inicio, fim);
                printf("BozoSort;Aleatorio;%d;%.4f;%llu;%llu;OK\n",
                       tam, tempo_ms, args.metricas_retorno->comparacoes, args.metricas_retorno->movimentacoes);
                liberaMetricas(args.metricas_retorno);
            }

            CloseHandle(hThread);
            liberaVetor(vetor);
        }
    }
}