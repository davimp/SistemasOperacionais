#include "fcfs.h"

/* Função para a thread */
void * Thread(void * a) {
   long int count;
   time_t tempo_inicial, tempo_atual;
   int * arg = a;
   tempo_inicial = time(NULL);
   tempo_atual = time(NULL);
   count = -10000000;
   while((tempo_atual - tempo_inicial) < (*arg)){
      count++;
      tempo_atual = time(NULL);
   }
   return NULL;
}

void fcfs(FILE* arq_trace, FILE* arq_saida)
{
    char nome[50]; /* nome do 'processo'*/
    int t0, dt, deadline; /* dados do 'processo'*/
    int tempo1, tempo2;
    int muda=0;
    pthread_t tid;
    time_t tempo_inicial, tempo_atual, tempo_iniciou, tempo_acabou;
    int ok;//se liga no terminal, passou pelo debug 3
    tempo_inicial = time(NULL); // começa a contar o tempo 

    while(fscanf(arq_trace, "%s %d %d %d", nome, &t0, &dt, &deadline) != EOF){ // lê um processo
        ok = 0;
        while(!ok){ //enquanto não consegue colocar ele numa cpu
            tempo_atual = time(NULL);
            printf("tempo: %ld\n", (tempo_atual - tempo_inicial));
            if((tempo_atual - tempo_inicial) >= t0){ //se tiver chegado no t0
                //checa se tem cpu livre (caso com multiplas cpus)
                //no caso de uma cpu só fica mais fácil porque só precisa chamar o escalonador
                //de novo depois que a thread acabar
                tempo_iniciou = time(NULL);
                if(pthread_create(&tid, NULL, Thread, (void*)&dt)){
                    printf("ERRO ao criar a Thread\n");
                    exit(1);
                }
                ok = 1;
            }

            if(!ok){
                // se não conseguiu dorme 1 segundo
                // se conseguiu não dorme porque o proximo processo pode ter o mesmo t0 (caso com multiplas cpus)
                sleep(1);
            }
            else{ 
                if(pthread_join(tid, NULL)){
                    printf("ERRO ao dar join na Thread\n");
                    exit(1);
                }
                tempo_acabou = time(NULL);
                tempo1 = (tempo_iniciou - tempo_inicial);
                tempo2 = (tempo_acabou - tempo_inicial);
                fprintf(arq_saida, "%s %d %d\n", nome, tempo2, tempo2-tempo1);
            }
        }
        muda++;
    }
    printf("%d\n", muda-1);
}
    /*fscanf(arq_trace, "%s %d %d %d", nome, t0, dt, deadline);*/
    /*
    const int NUM_CPU = 1;(antes da main)
    int mudancas; // quantidade total de mudanças de contexto
    int instante_livre_cpu[NUM_CPU]; // instante no qual cada CPU vai ficar livre
    int menor_instante_livre_cpu = 0; // menor instante no qual uma CPU vai ficar livre
    int indice_menor_instante = 0; // Índice da CPU que vai ficar dísponivel primeiro
    int i, j, k;
    */
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

