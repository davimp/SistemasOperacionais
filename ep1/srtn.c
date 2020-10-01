#include"srtn.h"

Processo_srtn prontos[10000], processos[10000]; //colocar MAXN talvez
pthread_t tid[10000];
pthread_mutex_t lock[10000];
pthread_cond_t cond[10000];
int play[10000];

/* Função para as threads */
void * Thread_SRTN(void * a)
{
   long int count;
   int * arg = a;
   int id = (*arg);
   int dt = processos[id].dt;
   double tempo;
   count = -10000000;
   tempo = 0;
   while(tempo < dt){
      count = (count + rand())* rand();
      pthread_mutex_lock(&lock[id]);
      while(!play[id]){
        pthread_cond_wait(&cond[id], &lock[id]);
      }
      pthread_mutex_unlock(&lock[id]);
      usleep(10000);
      tempo += 0.01;
   }
   return NULL;
}

void push(Processo_srtn p, Processo_srtn v[], int * tam)
{
    Processo_srtn aux;
    int i;
    for(i = 0; i < (*tam); i++){
        if(p.dt < v[i].dt){
            /* troca */
            aux.id = p.id;
            strcpy(aux.nome, p.nome);
            aux.t0 = p.t0;
            aux.dt = p.dt;
            aux.deadline = p.deadline;

            p.id = v[i].id;
            strcpy(p.nome, v[i].nome);
            p.t0 = v[i].t0;
            p.dt = v[i].dt;
            p.deadline = v[i].deadline;

            v[i].id = aux.id;
            strcpy(v[i].nome, aux.nome);
            v[i].t0 = aux.t0;
            v[i].dt = aux.dt;
            v[i].deadline = aux.deadline;
        }
    }
    /* coloca o ultimo */
    v[i].id = p.id;
    strcpy(v[i].nome, p.nome);
    v[i].t0 = p.t0;
    v[i].dt = p.dt;
    v[i].deadline = p.deadline;

    (*tam) = (*tam) + 1;
}

void pop_front(Processo_srtn v[], int * tam)
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

void imprime(Processo_srtn v[], int tam){
   int i;
   printf("\nIMPRIMINDO\n");
   for(i = 0; i < tam; i++){
      printf("id:%d | nome: %s | t0:%d | dt:%d | deadline:%d\n",
      v[i].id, v[i].nome, v[i].t0, v[i].dt, v[i].deadline);
   }
   printf("ACABOU DE IMPRIMIR\n\n");
}


void pause_thread(int id){
    pthread_mutex_lock(&lock[id]);
    play[id] = 0;
    pthread_mutex_unlock(&lock[id]);
}

void play_thread(int id){
    /* se não foi criada ainda */
    if(play[id] == -1){
        play[id]  = 1;
        if(pthread_create(&tid[id], NULL, Thread_SRTN, (void*)&id)){
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


void srtn(FILE* arq_trace, FILE* arq_saida, int d)
{
    time_t tempo_inicial, tempo_atual;
    int i, j, k, num_prontos, num_proc, processo_atual;
    int tempo;
    int muda = 0;
    int flag;
    tempo_inicial = time(NULL); /* começa a contar o tempo  */
    i=j=k=0;
    num_proc=num_prontos = 0;
   
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
    tempo_inicial = time(NULL);

    while(num_prontos || (j != num_proc)){
        tempo_atual = time(NULL);
        tempo = tempo_atual - tempo_inicial;

        if(d) fprintf(stderr, "\nTempo: %d\n", tempo);

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

        /* se tem alguem pronto */
        if(num_prontos){
            if(processo_atual < 0 || processos[processo_atual].dt <= 0){ /* se a cpu está livre */
                flag = 0;
                /* coloca na cpu */
                /*se tinha um processo na cpu que acabou agora*/
                if(processo_atual >= 0){
                    flag = 1;
                    pop_front(prontos, &num_prontos);
                    if(d){ 
                        fprintf(stderr, "Acabou a execução: %s %d %d %d\n", 
                        processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
                        fprintf(stderr, "Linha a ser imprimida: %s %d %d\n", 
                        processos[processo_atual].nome, tempo, tempo - processos[processo_atual].t0);
                    }
                    fprintf(arq_saida, "%s %d %d\n", processos[processo_atual].nome, tempo, tempo - processos[processo_atual].t0);
                    processo_atual = -1; /*não tem ninguem na cpu*/
                }

                /*se algum estiver pronto, coloca ele na cpu */
                if(num_prontos){
                    processo_atual = prontos[0].id;
                    play_thread(processo_atual);
                    if(d){ 
                        fprintf(stderr, "Começou a executar na CPU: %s %d %d %d\n", 
                        processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
                    }
                    if(flag) muda++;
                }
            }
            else{ /* se a cpu está ocupada */
               if(processos[processo_atual].dt > prontos[0].dt){/* poderia ser processo_atual != processos[0].id */
                    /*preempção*/

                    /*primeiro pausa quem ta executando*/
                    pause_thread(processo_atual);
                    if(d){ 
                        fprintf(stderr, "Pausou a execução na CPU: %s %d %d %d\n", 
                        processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
                    }

                    /*depois executa quem chegou*/
                    processo_atual = prontos[0].id;
                    play_thread(processo_atual);
                    if(d){ 
                        fprintf(stderr, "Começou a executar na CPU: %s %d %d %d\n",
                        processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
                    }
                   muda++;
               }
            }
        }
        sleep(1);
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
    fprintf(arq_saida, "%d\n", muda);
}
