#include"rr.h"

#define QUANTUM 1

Processo_rr pronto[10000], processos[10000]; //colocar MAXN talvez
pthread_t tid[10000];
pthread_mutex_t lock[10000];
pthread_cond_t cond[10000];
int play[10000];

int liberou;
char s_liberou[40];
int c_liberou;
int id_liberou;

/* Função para as threads */
void * Thread_RR(void * a)
{
   long int count;
   int * arg = a;
   int id = (*arg);
   free(arg);
   int dt = processos[id].dt;
   double tempo;
   time_t t1, t2, t3, t4;
   printf("debug:%d %d\n", id, dt);
   count = -10000000;
   tempo = 0;
   t1 = time(NULL);
   printf("%s comecou na CPU%d\n", processos[id].nome, sched_getcpu());
   while((t2 - t1) < dt){
       t2 = time(NULL);
       t3=t4=0;
      count++;
      pthread_mutex_lock(&lock[id]);
      while(!play[id]){
        printf("%s pausou na CPU%d\n", processos[id].nome, sched_getcpu());
        t3 = time(NULL);
        pthread_cond_wait(&cond[id], &lock[id]);
        t4 = time(NULL);
        printf("%s voltou na  CPU%d\n", processos[id].nome, sched_getcpu());
      }
      pthread_mutex_unlock(&lock[id]);
      dt += (int)(t4-t3);
   }
   printf("%s liberou na CPU%d\n", processos[id].nome, sched_getcpu());

    liberou = 1;
    strcpy(s_liberou, processos[id].nome);
    c_liberou = sched_getcpu();
    id_liberou = id;


   return NULL;
}

void atribui_processo(Processo_rr *p, Processo_rr *q)
{
    q->id = p->id;
    strcpy(q->nome, p->nome);
    q->t0 = p->t0;
    q->dt = p->dt;
    q->deadline = p->deadline;
}

void bota(Processo_rr p, Processo_rr v[], int * tam)
{
    Processo_rr aux;
    int i;
    i = (*tam);
    /* coloca na ultima posição */
    atribui_processo(&p, v+i);

    (*tam) = (*tam) + 1;
}

void tira(int index, Processo_rr v[], int * tam){
    int i;
    (*tam) = (*tam) - 1;
    for(i = index; i < (*tam); i++){
        atribui_processo(v+i+1, v+i);
    }
}

void pausa_thread(int id){
    pthread_mutex_lock(&lock[id]);
    play[id] = 0;
    pthread_mutex_unlock(&lock[id]);
}

void executa_thread(int id){
    /* se não foi criada ainda */
   int * argumento;
    if(play[id] == -1){
        play[id]  = 1;
        argumento = malloc(sizeof(int));
        (*argumento) = id;
        if(pthread_create(&tid[id], NULL, Thread_RR, argumento)){
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


void rr(FILE* arq_trace, FILE* arq_saida, int d)
{
    Processo_rr aux;
    int i, j, k, tam_prontos, num_proc, processo_atual;
    char nome[50]; /* nome do 'processo'*/
    int t0, dt, deadline; /* dados do 'processo'*/
    int tempo;
    int muda = 0;
    time_t tempo_inicial, tempo_atual;
    time_t tempo_processamento, tempo_comeca_processo;
    int ok;
    tempo_inicial = time(NULL); /* começa a contar o tempo  */
    i=j=k=0;
    num_proc=tam_prontos = 0;

    /* lê os processos e coloca no vetor de processos */
    while(fscanf(arq_trace, "%s %d %d %d", nome, &t0, &dt, &deadline) != EOF){
        processos[num_proc].id = num_proc;
        strcpy(processos[num_proc].nome, nome);
        processos[num_proc].t0 = t0;
        processos[num_proc].dt = dt;
        processos[num_proc].deadline = deadline;
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
    tempo_processamento = -1;
    aux.dt = -2;
    liberou = 0;
    int pode_mudar; // 1 se um processo preemptou ou finalizou, 0 caso contrario
    /* se tem algum processo a ser analisado ainda */
    while(tam_prontos || (j != num_proc) || (tempo_processamento != -1)){

        tempo_atual = time(NULL);
        tempo = tempo_atual - tempo_inicial;
        pode_mudar = 0;

        if(d) fprintf(stderr, "\nTempo: %d\n", tempo);

        if(liberou)
        {
            //printf("%s liberou na CPU%d ---> id: %d\n", s_liberou, c_liberou, id_liberou);
            fprintf(arq_saida, "%s %d %d\n", s_liberou, tempo, tempo - processos[id_liberou].t0);
            liberou = 0;
        }

        /* quem esta apronto vai pra fila de prontos */
        for(k = j; k < num_proc; k++){
            if(processos[k].t0 <= tempo){
                /* coloca na fila de prontos */
                bota(processos[k], pronto, &tam_prontos);
                if(d){
                    fprintf(stderr, "Chegou processo: %s %d %d %d\n", 
                    processos[k].nome, processos[k].t0, processos[k].dt, processos[k].deadline);
                }
                j++;
            }
            else break;
        }


        /*checa se alguém acabou e tira da fila de prontos caso isso aconteca*/
        if(tempo_processamento != -1)
            tempo_processamento = time(NULL) - tempo_comeca_processo;

        if(tempo_processamento != -1 && tempo_processamento >= aux.dt)
        {
            tempo_processamento = -1;
            /*finalizar processo*/
            if(d){
                fprintf(stderr, "Acabou a execução: %s %d %d %d\n", 
                processos[aux.id].nome, processos[aux.id].t0, processos[aux.id].dt, processos[aux.id].deadline);
                fprintf(stderr, "Linha a ser imprimida: %s %d %d\n", 
                processos[aux.id].nome, tempo, tempo - processos[aux.id].t0);
            }
            pode_mudar = 1;
        }
        else if(tempo_processamento >= QUANTUM)
        {
            aux.dt -= QUANTUM;
            bota(aux, pronto, &tam_prontos);
            tempo_processamento = -1;
            /*preemptar*/
            /*preempção*/
            /*pausa quem ta executando*/
            pausa_thread(aux.id);
            if(d){
                fprintf(stderr, "Pausou a execução na CPU: %s %d %d %d\n", processos[aux.id].nome, processos[aux.id].t0, processos[aux.id].dt, processos[aux.id].deadline);
                fprintf(stderr, "(2) Pausou a execução na CPU: %d\n", aux.dt);
            }
            //aux.dt = -2; /*isso ta certo? sim*/
            pode_mudar = 1;
        }

        /* executa um pouco de cada um */
        if(tempo_processamento == -1 && tam_prontos > 0)
        {
            if(pronto[0].id != aux.id && pode_mudar)
                muda++;
            atribui_processo(pronto, &aux);
            tira(0, pronto, &tam_prontos);
            tempo_processamento = 0;
            tempo_comeca_processo = time(NULL);
            /* tirar a preempção ou criar thread */
            executa_thread(aux.id);
            if(d){ 
                fprintf(stderr, "Começou a executar na CPU: %s %d %d %d\n", processos[aux.id].nome, processos[aux.id].t0, processos[aux.id].dt, processos[aux.id].deadline);
            }
        }
        
        sleep(1);
    }

    /*------------------------------*/
    for(i = 0; i < num_proc; i++){
        if(pthread_join(tid[i], NULL)){
                printf("ERRO ao dar join na Thread %d\n", i);
                exit(1);
        }
    }

    fprintf(arq_saida, "%d\n", muda);

    if(d)
        fprintf(stderr, "Mudanças de contexto: %d\n", muda);
}
