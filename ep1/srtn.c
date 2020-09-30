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
   printf("debug: %d %d\n", id, dt);
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
            aux.tempo_comecou = p.tempo_comecou;
            aux.tempo_acabou = p.tempo_acabou;

            p.id = v[i].id;
            strcpy(p.nome, v[i].nome);
            p.t0 = v[i].t0;
            p.dt = v[i].dt;
            p.deadline = v[i].deadline;
            p.tempo_comecou = v[i].tempo_comecou;
            p.tempo_acabou = v[i].tempo_acabou;

            v[i].id = aux.id;
            strcpy(v[i].nome, aux.nome);
            v[i].t0 = aux.t0;
            v[i].dt = aux.dt;
            v[i].deadline = aux.deadline;
            v[i].tempo_comecou = aux.tempo_comecou;
            v[i].tempo_acabou = aux.tempo_acabou;
        }
    }
    /* coloca o ultimo */
    v[i].id = p.id;
    strcpy(v[i].nome, p.nome);
    v[i].t0 = p.t0;
    v[i].dt = p.dt;
    v[i].deadline = p.deadline;
    v[i].tempo_comecou = p.tempo_comecou;
    v[i].tempo_acabou = p.tempo_acabou;

    (*tam) = (*tam) + 1;
}

void pop_front(Processo_srtn v[], int * tam)
{
    int i;
    (*tam) = (*tam) - 1;
    for(i = 0; i < (*tam); i++){
        /*[i] = [i+1]*/
        v[i].id = v[i+1].id;
        strcpy(v[i].nome, v[i+1].nome);
        v[i].t0 = v[i+1].t0;
        v[i].dt = v[i+1].dt;
        v[i].deadline = v[i+1].deadline;
        v[i].tempo_comecou = v[i+1].tempo_comecou;
        v[i].tempo_acabou = v[i+1].tempo_acabou;
    }
}
/* EXPLICAÇÃO DA IDEIA
- duas filas: uma de prontos e uma de nao prontos
- a fila de prontos sera ordenada pelo dt
- struct para guardar as informacoes dos Processo_srtns

    1) coloco tudo na fila de não prontos
    2) enquanto as duas filas não estiverem vazias eu
    faço o loop do 'clock':
        2.1) primeiro coloco na fila de prontos quem ja esta pronto em ordem de menor dt
        2.2) se a fila de prontos não estiver vazia, eu olho pro primeiro da fila
        e tento colocar ele na cpu. eu só coloco ele na cpu se o dt dele for menor do dt
        do Processo_srtn que ja ta executando ou se a cpu estiver livre. caso contrario eu nao
        faço nada.
        3) se tiver algum Processo_srtn executando na cpu diminuimos o dt dele no final do while
        (final do 'clock') e dou um sleep(1)
*/

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
    /*Processo_srtn prontos[10000], processos[10000]; //colocar MAXN talvez
    pthread_t tid[10000];*/
    time_t tempo_inicial, tempo_atual;
    int i, j, k, num_prontos, num_proc, processo_atual;
    int tempo;
    int muda = 0;
    int ok;
    tempo_inicial = time(NULL); /* começa a contar o tempo  */
    i=j=k=0;
    num_proc=num_prontos = 0;
    printf("debug\n");
    /* lê os processos e coloca no vetor de processos */
    puts("NEM LEU");
    while(fscanf(arq_trace, "%s %d %d %d", processos[num_proc].nome, &processos[num_proc].t0, &processos[num_proc].dt, &processos[num_proc].deadline) != EOF){
        processos[num_proc].id = num_proc;
        processos[num_proc].tempo_comecou = -1;
        processos[num_proc].tempo_acabou = -1;
        pthread_mutex_init(&lock[num_proc], NULL);
        pthread_cond_init(&cond[num_proc], NULL);
        play[num_proc] = -1;
        num_proc++;
    }

    imprime(processos, num_proc);

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
                    fprintf(stderr, "Chegou processo: %s %d %d %d\n", processos[k].nome, processos[k].t0, processos[k].dt, processos[k].deadline);
                }
            }
            else break;
        }
        imprime(prontos, num_prontos);

        /* se tem alguem pronto */
        if(num_prontos){
            if(processo_atual < 0 || processos[processo_atual].dt <= 0){ /* se a cpu está livre */
                /* ERRADO! PODE SER QUE A THREAD JA TENHA SIDO CRIADA */
                /* coloca na cpu */
                if(processo_atual >= 0){
                    pop_front(prontos, &num_prontos);
                    if(d){ 
                        fprintf(stderr, "Acabou a execução: %s %d %d %d\n", processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
                        fprintf(stderr, "Linha a ser imprimida: %s %d %d\n", processos[processo_atual].nome, tempo, tempo - processos[processo_atual].t0);
                    }
                    fprintf(arq_saida, "%s %d %d\n", processos[processo_atual].nome, tempo, tempo - processos[processo_atual].t0);
                }
                if(num_prontos){
                    processo_atual = prontos[0].id;
                    /* se ainda não foi criada*/
                    if(play[processo_atual] == -1){
                        play[processo_atual]  = 1;
                        if(pthread_create(&tid[processo_atual], NULL, Thread_SRTN, (void*)&processos[processo_atual].id)){
                            printf("ERRO ao criar a Thread\n");
                            exit(1);
                        }
                    }
                    else{ /* se a thread ja foi executada antes, só dar o play */
                        pthread_mutex_lock(&lock[processo_atual]);
                        play[processo_atual] = 1;
                        pthread_cond_signal(&cond[processo_atual]);
                        pthread_mutex_unlock(&lock[processo_atual]);
                    }
                    if(d){ 
                        fprintf(stderr, "Começou a executar na CPU: %s %d %d %d\n", processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
                    }
                    muda++;
                }
            }
            else{
               if(processos[processo_atual].dt > prontos[0].dt){/* poderia ser processos[processo_atual].id != processos[0].id*/
                   /*preempção*/
                   /*primeiro pausa quem ta executando*/
                   pthread_mutex_lock(&lock[processo_atual]);
                   play[processo_atual] = 0;
                   pthread_mutex_unlock(&lock[processo_atual]);
                   if(d){ 
                        fprintf(stderr, "Pausou a execução na CPU: %s %d %d %d\n", processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
                    }
                   /*executa quem chegou agora*/
                   processo_atual = prontos[0].id;
                   play[processo_atual] = 1;
                   if(pthread_create(&tid[processo_atual], NULL, Thread_SRTN, (void*)&processos[processo_atual].id)){
                      printf("ERRO ao criar a Thread\n");
                      exit(1);
                   }
                   if(d){ 
                        fprintf(stderr, "Começou a executar na CPU: %s %d %d %d\n", processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
                    }
                   muda++;
               }
            }
        }
        sleep(1);
        if(processo_atual >= 0){
            processos[processo_atual].dt--;/* dt pe uma informação que na pratica gente não tem*/
            prontos[0].dt--;
            /*checa se liberou a cpu*/
            /* se acabou de rodar, tira ele da fila de prontos ta fazendo em outro lugar já*/
        }
    }
    puts("cheguei aqui\n");
    /*------------------------*/
    for(i = 0; i < num_proc; i++){
        if(pthread_join(tid[i], NULL)){
                printf("ERRO ao dar join na Thread %d\n", i);
                exit(1);
        }
    }
    /* imprime o último */
    tempo_atual = time(NULL);
    tempo = tempo_atual - tempo_inicial;
    fprintf(arq_saida, "%s %d %d\n", processos[processo_atual].nome, tempo, tempo - processos[processo_atual].t0);
    fprintf(arq_saida, "%d\n", muda);
}
