#ifndef fcfs_h
#define fcfs_h
#include <stdlib.h>

void fcfs(FILE* arq_trace, FILE* arq_saida);

typedef struct processo
{
	char nome[40];
	int t0;
	int dt;
	int deadline;
	Processo *prox;
} Processo;


#endif
