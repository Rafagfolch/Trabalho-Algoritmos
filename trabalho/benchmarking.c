#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <windows.h> // Substitui o <signal.h> para lidar com o timeout no MinGW
#include "benchmarking.h"

// Retorna o tempo decorrido em milissegundos
double calcula_tempo_ms(struct timeval inicio, struct timeval fim) {
    double segundos_passados = (double)(fim.tv_sec - inicio.tv_sec);
    double microssegundos_passados = (double)(fim.tv_usec - inicio.tv_usec);
    double seg_em_ms = segundos_passados * 1000;
    double mic_em_ms = microssegundos_passados / 1000;

    return seg_em_ms + mic_em_ms;
}

// O sistema deve checar se, em caso de notas iguais, a ordem original dos IDs foi preservada
int verifica_estabilidade(r *vet, int tam) {
    for (int i = 0; i < tam - 1; i++) {
        if (vet[i].user_id == vet[i+1].user_id) { //A verificação só começa se encontrarmos dois elementos com o mesmo id
            if (vet[i].chegada > vet[i+1].chegada) return 0; // Se o elemento da esquerda[i] tem um tempo de chegada maior(ou seja, chegou mais tarde) do que o elemento da direita[i+1], significa que o algoritmo os inverteu
        }
    }
    return 1; // Estável
}

void executar_teste(int id_algo, const char* nome_algo, int tam, int cenario) {
    double tempo_total = 0.0;
    unsigned long long comps_totais = 0;
    unsigned long long movs_totais = 0;
    int estavel = 1;
    int repeticoes;
    const char* nome_cenario = "";
    r *vetor;
    unsigned int seed;
    struct timeval inicio, fim;

    // Aleatório (0) e Quase Ordenado (2) exigem 30 repetições
    if (cenario == 0 || cenario == 2) {
        repeticoes = 30;
    }
    else {
        repeticoes = 1; // Para os outros casos (Crescente ou Decrescente) que são determinísticos
    }

    if (cenario == 0) {
        nome_cenario = "Aleatorio"; //Dados sem ordem aparente
    }
    else if (cenario == 1) {
        nome_cenario = "Crescente"; //Ordenado
    }
    else if (cenario == 2) {
        nome_cenario = "Quase_Ordenado"; //Quase Ordenado - 90% do vetor ordenado, 10% fora de ordem
    }
    else {
        nome_cenario = "Decrescente"; //Inversamente ordenado
    }

    for (int i = 0; i < repeticoes; i++) {

        seed = i + 1; //permite que os testes sejam reprodutíveis, visando a estabilidade dos resultados

        if (cenario == 0) {
            vetor = geraAleatorios(tam, seed);
        }
        else if (cenario == 1) {
            vetor = geraOrdenados(tam, seed);
        }
        else if (cenario == 2) {
            vetor = geraQuaseOrdenados(tam, seed, 10); // 10% de desordem
        }
        else {
            vetor = geraDecrescente(tam, seed);
        }

        met *metricas = NULL; // para receber os dados dos algoritmos Bolha, Inserção, Seleção, shellSort, heapSort

        gettimeofday(&inicio, NULL); // inicia o cronômetro

        switch (id_algo) {
            case 1:
                metricas = bolhaInteligente(vetor, tam);
                break;
            case 2:
                metricas = selecao(vetor, tam);
                break;
            case 3:
                metricas = insercao(vetor, tam);
                break;
            case 4:
                metricas = alocaMetricas();
                mergeSort(vetor, 0, tam - 1, metricas);
                break;
            case 5:
                metricas = alocaMetricas();
                quickSort(vetor, 0, tam - 1, metricas);
                break;
            case 6:
                metricas = shellSort(vetor, tam);
                break;
            case 7:
                metricas = heapSort(vetor, tam);
                break;
        }

        gettimeofday(&fim, NULL); // para o cronômetro

        tempo_total += calcula_tempo_ms(inicio, fim); //soma o tempo desta rodada para calcular a média depois
        comps_totais += metricas->comparacoes; // acumula as comparações para obter a média estatítica
        movs_totais += metricas->movimentacoes; // acumula as movimentações totais da N repetições

        if (!verifica_estabilidade(vetor, tam)) estavel = 0; // verifica a estabilidade do algoritmo

        liberaMetricas(metricas);
        liberaVetor(vetor);
    }

    printf("%s;%s;%d;%.4f;%llu;%llu;%s\n",nome_algo, nome_cenario, tam, tempo_total / repeticoes, comps_totais / repeticoes, movs_totais / repeticoes, estavel ? "Sim" : "Nao");
}

