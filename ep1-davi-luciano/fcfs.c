#include "fcfs.h"

#define MAXN 10000
Processo_fcfs processos[MAXN], prontos[MAXN];
pthread_t tid[MAXN];
pthread_mutex_t mutex;
int livre;
int liberou;
int c_liberou;
int id_liberou;
int flag;

/* Função para a thread */
void * Thread_FCFS(void * a) {
   livre = 0;
   time_t tempo1, tempo2;
   long long tempo;
   int * arg = a;
   int id;
   int dt;
   id = (*arg);
   dt = processos[id].dt;
   free(arg);
   tempo = 0;
   tempo1 = time(NULL);
   if(flag) fprintf(stderr, "Começou a executar na CPU%d: %s\n", sched_getcpu(), processos[id].nome);
   while(tempo < dt){
      tempo2 = time(NULL);
      tempo = tempo2 - tempo1;
   }
   liberou = 1;
   c_liberou = sched_getcpu();
   id_liberou = id;
   livre = 1;
   return NULL;
}

/*   
    Coloca o processo p na ultima posição do vetor de processos v
    e atualiza o tamanho do vetor.
*/
void push_fcfs(Processo_fcfs p, Processo_fcfs v[], int * tam)
{
    Processo_fcfs aux;
    int i;
    i = (*tam);
    v[i].id = p.id;
    strcpy(v[i].nome, p.nome);
    v[i].t0 = p.t0;
    v[i].dt = p.dt;
    v[i].deadline = p.deadline;
    (*tam) = (*tam) + 1;
}

/*
    Remove o primeiro processo do vetor v
    e retorna o id dele.
*/
int pop_fcfs(Processo_fcfs v[], int * tam)
{
    int i;
    int ret;
    ret = v[0].id;
    (*tam) = (*tam) - 1;
    for(i = 0; i < (*tam); i++){
        /*[i] = [i+1]*/
        v[i].id = v[i+1].id;
        strcpy(v[i].nome, v[i+1].nome);
        v[i].t0 = v[i+1].t0;
        v[i].dt = v[i+1].dt;
        v[i].deadline = v[i+1].deadline;
    }
    return ret;
}


/*
    'main'
*/
void fcfs(FILE* arq_trace, FILE* arq_saida, int d)
{
    int tempo;
    int muda;
    int *argumento;
    int processo_atual;
    int acabou_de_liberar;
    int i, j, k;
    int num_proc, num_prontos;
    int saida_tf[MAXN], saida_tr[MAXN];
    int estourou; /*guarda quantos processos estouraram a deadline*/
    
    liberou = 0;
    muda = 0;
    livre = 1;
    num_proc=num_prontos=0;
    pthread_mutex_init(&mutex, NULL);
    flag = d;

    while(fscanf(arq_trace, "%s %d %d %d", processos[num_proc].nome, &processos[num_proc].t0, &processos[num_proc].dt, &processos[num_proc].deadline) != EOF) //lê os processos
    {    
        processos[num_proc].id = num_proc;
        num_proc++;
    }
    

    j = 0; /* ninguem esta pronto ainda */
    processo_atual = -1; /* ninguem rodando na cpu */
    tempo = 0;
    while(num_prontos || (j < num_proc) || processo_atual >= 0){
        acabou_de_liberar = 0;
        if(d) fprintf(stderr, "\nTempo: %d\n", tempo);

        /* checa se algum processo acabou de terminar*/
        if(liberou){
            acabou_de_liberar = 1;
            if(d){ 
                fprintf(stderr, "Acabou a execução na CPU%d: %s %d %d %d\n", 
                c_liberou, processos[id_liberou].nome, processos[id_liberou].t0, processos[id_liberou].dt, processos[id_liberou].deadline);
                fprintf(stderr, "Linha a ser imprimida: %s %d %d\n", 
                processos[id_liberou].nome, tempo, tempo - processos[id_liberou].t0);
            }
            saida_tf[id_liberou] = tempo;
            saida_tr[id_liberou] = tempo - processos[id_liberou].t0;

            processo_atual = -1;
            liberou = 0;
            livre = 1;
        }

        /* quem esta pronto vai pra fila de prontos */
        for(k = j; k < num_proc; k++){
            if(processos[k].t0 <= tempo){
                /* coloca na fila de prontos */
                push_fcfs(processos[k], prontos, &num_prontos);
                j++;
                if(d){ 
                    fprintf(stderr, "Chegou processo: %s %d %d %d\n", 
                    processos[k].nome, processos[k].t0, processos[k].dt, processos[k].deadline);
                }
            }
            else break;
        }


        /* se tiver alguém pronto e a cpu estiver livre */
        if(livre && num_prontos){
            processo_atual = pop_fcfs(prontos, &num_prontos);
            argumento = malloc(sizeof(int));
            (*argumento) = processos[processo_atual].id;
            if(pthread_create(&tid[processo_atual], NULL, Thread_FCFS, (void*)argumento)){
                printf("ERRO ao criar a Thread\n");
                exit(1);
            }
            if(acabou_de_liberar) muda++;
        }

        sleep(1);
        if(d) fprintf(stderr, "Mudanças de contexto até agora: %d\n", muda);
        tempo++;
    }

    /*-------------------------------*/
    for(i = 0; i < num_proc; i++){
        if(pthread_join(tid[i], NULL)){
                printf("ERRO ao dar join na Thread\n");
                exit(1);
        }
    }

    estourou = 0;
    /*imprime arquivo de saida */
    for(i = 0; i < num_proc; i++){
        fprintf(arq_saida, "%s %d %d\n", processos[i].nome, saida_tf[i], saida_tr[i]);
        if(saida_tf[i] > processos[i].deadline) estourou++;
    }

    fprintf(arq_saida, "%d", muda);
    /*fprintf(arq_saida, "%d\n", estourou);*/
}