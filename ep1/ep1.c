#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"fcfs.h"
#include"srtn.h"
#include"rr.h"

/* ./ep1 <escalonador> <arquivo_trace> <arquivo_saida> <parametro opcional d> */

int main(int argc, char* argv[])
{
    FILE *entrada, *saida;
    entrada = fopen(argv[2], "r");
    saida = fopen(argv[3], "w");
    int d;
    d = 0;
    if(argc == 5) d = 1;

    if(!strcmp(argv[1],"1")){
        /*1. First-Come First-Served */
        fcfs(entrada, saida, d);
    }
    else if(!strcmp(argv[1],"2")){
        /*2. Shortest Remaining Time Next*/
        srtn(entrada, saida, d);
    }
    else if(!strcmp(argv[1],"3")){
        /*3. Round-Robin*/
        rr(entrada, saida, d);
    }

    fclose(entrada);
    fclose(saida);

    /* Ordenar os processos para facilitar a geração dos gráficos */
    

    return 0;
}