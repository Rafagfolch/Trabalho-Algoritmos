#include <stdio.h>
#include "benchmarking.h"

int main() {
    printf("Inicializando testes\n\n");

    iniciar_testes_principais();
    iniciar_testes_bozo();

    printf("\nColeta de métricas finalizada\n");
    return 0;
}