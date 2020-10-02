#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sched.h>
#include<sys/time.h>

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

Processo pronto[10000], processos[10000]; //colocar MAXN talvez
pthread_t tid[10000];
pthread_mutex_t lock[10000];
pthread_cond_t cond[10000];
int play[10000];

void imprime(Processo v[], int tam){
   int i;
   printf("\nIMPRIMINDO\n");
   for(i = 0; i < tam; i++){
      printf("id:%d | nome: %s | t0:%d | dt:%d | deadline:%d\n",
      v[i].id, v[i].nome, v[i].t0, v[i].dt, v[i].deadline,);
   }
   printf("ACABOU DE IMPRIMIR\n\n");
}

void * Thread(void * a)
{
   int * arg = a;
   int id = (*arg);
   long long dt = (long long)processos[id].dt;
   int cont;
   long long tempo, espera;
   int T;
   /*time_t t1, t2, t3, t4;*/
   struct timeval t0, t1, t2, t3;
   struct timespec ts;
   ts.tv_sec=0;
   ts.tv_nsec=1000000;
   free(arg);
   printf("debug %d %d\n", id, dt);
   tempo = 0.;
   /*t1 = time(NULL);*/
   dt = dt;
   gettimeofday(&t1, NULL); 
   gettimeofday(&t0, NULL); 
   printf("inicio: %d\n", sched_getcpu());
   T = 0;
   while(T < dt*1000){
      cont = 0;
      espera = 0;
      /*gettimeofday(&t2, NULL);
      gettimeofday(&t3, NULL);*/
      pthread_mutex_lock(&lock[id]);
      while(!play[id]){
         /*gettimeofday(&t2, NULL);*/
         printf("pausou: %d\n", sched_getcpu());
         pthread_cond_wait(&cond[id], &lock[id]);
         printf("voltou: %d\n", sched_getcpu());
         /*gettimeofday(&t3, NULL);*/
         cont = 1;
      }
      pthread_mutex_unlock(&lock[id]);
      /*usleep(10000);
      tempo += 0.01;*/
      /*gettimeofday(&t1, NULL); 
      tempo = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
      espera = (t3.tv_sec-t2.tv_sec)*1000000 + t3.tv_usec-t2.tv_usec;*/
      /*if(cont){
          dt += espera;
      }*/
      nanosleep(&ts,NULL);
      T += 1;
   }
   printf("final: %d\n", sched_getcpu());
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



void pause_thread(int id){
    pthread_mutex_lock(&lock[id]);
    play[id] = 0;
    pthread_mutex_unlock(&lock[id]);
}

void play_thread(int id){
    pthread_mutex_lock(&lock[id]);
    play[id] = 1;
    pthread_cond_signal(&cond[id]);
    pthread_mutex_unlock(&lock[id]);
}

int main() {
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
   pthread_mutex_init(&lock[0], NULL);
   pthread_cond_init(&cond[0], NULL);
   while(scanf("%s %d %d %d", processos[tam_processos].nome, &processos[tam_processos].t0, &processos[tam_processos].dt, &processos[tam_processos].deadline) != EOF){
        processos[tam_processos].id = tam_processos;
        processos[tam_processos].tempo_comecou = -1;
        processos[tam_processos].tempo_acabou = -1;
        play[tam_processos] = -1;
        tam_processos++;
    }

    printf("debug 1: %d\n", sched_getcpu());
   imprime(processos, tam_processos);


   imprime(pronto, tam_prontos);

    play[0] = 1;
    int * ide;
    ide = malloc(sizeof(int));
    (*ide) = 0;
    if(pthread_create(&tid[0], NULL, Thread, (void*)ide)){
        printf("ERRO ao criar a Thread\n");
        exit(1);
    }
    printf("debug 2: %d\n", sched_getcpu());

    /*sleep(5);
    pause_thread(0);
    sleep(5);
    play_thread(0);*/

    if(pthread_join(tid[0], NULL)){
        printf("ERRO ao dar join na Thread\n");
        exit(1);
    }
    printf("debug 4: %d\n", sched_getcpu());

   return 0;
}