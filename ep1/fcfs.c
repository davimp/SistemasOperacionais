#include "fcfs.h"

int livre;

/* Função para a thread */
void * Thread_FCFS(void * a) {
   livre = 0;
   long int count;
   time_t tempo_inicial, tempo_atual;
   int * arg = a;
   int dt;
   dt = (*arg);
   tempo_inicial = time(NULL);
   tempo_atual = time(NULL);
   count = -10000000;
   while((tempo_atual - tempo_inicial) < dt){
      count++;
      tempo_atual = time(NULL);
   }
   livre = 1;
   return NULL;
}

void push_fcfs(Processo_fcfs p, Processo_fcfs v[], int * tam)
{
    Processo_fcfs aux;
    int i;
    i = (*tam);
    /* coloca na ultima posição */
    v[i].id = p.id;
    strcpy(v[i].nome, p.nome);
    v[i].t0 = p.t0;
    v[i].dt = p.dt;
    v[i].deadline = p.deadline;
    (*tam) = (*tam) + 1;
}


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

void fcfs(FILE* arq_trace, FILE* arq_saida, int d)
{
    int tempo, tf;
    int muda;
    pthread_t tid[10000];//MAXN
    time_t tempo_inicial, tempo_atual;
    int processo_atual;
    int ok;
    int i, j, k;
    Processo_fcfs processos[1000], prontos[10000]; //MAXN
    int pos_proc; // Índice do próximo processo a chegar
    int num_proc, num_prontos;
    muda = 0;
    livre = 1;
    num_proc = 0;

    while(fscanf(arq_trace, "%s %d %d %d", processos[num_proc].nome, &processos[num_proc].t0, &processos[num_proc].dt, &processos[num_proc].deadline) != EOF) //lê os processos
    {    
        processos[num_proc].id = num_proc;
        num_proc++;
        fprintf(stderr, "num_proc: %d %s\n", num_proc, processos[num_proc-1].nome);
    }
    

    j = 0; /*ninguem esta pronto ainda */
    processo_atual = -1; /* ninguem rodando na cpu */
    livre = 1; /* cpu esta livre */
    tempo_inicial = time(NULL);
    while(num_prontos || (j < num_proc) || !livre){
        tempo_atual = time(NULL);
        tempo = tempo_atual - tempo_inicial;

        if(d) fprintf(stderr, "\nTempo: %d\n", tempo);

        /* quem esta pronto vai pra fila de prontos */
        for(k = j; k < num_proc; k++){
            if(processos[k].t0 <= tempo){
                /* coloca na fila de prontos */
                push_fcfs(processos[k], prontos, &num_prontos);
                j++;
                if(d){ 
                    fprintf(stderr, "Chegou processo: %s %d %d %d\n", processos[k].nome, processos[k].t0, processos[k].dt, processos[k].deadline);
                }
            }
            else break;
        }
        /*imprime(prontos, num_prontos);*/

        /* se tiver alguém pronto e a cpu estiver livre */
        if(livre){
            /*checa se algum processo acabou de terminar*/
            if(processo_atual >= 0){
                /*tf = tempo;*/
                fprintf(arq_saida, "%s %d %d\n", processos[processo_atual].nome, tempo, tempo - processos[processo_atual].t0);
                if(d){ 
                    fprintf(stderr, "Acabou a execução: %s %d %d %d\n", processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
                    fprintf(stderr, "Linha a ser imprimida: %s %d %d\n", processos[processo_atual].nome, tempo, tempo - processos[processo_atual].t0);
                }
                processo_atual = -1;
            }

            if(num_prontos){
                processo_atual = pop_fcfs(prontos, &num_prontos);
                if(pthread_create(&tid[processo_atual], NULL, Thread_FCFS, (void*)&processos[processo_atual].dt)){
                    printf("ERRO ao criar a Thread\n");
                    exit(1);
                }
                if(d){ 
                    fprintf(stderr, "Começou a executar na CPU: %s %d %d %d\n", processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
                }
            }
        }
        if(d) fprintf(stderr, "Mudanças de contexto até agora: %d\n", muda);
        sleep(1);
    }
    /*-------------------------------*/
    for(i = 0; i < num_proc; i++){
        if(pthread_join(tid[i], NULL)){
                printf("ERRO ao dar join na Thread\n");
                exit(1);
        }
    }
    /* imprime o último */
    tempo_atual = time(NULL);
    tempo = tempo_atual - tempo_inicial;
    if(d) fprintf(stderr, "\nTempo: %d\n", tempo);
    if(d){ 
        fprintf(stderr, "Acabou a execução: %s %d %d %d\n", 
        processos[processo_atual].nome, processos[processo_atual].t0, processos[processo_atual].dt, processos[processo_atual].deadline);
        fprintf(stderr, "Linha a ser imprimida: %s %d %d\n", 
        processos[processo_atual].nome, tempo, tempo - processos[processo_atual].t0);
    }
    fprintf(arq_saida, "%s %d %d\n", processos[processo_atual].nome, tempo, tempo - processos[processo_atual].t0);
    fprintf(arq_saida, "%d\n", muda);
}