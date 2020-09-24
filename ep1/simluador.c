#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"fcfs.h"
#include"srtn.h"
#include"rr.h"

/* ./ep1 escalonador arquivo_trace arquivo_saida (parametro opcional d) */

int main(int agrc, char* argv[])
{
    FILE *entrada, *saida;
    entrada = fopen(argv[2], "r");
    saida = fopen(argv[3], "w");

    if(!strcmp(argv[1],"1")){
        /*1. First-Come First-Served */
    }
    else if(!strcmp(argv[1],"2")){
        /*2. Shortest Remaining Time Next*/
    }
    else if(!strcmp(argv[1],"3")){
        /*3. Round-Robin*/
    }

}