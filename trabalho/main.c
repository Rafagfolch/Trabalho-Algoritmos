#include <stdio.h>
#include "benchmarking.h"

int main() {
    printf("Inicializando auditoria de algoritmos...\n\n");

    iniciar_testes_principais();
    iniciar_testes_bozo();

    printf("\nColeta de metricas finalizada.\n");
    return 0;
}