#ifndef BENCHMARKING_H
#define BENCHMARKING_H

#include "ordenacao.h"

// Orquestra a bateria principal de testes para O(n^2) e O(n log n)
void iniciar_testes_principais();

// Orquestra a bateria experimental do BozoSort com limite de 30s
void iniciar_testes_bozo();

#endif // BENCHMARKING_H