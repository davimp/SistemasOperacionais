#include "fcfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
const int NUM_CPU = 4;

void fcfs(FILE* arq_trace, FILE* arq_saida)
{

    char nome[50];
    int t0, dt, deadline;
    int cpu_livres = NUM_CPU;
    Cpu cpus[NUM_CPU];
    int i, j, k;

    while(fscanf("%s %d %d %d", nome, t0, dt, deadline))
    {
    	j = cpus[0].tempo_livre;
    	k = 0;
    	
        for(i = 1; i < NUM_CPU; i++) //pega min(cpu.tempo_livre)
        {
        	
        }
    }
}
