#include"rr.h"

#define QUANTUM 1

pthread_t tid[10000];
pthread_mutex_t lock[10000];
pthread_cond_t cond[10000];
int play[10000];

/* Função para as threads */
void * Thread_RR(void * a)
{
   long int count;
   time_t tempo_inicial, tempo_atual;
   int * arg = a;
   int dt = (*arg);
   tempo_inicial = time(NULL);
   tempo_atual = time(NULL);
   count = -10000000;
   while((tempo_atual - tempo_inicial) < dt){
      count++;
      tempo_atual = time(NULL);
   }
   return NULL;
}

void atribui_processo(Processo_rr *p, Processo_rr *q)
{
    q->id = p->id;
    strcpy(q->nome, p->nome);
    q->t0 = p->t0;
    q->dt = p->dt;
    q->deadline = p->deadline;
    q->tempo_comecou = p->tempo_comecou;
    q->tempo_acabou = p->tempo_acabou;
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

void rr(FILE* arq_trace, FILE* arq_saida, int d)
{
    Processo_rr pronto[10000], processos[10000]; //colocar MAXN talvez
    Processo_rr aux;
    pthread_t tid[10000];
    int mutex;
    int i, j, k, tam_prontos, tam_processos, processo_atual;
    char nome[50]; /* nome do 'processo'*/
    int t0, dt, deadline; /* dados do 'processo'*/
    int tempo, tempo2;
    int muda = 0;
    time_t tempo_inicial, tempo_atual, tempo_iniciou, tempo_acabou;
    time_t tempo_processamento, tempo_comeca_processo;
    int ok;
    tempo_inicial = time(NULL); /* começa a contar o tempo  */
    i=j=k=0;
    tam_processos=tam_prontos = 0;

    /* lê os processos e coloca no vetor de processos */
    while(fscanf(arq_trace, "%s %d %d %d", nome, &t0, &dt, &deadline) != EOF){
        processos[tam_processos].id = tam_processos;
        strcpy(processos[tam_processos].nome, nome);
        processos[tam_processos].t0 = t0;
        processos[tam_processos].dt = dt;
        processos[tam_processos].deadline = deadline;
        processos[tam_processos].tempo_comecou = -1;
        processos[tam_processos].tempo_acabou = -1;
        pthread_mutex_init(&lock[tam_processos], NULL);
        pthread_cond_init(&cond[tam_processos], NULL);
        play[tam_processos] = -1;
        tam_processos++;
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
    /* se tem algum processo a ser analisado ainda */
    while(tam_prontos || (j != tam_processos)){

        tempo_atual = time(NULL);
        tempo = tempo_atual - tempo_inicial;

        /* quem esta pronto vai pra fila de prontos */
        for(k = j; k < tam_processos; k++){
            if(processos[k].t0 <= tempo){
                /* coloca na fila de prontos */
                bota(processos[k], pronto, &tam_prontos);
                if(d){ 
                    fprintf(stderr, "Chegou processo: %s %d %d %d\n", processos[k].nome, processos[k].t0, processos[k].dt, processos[k].deadline);
                }
                j++;
                /*momento de imprimir a hora de chegada*/
            }
            else break;
        }


        /*checa se alguém acabou e tira da fila de prontos caso isso aconteca*/
        if(tempo_processamento != -1)
            tempo_processamento = time(NULL) - tempo_comeca_processo;

        if(tempo_processamento != -1 && tempo_processamento >= aux.dt)
        {
            tempo_processamento = -1;
            //finalizar processo
        }
        else if(tempo_processamento >= QUANTUM)
        {
            aux.dt -= tempo_processamento;
            bota(aux, pronto, &tam_prontos);
            aux.dt = -2;
            tempo_processamento = -1;
            //preemptar
            /*preempção*/
            /*pausa quem ta executando*/
            pthread_mutex_lock(&lock[aux.id]);
            play[aux.id] = 0;
            pthread_mutex_unlock(&lock[aux.id]);
            if(d){ 
                fprintf(stderr, "Pausou a execução na CPU: %s %d %d %d\n", processos[aux.id].nome, processos[aux.id].t0, processos[aux.id].dt, processos[aux.id].deadline);
            }
        }

        /*executa um pouco de cada um*/
        if(tempo_processamento == -1 && tam_prontos > 0)
        {
            atribui_processo(pronto, &aux);
            tira(0, pronto, &tam_prontos);
            tempo_processamento = 0;
            tempo_comeca_processo = time(NULL);
            //tirar a preempção ou criar thread
            /* se ainda não foi criada*/
            if(play[aux.id] == -1)
            {
                play[aux.id]  = 1;
                if(pthread_create(&tid[aux.id], NULL, Thread_RR, (void*)&processos[aux.id].id)){
                    printf("ERRO ao criar a Thread\n");
                    exit(1);
                }
            }
            /* se a thread ja foi executada antes, só dar o play */
            else
            { 
                pthread_mutex_lock(&lock[aux.id]);
                play[aux.id] = 1;
                pthread_cond_signal(&cond[aux.id]);
                pthread_mutex_unlock(&lock[aux.id]);
            }
            if(d){ 
                fprintf(stderr, "Começou a executar na CPU: %s %d %d %d\n", processos[aux.id].nome, processos[aux.id].t0, processos[aux.id].dt, processos[aux.id].deadline);
            }
            muda++;
        }
        
        sleep(1);
    }
    printf("%d\n", muda);
}
