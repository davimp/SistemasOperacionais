#include "fcfs.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

const int NUM_CPU = 1;

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
    //int tempo_atual = 0; // instante de tempo atual do simulador do clock
    
    mudancas=i=j=k=0;
    printf("DEBUG\n");
    double tempo1, tempo2, tempo3;

    pthread_t tid;
    clock_t tempo_inicial, tempo_atual, tempo_iniciou, tempo_acabou;
    int ok;//se liga no terminal, passou pelo debug 3
    tempo_inicial = clock(); // começa a contar o tempo 
    printf("DEBUG 2\n");
    int t = 0;

    while(fscanf(arq_trace, "%s %d %d %d", nome, &t0, &dt, &deadline) != EOF){ // lê um processo
        ok = 0;
        while(!ok){ //enquanto não consegue colocar ele numa cpu
            puts("segundo while\n");
            tempo_atual = clock();
            tempo3 = (double)((tempo_atual-tempo_inicial)/CLOCKS_PER_SEC);
            printf("tempo: %f\n", tempo3);
            if(tempo3 >= (double)t0){ //se tiver chegado no t0
                //checa se tem cpu livre 
                //no caso de uma cpu só fica mais fácil porque só precisa chamar o escalonador
                //de novo depois que a thread acabar
                tempo_iniciou = clock();
                if(pthread_create(&tid, NULL, Thread, (void*)&dt)){
                    printf("ERRO ao criar a Thread\n");
                    exit(1);
                }
                ok = 1;
                puts("OK");
            }

            if(!ok){
                puts("IF DO SLEEP ANTES\n");
                // se não conseguiu dorme 1 segundo
                // se conseguiu não dorme porque o proximo processo pode ter o mesmo t0 (caso com multiplas cpus)
                /* sleep(1); */
                tempo_iniciou = clock();
                tempo_acabou = clock();
                tempo1 = 0.;
                while(tempo1 < 1.){
                    tempo_acabou = clock();
                    tempo1 = (double)((tempo_acabou - tempo_iniciou)/CLOCKS_PER_SEC);
                }
                /*t++;*/
                puts("IF DO SLEEP DEPOIS\n");
            }
            else{ 
                puts("Chegou aqui");
                if(pthread_join(tid, NULL)){
                    printf("ERRO ao dar join na Thread\n");
                    exit(1);
                }
                tempo_acabou = clock();
                tempo1 = (double)((tempo_iniciou - tempo_inicial)/CLOCKS_PER_SEC);
                tempo2 = (double)((tempo_acabou - tempo_inicial)/CLOCKS_PER_SEC);
                /*fprintf(arq_saida, "%s %d %d\n", nome, t, dt-t);*/
                fprintf(arq_saida, "%s %f %f\n", nome, tempo2, tempo2-tempo1);
            }
        }
    }
    

    /*fscanf(arq_trace, "%s %d %d %d", nome, t0, dt, deadline);*/
    /*while(!fim_da_linha || tempo_atual <= menor_instante_livre_cpu) // cada ciclo desse while representa um ciclo do clock
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
*/

}
