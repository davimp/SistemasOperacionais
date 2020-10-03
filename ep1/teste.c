#include"srtn.h"

#define MAXN 10000
Processo_srtn prontos[MAXN], processos[MAXN]; 
pthread_t tid[MAXN];
pthread_mutex_t lock[MAXN];
pthread_mutex_t mutex;
pthread_cond_t cond[MAXN];
int play[MAXN];
int livre;
int liberou;
int c_liberou;
int id_liberou;
int flag;

/* Função para as threads */
void * Thread_SRTN(void * a)
{
   time_t tempo_inicial, tempo_atual;
   struct timeval t0, t1, t2, t3;
   int * arg = a;
   int id, espera, parou;
   long long tempo;
   long long dt;
   id = (*arg);
   free(arg);
   dt = (long long)processos[id].dt;
   dt = dt*1000000; /* microsegundos */
   tempo = 0;
   gettimeofday(&t0, NULL);
   if(flag) fprintf(stderr, "Começou a executar na CPU%d: %s\n", sched_getcpu(), processos[id].nome);
   while(tempo < dt){
      parou = 0;
      pthread_mutex_lock(&lock[id]);
      while(!play[id]){
        gettimeofday(&t2, NULL);
        if(flag) fprintf(stderr, "Pausou a execução na CPU%d: %s\n", sched_getcpu(), processos[id].nome);
        pthread_cond_wait(&cond[id], &lock[id]);
        if(flag) fprintf(stderr, "Voltou a execução na CPU%d: %s\n", sched_getcpu(), processos[id].nome);
        gettimeofday(&t3, NULL);

        parou = 1;
      }
      pthread_mutex_unlock(&lock[id]);

      gettimeofday(&t1, NULL);
      
      if(parou){
        espera = (t3.tv_sec-t2.tv_sec)*1000000 + t3.tv_usec-t2.tv_usec;
        dt += espera;
      }
      tempo = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
   }

   liberou = 1;
   c_liberou = sched_getcpu();
   id_liberou = id;
   return NULL;
}

void push_srtn(Processo_srtn p, Processo_srtn v[], int * tam)
{
    int i;
    for(i = (*tam); i > 1; i--){
        if(p.dt <= v[i-1].dt){
            v[i].id = p.id;
            strcpy(v[i].nome, p.nome);
            v[i].t0 = p.t0;
            v[i].dt = p.dt;
            v[i].deadline = p.deadline;
            break;
        }
        v[i].id = v[i-1].id;
        strcpy(v[i].nome, v[i-1].nome);
        v[i].t0 = v[i-1].t0;
        v[i].dt = v[i-1].dt;
        v[i].deadline = v[i-1].deadline;
    }

    (*tam) = (*tam) + 1;
}

void pop_srtn(Processo_srtn v[], int * tam)
{
    int i;
    (*tam) = (*tam) - 1;
    for(i = 0; i < (*tam); i++){
        v[i].id = v[i+1].id;
        strcpy(v[i].nome, v[i+1].nome);
        v[i].t0 = v[i+1].t0;
        v[i].dt = v[i+1].dt;
        v[i].deadline = v[i+1].deadline;
    }
}

void pause_thread(int id){
    /*pausa*/
    pthread_mutex_lock(&lock[id]);
    play[id] = 0;
    pthread_mutex_unlock(&lock[id]);

    /*cpu está livre*/
    livre = 1;
}

void play_thread(int id){
    /*cpu não está mais livre*/
    livre = 0;
    /* se não foi criada ainda */
    int * argumento;
    if(play[id] == -1){
        play[id]  = 1;
        argumento = malloc(sizeof(int));
        (*argumento) = id;
        if(pthread_create(&tid[id], NULL, Thread_SRTN, argumento)){
            printf("ERRO ao criar a Thread\n");
            exit(1);
        }
    }
    else{ /* se a thread ja foi executada antes, só dar o play */
        pthread_mutex_lock(&lock[id]);
        play[id] = 1;
        pthread_cond_signal(&cond[id]);
        pthread_mutex_unlock(&lock[id]);
    }
}

void imprime(Processo_srtn v[], int tam){
   int i;
   printf("\nIMPRIMINDO\n");
   for(i = 0; i < tam; i++){
      printf("id:%d | nome: %s | t0:%d | dt:%d | deadline:%d\n",
      v[i].id, v[i].nome, v[i].t0, v[i].dt, v[i].deadline);
   }
   printf("ACABOU DE IMPRIMIR\n\n");
}

