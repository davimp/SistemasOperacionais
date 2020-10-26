#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAXN 100000  /* tamanho maximo de d*/

/* parametros de cada */
typedef struct 
{
    int tempo; //1 a cada 20ms
    int volta; //número de voltas completadas
    int dist; //distancia percorrido
    int velocidade; // 1 2 ou 3, 1 a cada 30km/h
    int posicao; //posicao na pista
    int trecho; // o quanto percorreu de um metro, em centimetros
    int faixa; //qual faixa o ciclista esta de 0 a 9
} Parametros;

int d, n;
int pista[10][MAXN]; /* pista tem tamanho d */
int tempo, intervalo;
pthread_t ciclista[MAXN]; /*devem ser criados n threads para os ciclistas*/
Parametros parametros[MAXN];
_Atomic int processados;
_Atomic int terminados;
_Atomic int n_atual;
_Atomic int volta[MAXN]; // volta[i] representa quantos ciclistas já realizaram a volta i até o momento
int prox_volta;
pthread_mutex_t mutex_pista;
pthread_mutex_t mutex_volta[MAXN];
pthread_mutex_t mutex[10][MAXN];
pthread_barrier_t barreira;
int posicao = 1;



/* a thread receberá como argumento o id do ciclista?*/
void * Thread(void * a){
    int i, j, aux, aux2;
    int* arg = a;
    int id = (*arg);
    Parametros *self;
    self = parametros + id;
    free(arg);

    fprintf(stderr, "id: %d\n iniciou\n", id);

    while(1)
    {
        /* verifica se nesse intervalo de tempo já executou as ações */
        /* caso 1: ja foi processado nesse periodo */
        if(self->tempo > tempo)
            continue; /*continue*/
        fprintf(stderr, "processados --> %d \n", processados);
        /*  */

        /* verifica se pode ultrapassar */

        /* Andar pela pista */

        self->trecho += self->velocidade * intervalo / 36;

        //fprintf(stderr, "id: %d posicao: %d trecho: %d\n", id, self->posicao, self->trecho);

        //fprintf(stderr, "trecho: %d\n", self->trecho);

        /* caso 2: se ainda não avançou o suficiente para trocar de posição */
        if(self->trecho < 100)
        {
            self->tempo += intervalo;
            processados += 1;
            //pthread_barrier_wait(&barreira);
            continue;
        }

        //self->trecho -= 100;
        self->trecho = 0;
        //fprintf(stderr, "passou trecho posicao: %d\n", self->posicao);


        aux = -1;
        while(aux != 0)
        {
            fprintf(stderr, "outro loop aux2: %d faixa: %d \n", aux2, self->faixa);
            //pthread_mutex_lock(&mutex_pista); //P(protege_pista) 

            aux2 = 0;
            for(i = self->faixa; i < 10 && aux != 0; i++)
            {   
                pthread_mutex_lock(&mutex[i][(self->posicao+1)%d]);
                aux = pista[i][(self->posicao+1)%d];

                if(aux == 0)
                {
                    pista[i][(self->posicao+1)%d] = id; 
                    pista[self->faixa][self->posicao] = 0;
                }
                else if(parametros[aux].tempo > tempo)
                    aux2++;
                pthread_mutex_unlock(&mutex[i][(self->posicao+1)%d]);
            }
            i--;
         
            if(aux2 == 10 - self->faixa)
                aux = 0;
            //pthread_mutex_unlock(&mutex_pista); //V(protege_pista)

        }

        /* caso 3: se ele não pode ultrapassar/avançar */
        if(aux2 == 10 - self->faixa)
        {
            self->tempo += intervalo;
            self->trecho = 100;
            //self->velocidade = 30;
            self->tempo += intervalo;
            processados += 1;
            //pthread_barrier_wait(&barreira);
            fprintf(stderr, "------------DEU RUIM------------ aux2: %d faixa: %d\n", aux2, self->faixa);
            continue;
        }
        fprintf(stderr, "id: %d posicao: %d volta: %d\n", id, self->posicao, self->volta);

        /* se e está no último metro da pista */
        if(self->posicao == d-1)
        {
            if(self->velocidade == 30)
                self->velocidade = 30 + 30*((int) (rand()%10 >= 2));
            else if(self->velocidade == 60)
                self->velocidade = 30 + 30*((int) (rand()%10 >= 4));
            //self->velocidade = 30 + 30*(id%2);//30km/h ou 60km/h
            self->volta++;

            //P()
            pthread_mutex_lock(&mutex_volta[self->volta]);
            if(++volta[self->volta] == n_atual && self->volta % 2 == 0 && self->volta >= 2)
            {
              fprintf(stderr, "------------Eliminado------------\n");
              fprintf(stderr, "----Posição: %d Ciclista: %d ----\n", n_atual, id);  
              processados += 1;
              break;
            }
            pthread_mutex_unlock(&mutex_volta[self->volta]);
            
              //fprintf(stderr, "======> volta[i] = %d\n", volta[self->volta]);
            //V()

            //fprintf(stderr, "------------VOLTA TERMINADA------------\n");
        }

        //pthread_mutex_lock(&mutex_pista);
        pthread_mutex_lock(&mutex[self->faixa][self->posicao]);
        pista[self->faixa][self->posicao] = 0;
        pthread_mutex_unlock(&mutex[self->faixa][self->posicao]);

        self->posicao++;
        self->posicao = self->posicao % d;
        pthread_mutex_lock(&mutex[i][self->posicao]);
        pista[i][self->posicao] = id;
        pthread_mutex_unlock(&mutex[i][self->posicao]);
        self->faixa = i;
        //pthread_mutex_unlock(&mutex_pista);

        processados += 1;
        self->tempo += intervalo;
        //pthread_barrier_wait(&barreira);
        /*if(self->volta == 6)
        {
            fprintf(stderr, "%d %d %d\n", posicao++, self->tempo, id);
            break;
        }*/

    }

    fprintf(stderr, "id: %d\n terminou\n", id);
    pthread_mutex_lock(&mutex[self->faixa][self->posicao]);
    pista[self->faixa][self->posicao] = 0;
    pthread_mutex_unlock(&mutex[self->faixa][self->posicao]);
    terminados += 1;
    n_atual -= 1;
    fprintf(stderr, "terminados agora: %d \n", terminados);
    
    return NULL;
}