void iniciar_testes_principais() {
    int tamanhos[] = {1000, 5000, 10000, 15000;
    int num_tamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]); //calcula a quantidade de elementos no array. Garante que o loop se ajuste se novos tamanhos forem adicionados

    printf("ALGORITMO;CENÁRIO;TAMANHO;TEMPO MÉDIO EM MS;MÉDIA DE COMPARAÇÕES;MÉDIA DE MOVIMENTAÇÕES;ESTABILIDADE\n");

    // Loop externo por Algoritmo (Isolamento de resultados)
    for (int algo = 1; algo <= 7; algo++) {
        const char* nome;
        if(algo == 1) nome = "BolhaInteligente";
        else if(algo == 2) nome = "Selecao";
        else if(algo == 3) nome = "Insercao";
        else if(algo == 4) nome = "MergeSort";
        else if(algo == 5) nome = "QuickSort";
        else if(algo == 6) nome = "ShellSort";
        else nome = "HeapSort";

        // Loop intermediário por Cenário
        for (int cenario = 0; cenario < 4; cenario++) {

            // Loop interno por Tamanho
            for (int t = 0; t < num_tamanhos; t++) {
                executar_teste(algo, nome, tamanhos[t], cenario);
            }
        }
        // Uma linha em branco ajuda a separar visualmente os blocos no CSV
        printf("\n");
    }
}

// BOZOSORT

// Estrutura para passar parâmetros para a Thread do Windows: elas só aceitam um parâmetro
// Garante execução assíncrona, impedindo que a natureza aleatória do BozoSort trave o fluxo do benchmarking
typedef struct {
    r* vetor;
    int tam;
    met* metricas_retorno;
} BozoArgs;

// Função que a Thread vai executar paralelamente
// DWORD WINAPI: Define a assinatura e a convenção de chamada exigida pelo Kernel do Windows
// LPVOID lpParam: ponteiro genérico(void*) contendo o pacote de argumentos
DWORD WINAPI thread_bozosort(LPVOID lpParam) {
    BozoArgs* args = (BozoArgs*)lpParam; //Type Casting: converte o ponteiro genérico de volta para o tipo estruturado
    args->metricas_retorno = bozoSort(args->vetor, args->tam); //executa o algoritmo de forma isolada e captura o retorno das métricas
    return 0;
}

void iniciar_testes_bozo() {
    // Execute o BozoSort para vetores de tamanho 4, 8, 10 e 12 elementos
    int tamanhos[] = {4, 8, 10, 12};
    int num_tamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]); //calcula a quantidade de elementos no array. Garante que o loop se ajuste se novos tamanhos forem adicionados

    printf("\nTESTES BOZOSORT\n");
    printf("ALGORITMO;CENÁRIO;TAMANHO;TEMPO MÉDIO EM MS;MÉDIA DE COMPARAÇÕES;MÉDIA DE MOVIMENTAÇÕES;STATUS\n");

    for (int t = 0; t < num_tamanhos; t++) {
        int tam = tamanhos[t];

        // Para cada tamanho, realizar 5 repetições para tirar a média
        for (int rep = 0; rep < 5; rep++) {
            r *vetor = geraAleatorios(tam, rep + 1); //rep+1: gera número diferentes em cada repetição
            struct timeval inicio, fim;
            BozoArgs args = {vetor, tam, NULL};

            gettimeofday(&inicio, NULL); // inicia o cronômetro

            // Solicita ao Kernel do Windows a criação de um fluxo de execução paralelo (thread)
            // hThread funciona como um "identificador" para monitorar ou manipular o processo do BozoSort
            HANDLE hThread = CreateThread(NULL, 0, thread_bozosort, &args, 0, NULL);

            // Monitora a thread e aguarda sua finalização por no máximo 30.000ms (30 segundos)
            // Caso o tempo esgote antes do término do algoritmo, a função retornará WAIT_TIMEOUT
            DWORD waitResult = WaitForSingleObject(hThread, 30000);

            gettimeofday(&fim, NULL); // fim do cronômetro

            if (waitResult == WAIT_TIMEOUT) { // WAIT_TIMEOUT: código de retorno
                // Se o tempo estourou, força a interrupção
                TerminateThread(hThread, 0);
                printf("Falha no BozoSort Aleatório de tamanho %d: estouro de tempo;\n", tam);
            } else {
                double tempo_ms = calcula_tempo_ms(inicio, fim); //descobre quanto tempo levou entre o início e o fim do cronômetro
                printf("BozoSort;Aleatorio;%d;%.4f;%llu;%llu;OK\n", tam, tempo_ms, args.metricas_retorno->comparacoes, args.metricas_retorno->movimentacoes);
                liberaMetricas(args.metricas_retorno); //limpa a memória das métricas que a thread criou
            }

            CloseHandle(hThread); //fecha a conexão do programa principal com a thread e libera os recursos
            liberaVetor(vetor); //devolve a memória do vetor para o sistema, evitando que o PC fique lento por falta de RAM
        }
    }
}