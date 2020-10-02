#ifndef srtn_h
#define srtn_h
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

void srtn(FILE* arq_trace, FILE* arq_saida, int d);

typedef struct processo_srtn{
    int id;
    char nome[40];
    time_t t0;
    time_t dt;
    time_t deadline;
} Processo_srtn;

#endif