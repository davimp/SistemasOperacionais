#include <stdio.h>
#include <stdlib.h>

#define CONST_T0 5       //t0 máximo = CONST_T0 * número de processos //t0 = (rand() % t0 máximo)
#define MAX_DT 15         //dt máximo
#define CONST_DEADLINE 8  //deadline = t0 + dt + 1 + (rand() % MAX_DT) * (número de processos - (rand() % número de processos)


//uso: ./gera_testes arg1 arg2
//arg1: número de processos
//arg2: nome do arquivo de saída
//arg3: semente

int main(int argc, char *argv[])
{
    int t0, dt, deadline;
    int num_processos; //número de processos
    int i, j;
    int t0_max;
    int semente;
    FILE *f;

    if(argc < 2)
        exit(1);

    num_processos = atoi(argv[1]);
    semente = atoi(argv[3]);
    f = fopen(argv[2], "w");

    srand(semente);
    t0_max = CONST_T0 * num_processos;

    t0 = 0;
    for(i = 0; i < num_processos; i++)
    {
        t0 += rand() % CONST_T0;
        dt = rand() % MAX_DT + 1;
        deadline = t0 + dt + 1 + (rand() % MAX_DT) * (num_processos - (rand() % num_processos)) / (1 + rand() % CONST_DEADLINE);
        fprintf(f, "p%d %d %d %d\n", i, t0, dt, deadline);
    }


    
    return 0;
}