void inicia_thread(int id){
    parametros[id].tempo = 0;
    parametros[id].volta = 0;
    parametros[id].dist  = 0;
    parametros[id].velocidade = 30;
    parametros[id].trecho = 0;
    int *argumento;
    argumento = malloc(sizeof(int));
    (*argumento) = id;
    if(pthread_create(&ciclista[id], NULL, Thread, (void*)argumento)){
        printf("ERRO ao criar a Thread\n");
        exit(1);
    }
}

int main(int argc, char* argv[]){
    int i, j;
    int faixa, metro, aux;
    FILE *debug;
    
    for(i = 0; i <= 3*n + 10; i++)
        volta[i] = 0;

    debug = fopen("corrida.dat", "w");

    d = atoi(argv[1]);
    n = atoi(argv[2]);

    fprintf(debug, "%d %d\n", d, n);

    /* ------------- inicia as estruturas -----------*/
    n_atual = n;

    intervalo = 60; 
    terminados = 0;
    processados = 0;
    pthread_barrier_init(&barreira, NULL, n);
    pthread_mutex_init(&mutex_pista, NULL);
    srand(8080);

    for(i = 0; i < 10;i++){
        for(j = 0; j < d; j++){
            pthread_mutex_init(&mutex[i][j], NULL);
        }
    }

    for(i = 0; i < MAXN; i++)
        pthread_mutex_init(&mutex_volta[i], NULL);

    //printf(stderr, "eita antes\n");

    /* ---------- inicia todo mundo na pista -----------*/
    aux = 1;
    for(metro = 0; metro < d && aux <= n; metro++){
        for(faixa = 0; faixa < 5 && aux <= n; faixa++){
            pista[faixa][metro] = aux;
            /*cria a thread*/
            parametros[aux].posicao = metro;
            parametros[aux].faixa = faixa;
            inicia_thread(aux);
            aux++;
        }
    }

    //fprintf(stderr, "eita\n");
    

    fprintf(debug, "%d\n", tempo);
    for(j = 0; j < 10; j++)
        for(i = 0; i < d; i++)
            fprintf(debug, "%d ", pista[j][i]);
    fprintf(debug, "\n");

    while(terminados < n)
    {
        fprintf(stderr, "\n terminados: %d\n", terminados);
        /*processa todo mundo*/
        while(n_atual && processados < n_atual && terminados < n) 
        {
            //fprintf(stderr, "loop proc: %d n_atual: %d\n", processados, n_atual);
            continue;
        }

        fprintf(stderr, "\n preocessados: %d\n", processados);
       
        fprintf(debug, "%d\n", tempo);
        for(j = 0; j < 10; j++)
            for(i = 0; i < d; i++)
                fprintf(debug, "%d ", pista[j][i]);
        fprintf(debug, "\n");

        tempo += intervalo; /* unidade de tempo = 20ms*/
        processados = 0;
        fprintf(stderr, "tempo: %d\n", tempo);
    }

    fprintf(stderr, "PASSOU\n");
    fclose(debug);

    /* ------------- finalizações das estruturas usadas ---------------- */
    for(aux = 0; aux < n; aux++){
        if(pthread_join(ciclista[aux], NULL)){
                printf("ERRO ao dar join na Thread\n");
                exit(1);
        }
    }
    pthread_mutex_destroy(&mutex_pista);
    pthread_barrier_destroy(&barreira);
    for(i = 0; i < 10;i++){
        for(j = 0; j < d; j++){
            pthread_mutex_destroy(&mutex[i][j]);
        }
    }
    for(i = 0; i < MAXN; i++)
        pthread_mutex_destroy(&mutex_volta[i]);

    return 0;
}