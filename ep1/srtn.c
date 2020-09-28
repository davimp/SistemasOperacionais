#include"srtn.h"


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

void srtn(FILE* arq_trace, FILE* arq_saida)
{
    char nome[50]; /* nome do 'processo'*/
    int t0, dt, deadline; /* dados do 'processo'*/
    int tempo1, tempo2;
    pthread_t tid;
    time_t tempo_inicial, tempo_atual, tempo_iniciou, tempo_acabou;
    int ok; //se liga no terminal, passou pelo debug 3
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
                /*
                if(pthread_join(tid, NULL)){
                    printf("ERRO ao dar join na Thread\n");
                    exit(1);
                }*/
                tempo_acabou = time(NULL);
                tempo1 = (tempo_iniciou - tempo_inicial);
                tempo2 = (tempo_acabou - tempo_inicial);
                fprintf(arq_saida, "%s %d %d\n", nome, tempo2, tempo2-tempo1);
            }
        }
    }
    printf("\n");
}
