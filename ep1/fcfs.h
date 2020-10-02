#ifndef fcfs_h
#define fcfs_h
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

void fcfs(FILE* arq_trace, FILE* arq_saida, int d);

typedef struct processo_fcfs{
    int id;
    char nome[40];
    time_t t0;
    time_t dt;
    time_t deadline;
} Processo_fcfs;


#endif
