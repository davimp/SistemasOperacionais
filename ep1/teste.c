#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define NITER 1000000000
#define VEZES 5

int flag = 1;

typedef struct processo{
    int id;
    char nome[40];
    time_t t0;
    time_t dt;
    time_t deadline;
    time_t tempo_comecou;
    time_t tempo_acabou;
} Processo;

void imprime(Processo v[], int tam){
   int i;
   printf("\nIMPRIMINDO\n");
   for(i = 0; i < tam; i++){
      printf("id:%d | nome: %s | t0:%d | dt:%d | deadline:%d | tempo_comecou:%d | tempo_acabou:%d\n",
      v[i].id, v[i].nome, v[i].t0, v[i].dt, v[i].deadline, v[i].tempo_comecou, v[i].tempo_acabou);
   }
   printf("ACABOU DE IMPRIMIR\n\n");
}

void * Thread(void * a)
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

void push(Processo p, Processo v[], int * tam)
{
    Processo aux;
    int i;
    for(i = 0; i < (*tam); i++){
        if(p.dt < v[i].dt){
            /* troca */
            aux.id = p.id;
            /*aux.nome = p.nome;*/
            strcpy(aux.nome, p.nome);
            aux.t0 = p.t0;
            aux.dt = p.dt;
            aux.deadline = p.deadline;
            aux.tempo_comecou = p.tempo_comecou;
            aux.tempo_acabou = p.tempo_acabou;

            p.id = v[i].id;
            /*p.nome = v[i].nome;*/
            strcpy(p.nome, v[i].nome);
            p.t0 = v[i].t0;
            p.dt = v[i].dt;
            p.deadline = v[i].deadline;
            p.tempo_comecou = v[i].tempo_comecou;
            p.tempo_acabou = v[i].tempo_acabou;

            v[i].id = aux.id;
            /*v[i].nome = aux.nome;*/
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
    /*v[i].nome = p.nome;*/
    strcpy(v[i].nome, p.nome);
    v[i].t0 = p.t0;
    v[i].dt = p.dt;
    v[i].deadline = p.deadline;
    v[i].tempo_comecou = p.tempo_comecou;
    v[i].tempo_acabou = p.tempo_acabou;

    (*tam) = (*tam) + 1;
}

void pop_front(Processo v[], int * tam)
{
    int i;
    (*tam) = (*tam) - 1;
    for(i = 0; i < (*tam); i++){
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


int main() {
   Processo pronto[10000], processos[10000]; //colocar MAXN talvez
   pthread_t tid[10000];
   int x,y;
   pthread_mutex_t mut[10000];

   int mutex;
   int i, j, k, tam_prontos, tam_processos, processo_atual;
   char nome[50]; /* nome do 'processo'*/
   int t0, dt, deadline; /* dados do 'processo'*/
   int tempo, tempo2;
   int muda = 0;
   time_t tempo_inicial, tempo_atual, tempo_iniciou, tempo_acabou;
   int ok;
   tempo_inicial = time(NULL); /* começa a contar o tempo  */
   i=j=0;/* as filas estao vazias */
   tam_processos=k=tam_prontos = 0;

   while(scanf("%s %d %d %d", processos[tam_processos].nome, &processos[tam_processos].t0, &processos[tam_processos].dt, &processos[tam_processos].deadline) != EOF){
        processos[tam_processos].id = tam_processos;
        processos[tam_processos].tempo_comecou = -1;
        processos[tam_processos].tempo_acabou = -1;
        tam_processos++;
    }
   imprime(processos, tam_processos);

   for(i = 0; i < tam_processos; i++){
      push(processos[i], pronto, &tam_prontos);
   }

   imprime(pronto, tam_prontos);

   while(tam_prontos){
      pop_front(pronto, &tam_prontos);
      imprime(pronto, tam_prontos);
   }

   return 0;
}