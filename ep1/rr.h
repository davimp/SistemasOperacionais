#ifndef rr_h
#define rr_h
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

void rr(FILE* arq_trace, FILE* arq_saida, int d);

typedef struct processo_rr{
    int id;
    char nome[40];
    time_t t0;
    time_t dt;
    time_t deadline;

    struct processo_rr *prox;
} Processo_rr;

#endif