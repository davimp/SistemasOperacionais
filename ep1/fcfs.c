#include "fcfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

const int NUM_CPU = 4;

//função para a thread
void * Thread(void * a) {
   long int count;
   clock_t tempo_inicial, tempo_atual;
   int * arg = a;
   tempo_inicial = clock();
   tempo_atual = clock();
   count = -10000000;
   while((tempo_atual - tempo_inicial)/CLOCKS_PER_SEC < (*arg)){
      count++;
      tempo_atual = clock();
   }
   free(arg);
   return NULL;
}

void fcfs(FILE* arq_trace, FILE* arq_saida)
{
    char nome[50]; // nome do 'processo'
    int t0, dt, deadline; // dados do 'processo'
    int tf, tr; // 
    int mudancas; // quantidade total de mudanças de contexto
    int instante_livre_cpu[NUM_CPU]; // instante no qual cada CPU vai ficar livre
    int menor_instante_livre_cpu = 0; // menor instante no qual uma CPU vai ficar livre
    int indice_menor_instante = 0; // Índice da CPU que vai ficar dísponivel primeiro
    int i, j, k;
    int fim_da_linha;
    int tempo_atual = 0; // instante de tempo atual do simulador do clock
    
    mudancas=i=j=k=0;


    /*
    clock_t tempo_inicial, tempo_atual;
    int ok;
    tempo_inicial = clock(); // começa a contar o tempo
    while(fscanf(arq_trace, "%s %d %d %d", nome, t0, dt, deadline)){ // lê um processo
        ok = 0;
        while(!ok){ //enquanto não consegue colocar ele numa cpu
            tempo_atual = clock();
            if((tempo_atual-tempo_inicial)/CLOCKS_PER_SEC > t0){ //se tiver chegado no t0
                //checa se tem cpu livre  
                for(i = 1; i < NUM_CPU; i++)
                {
                    if(cpus[i].livre)//checa se a cpu 'i' ta livre, considerando que teria uma variavel pra dizer se esta livre ou não
                    {
                        //coloca na cpu
                        cpus[i].livre = 0;//false
                        ok = 1;
                        break;
                    }
                }
            }
            if(!ok){ 
                // se não conseguiu dorme 1 segundo
                // se conseguiu não dorme porque o proximo processo pode ter o mesmo t0 
                usleep(1000);
            }
        }
    }
    */

    /*fscanf(arq_trace, "%s %d %d %d", nome, t0, dt, deadline);*/
    while(!fim_da_linha || tempo_atual <= menor_instante_livre_cpu) // cada ciclo desse while representa um ciclo do clock
    {
        while(menor_instante_livre_cpu <= tempo_atual) // Só executa quando alguma CPU estiver livre
        {

            // Crio um processo e mudo o valor do instante_livre da próxima CPU a ficar livre
            
            //...

            // Recalculo o menor_instante_livre_cpu
            menor_instante_livre_cpu = instante_livre_cpu[0];
            indice_menor_instante = 0;

            for(i = 1; i < NUM_CPU; i++)
            {
                if(instante_livre_cpu[i] < menor_instante_livre_cpu)
                {
                    menor_instante_livre_cpu = instante_livre_cpu[i];
                    indice_menor_instante = i;
                }
            }


            if(fscanf(arq_trace, "%s %d %d %d", nome, t0, dt, deadline) == EOF) //lê outro 'processo'
                fim_da_linha = 1;
        }

    
        usleep(1000);
        tempo_atual++;
    }


    return 0;
}
