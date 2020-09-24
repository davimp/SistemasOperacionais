#include "fcfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
const int NUM_CPU = 4;

void fcfs(FILE* arq_trace, FILE* arq_saida)
{
    //Ah não. Eu quis dizer cada aviao eh uma thread
    //ja volto
    //vou comer
    // eu acho que fico mais livre daqui a pouco tbm kk
    // to sendo constantemente interrompido aqui

    char nome[50];
    int t0, dt, deadline;
    int cpu_livres = NUM_CPU;

    while(fscanf("%s %d %d %d", nome, t0, dt, deadline))
    {
        
    }
}