void srtn(FILE* arq_trace, FILE* arq_saida, int d)
{
    int i, j, k, num_prontos, num_proc, processo_atual;
    int tempo;
    int muda = 0;
    int tinha_alguem_antes;
    int saida_tf[MAXN], saida_tr[MAXN];

    i=j=k=0;
    num_proc=num_prontos = 0;
    liberou = 0;
    livre = 1;
    pthread_mutex_init(&mutex, NULL);
    flag = d;
   
    while(fscanf(arq_trace, "%s %d %d %d", processos[num_proc].nome, &processos[num_proc].t0, &processos[num_proc].dt, &processos[num_proc].deadline) != EOF){
        processos[num_proc].id = num_proc;
        pthread_mutex_init(&lock[num_proc], NULL);
        pthread_cond_init(&cond[num_proc], NULL);
        play[num_proc] = -1;
        num_proc++;
    }
    /* 
    Enquanto ainda tiver processos para executar, 
    ou seja, enquanto pelo menos uma das filas ainda
    nao estiver vazia 
    */
    processo_atual = -1;
    tempo = 0;
    while(num_prontos || (j != num_proc)){
        tinha_alguem_antes = 0;

        if(d) fprintf(stderr, "\nTempo: %d\n", tempo);

        if(liberou){
            tinha_alguem_antes = 1;
            pop_srtn(prontos, &num_prontos);
            if(d){ 
                fprintf(stderr, "Acabou a execução na CPU%d: %s %d %d %d\n", 
                c_liberou, processos[id_liberou].nome, processos[id_liberou].t0, processos[id_liberou].dt, processos[id_liberou].deadline);
                fprintf(stderr, "Linha a ser imprimida: %s %d %d\n", 
                processos[id_liberou].nome, tempo, tempo - processos[id_liberou].t0);
            }
            //fprintf(arq_saida, "%s %d %d\n", processos[id_liberou].nome, tempo, tempo - processos[id_liberou].t0);
            saida_tf[id_liberou] = tempo;
            saida_tr[id_liberou] = tempo - processos[id_liberou].t0;

            processo_atual = -1; /*não tem ninguem na cpu*/
            liberou = 0;
            livre = 1;
        }

        /* quem esta pronto vai pra fila de prontos */
        for(k = j; k < num_proc; k++){
            if(processos[k].t0 <= tempo){
                /* coloca na fila de prontos */
                push(processos[k], prontos, &num_prontos);
                j++;
                if(d){ 
                    fprintf(stderr, "Chegou processo: %s %d %d %d\n", 
                    processos[k].nome, processos[k].t0, processos[k].dt, processos[k].deadline);
                }
            }
            else break;
        }

        imprime(prontos, num_prontos);

        /* se tinha um processo na cpu que acabou agora */
        /* se tem alguem pronto */
        if(num_prontos){
            if(livre && num_prontos){ /* se a cpu está livre e tem pelo menos alguém pronto */
                /*printf("entrei: %d E %d\n", num_prontos, livre);*/
                processo_atual = prontos[0].id;
                play_thread(processo_atual);
                if(tinha_alguem_antes) muda++;
            }
            else{ /* se a cpu está ocupada */
               /* se o dt do que chegou é menor, então troca. caso contrário não faz nada */
               /* poderia ser processo_atual != processos[0].id */
               if(processos[processo_atual].dt > prontos[0].dt){
                    /*preempção*/
                    /*primeiro pausa quem ta executando*/
                    pause_thread(processo_atual);
                    printf("Pausei %d", processo_atual);
                    /*depois executa quem chegou*/
                    processo_atual = prontos[0].id;
                    play_thread(processo_atual);
                    printf(" e comecei %d\n", processo_atual);
                    muda++;
               }
            }
        }
        sleep(1);
        tempo++;
        /*atualiza*/
        if(processo_atual >= 0){
            processos[processo_atual].dt--;
            prontos[0].dt--;
        }
    }

    /*------------------------*/
    for(i = 0; i < num_proc; i++){
        if(pthread_join(tid[i], NULL)){
                printf("ERRO ao dar join na Thread %d\n", i);
                exit(1);
        }
    }

    for(i = 0; i < num_proc; i++)
        fprintf(arq_saida, "%s %d %d\n", processos[i].nome, saida_tf[i], saida_tr[i]);

    fprintf(arq_saida, "%d\n", muda);
}
