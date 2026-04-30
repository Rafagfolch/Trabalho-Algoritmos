#include <stdio.h>
#include <stdlib.h>
#include "ordenacao.h"

met *selecaoCorrigido(r *vet, int tam) {
    met *m = alocaMetricas();
    int i, j, menor;
    r aux;
    for (i = 0; i < tam - 1; i++) {
        menor = i;
        for (j = i + 1; j < tam; j++) {
            m->comparacoes++;
            if (vet[j].user_id < vet[menor].user_id) menor = j;
        }
        if (menor != i) {
            m->movimentacoes += 3;
            aux = vet[i];
            vet[i] = vet[menor];
            vet[menor] = aux;
        }
    }
    return m;
}
void mergeCorrigido(r *vet, int inicio, int meio, int fim, met *m) {
    int i, j, k;
    int tamV1 = meio - inicio + 1;
    int tamV2 = fim - meio;
    r *v1 = (r *)malloc(tamV1 * sizeof(r));
    r *v2 = (r *)malloc(tamV2 * sizeof(r));

    for (i = 0; i < tamV1; i++) { v1[i] = vet[inicio + i]; m->movimentacoes++; }
    for (j = 0; j < tamV2; j++) { v2[j] = vet[meio + 1 + j]; m->movimentacoes++; }

    i = 0; j = 0; k = inicio;
    while (i < tamV1 && j < tamV2) {
        m->comparacoes++;
        if (v1[i].user_id <= v2[j].user_id) vet[k++] = v1[i++];
        else vet[k++] = v2[j++];
        m->movimentacoes++;
    }
    while (i < tamV1) { vet[k++] = v1[i++]; m->movimentacoes++; }
    while (j < tamV2) { vet[k++] = v2[j++]; m->movimentacoes++; }
    free(v1); free(v2);
}

void mergeSortCorrigido(r *vet, int inicio, int fim, met *m) {
    if (inicio < fim) {
        int meio = inicio + (fim - inicio) / 2;
        mergeSortCorrigido(vet, inicio, meio, m);
        mergeSortCorrigido(vet, meio + 1, fim, m);
        mergeCorrigido(vet, inicio, meio, fim, m);
    }
}