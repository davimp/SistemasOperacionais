#include"rr.h"


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

void bota(Processo_rr p, Processo_rr v[], int * tam)
{
    Processo_rr aux;
    int i;
    i = (*tam);
    /* coloca na ultima posição */
    v[i].id = p.id;
    strcpy(v[i].nome, p.nome);
    v[i].t0 = p.t0;
    v[i].dt = p.dt;
    v[i].deadline = p.deadline;
    v[i].tempo_comecou = p.tempo_comecou;
    v[i].tempo_acabou = p.tempo_acabou;

    (*tam) = (*tam) + 1;
}

void tira(int index, Processo_rr v[], int * tam){
    int i;
    (*tam) = (*tam) - 1;
    for(i = index; i < (*tam); i++){
        /*[i] = [i+1]*/
        v[i].id = v[i+1].id;
        /*v[i].nome = v[i+1].nome;*/
        strcpy(v[i].nome, v[i+1].nome);
        v[i].t0 = v[i+1].t0;
        v[i].dt = v[i+1].dt;
        v[i].deadline = v[i+1].deadline;
        v[i].tempo_comecou = v[i+1].tempo_comecou;
        v[i].tempo_acabou = v[i+1].tempo_acabou;
    }
}

void rr(FILE* arq_trace, FILE* arq_saida, int d)
{
    Processo_rr pronto[10000], processos[10000]; //colocar MAXN talvez
    pthread_t tid[10000];
    int mutex;
    int i, j, k, tam_prontos, tam_processos, processo_atual;
    char nome[50]; /* nome do 'processo'*/
    int t0, dt, deadline; /* dados do 'processo'*/
    int tempo, tempo2;
    int muda = 0;
    time_t tempo_inicial, tempo_atual, tempo_iniciou, tempo_acabou;
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
        tam_processos++;
    }

    /* 
    Enquanto ainda tiver processos para executar, 
    ou seja, enquanto pelo menos uma das filas ainda
    nao estiver vazia   
    */
    processo_atual = -1;
    tempo_inicial = time(NULL);
    /* se tem algum processo a ser analisado ainda */
    while(tam_prontos || (j != tam_processos)){
        tempo_atual = time(NULL);
        tempo = tempo_atual - tempo_inicial;
        /* quem esta pronto vai pra fila de prontos */
        for(k = j; k < tam_processos; k++){
            if(processos[k].t0 <= tempo){
                /* coloca na fila de prontos */
                bota(processos[k], pronto, &tam_prontos);
                j++;
                /*momento de imprimir a hora de chegada*/
            }
            else break;
        }

        /* se tem alguem pronto */
        if(tam_prontos){
            /*executa um pouco de cada um*/
            /*checa se alguém acabou e tira da fila de prontos caso isso aconteca*/
        }
        sleep(1);
    }
    printf("%d\n", muda);
}
