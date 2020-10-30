#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAXN 10005  /* tamanho maximo de d*/

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
    int eliminado; //1 se ja foi eliminado ou quebrou 0 caso contrário
} Parametros;

int d, n;
int pista[15][MAXN]; /* pista tem tamanho d */
int tempo, intervalo;
pthread_t ciclista[MAXN]; /*devem ser criados n threads para os ciclistas*/
Parametros parametros[MAXN];
_Atomic int processados;
_Atomic int terminados;
_Atomic int n_atual;
_Atomic int quebrados;
_Atomic int volta[MAXN]; // volta[i] representa quantos ciclistas já realizaram a volta i até o momento
_Atomic int colocacao[MAXN][MAXN]; //colocacao[i][j] vale o id do (j+1)º ciclista que passou pela volta i
int prox_volta;
pthread_mutex_t mutex_n;
pthread_mutex_t mutex_volta[MAXN];
pthread_mutex_t mutex[15][MAXN];
pthread_barrier_t barreira;
int posicao = 1;



/* a thread receberá como argumento o id do ciclista*/
void * Thread(void * a){
    int i, j, aux, aux2, aux3;
    int* arg = a;
    int id = (*arg);
    int ok = 1;
    Parametros *self;
    self = parametros + id;
    free(arg);

    fprintf(stderr, "id: %d\n iniciou\n", id);

    while(1)
    {
        /* verifica se nesse intervalo de tempo já executou as ações */
        /* caso 1: ja foi processado nesse periodo */
        //fprintf(stderr, "id --> %d n_atual --> %d processados --> %d \n", id,  n_atual, processados);
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
            fprintf(stderr, "ANTES %d %d %d\n", id, self->faixa, (self->posicao+1)%d);
            fprintf(stderr, "outro loop aux2: %d faixa: %d \n", aux2, self->faixa);

            pthread_mutex_lock(&mutex[self->faixa][(self->posicao+1)%d]);
            aux = pista[self->faixa][(self->posicao+1)%d];
            i = self->faixa + 1;
            fprintf(stderr, "aux: %d\n", aux);
            if(self->faixa > 0 && aux && parametros[aux].tempo == tempo && parametros[aux].velocidade >= self->velocidade)
            {
                pthread_mutex_lock(&mutex[self->faixa-1][(self->posicao+1)%d]);
                aux = pista[self->faixa - 1][(self->posicao+1)%d];

                if(aux == 0)
                {   
                    i = self->faixa;
                    pista[self->faixa - 1][(self->posicao+1)%d] = id;

                    pthread_mutex_lock(&mutex[self->faixa][self->posicao]); 

                    pista[self->faixa][self->posicao] = 0;
                    pthread_mutex_unlock(&mutex[self->faixa][self->posicao]);
                }
                pthread_mutex_unlock(&mutex[self->faixa-1][(self->posicao+1)%d]);
            }
            else
                aux = -1;
            
            pthread_mutex_unlock(&mutex[self->faixa][(self->posicao+1)%d]);
            /*fprintf(stderr, "DEPOIS %d %d %d\n", id, self->faixa, self->posicao);*/
            aux2 = 0;
            if(aux != 0){
            for(i = self->faixa; i < 10 && aux != 0; i++)
            {   
                pthread_mutex_lock(&mutex[i][(self->posicao+1)%d]);
                aux = pista[i][(self->posicao+1)%d];

                if(aux == 0)
                {
                    pthread_mutex_lock(&mutex[self->faixa][self->posicao]); 
                    pista[i][(self->posicao+1)%d] = id;
                    
                    pista[self->faixa][self->posicao] = 0;
                    pthread_mutex_unlock(&mutex[self->faixa][self->posicao]);
                }
                else if(parametros[aux].tempo > tempo)
                    aux2++;
                pthread_mutex_unlock(&mutex[i][(self->posicao+1)%d]);
            }}
            i--;
         
            if(aux2 == 10 - self->faixa)
                aux = 0;
        }
            fprintf(stderr, "PASSOU DAQUI %d\n", id);

        /* caso 3: se ele não pode ultrapassar/avançar */
        if(aux2 == 10 - self->faixa)
        {
            self->tempo += intervalo;
            self->trecho = 100;
            //self->velocidade = 30;
            processados += 1;
            //pthread_barrier_wait(&barreira);
            fprintf(stderr, "------------DEU RUIM------------ aux2: %d faixa: %d\n", aux2, self->faixa);
            continue;
        }
        fprintf(stderr, "id: %d posicao: %d volta: %d\n", id, self->posicao, self->volta);
        self->faixa = i;

        /* se e está no último metro da pista */
        if(self->posicao == d-1)
        {
            if(id != 1)
            {
            if(self->velocidade == 30)
                self->velocidade = 30 + 30*((int) (rand()%10 >= 2));
            else if(self->velocidade == 60)
                self->velocidade = 30 + 30*((int) (rand()%10 >= 4));
            //self->velocidade = 30 + 30*(id%2);//30km/h ou 60km/h
            }
            else
                self->velocidade = 5;
            self->volta++;

            if(self->volta < 3*n + 9)
            {
                j = 0;
                pthread_mutex_lock(&mutex_volta[self->volta]);//P()
                while(colocacao[self->volta][j] != 0) j++;
                colocacao[self->volta][j] = id;
                pthread_mutex_unlock(&mutex_volta[self->volta]);//V()
            }


            //se tem um cara q ta na volta 10 e eh eliminado na volta 4, mas na volta 6 ele tava na frente de um cara
            //o entao o ultimo da volta 6 seria ele e o depois do ultimo

            /* checa se foi eliminado */
            ok = 1;
            aux3 = 0;
            pthread_mutex_lock(&mutex_n);
            pthread_mutex_lock(&mutex_volta[self->volta]);
                if(id == 1)
                {
                        FILE *fff;
                        fff = fopen("olha2.out", "w");
                        fprintf(fff, "%d\n",  2*(n - n_atual + quebrados + 1));
                        fclose(fff);
                }
            if(self->volta >= 2*(n - n_atual + quebrados + 1))/* && colocacao[2*(n - n_atual + quebrados + 1)][n_atual-1] == id*/
            {
                aux3 = 0;
                j = 0;
                
                while(aux3 < n_atual && colocacao[2*(n - n_atual + quebrados + 1)][j] != 0)
                {
                    if(!parametros[colocacao[2*(n - n_atual + quebrados + 1)][j]].eliminado)
                        aux3++;
                    j++;
                }
                if(id == 1)
                {
                        FILE *fff;
                        fff = fopen("olha1.out", "w");
                        fprintf(fff, "%d\n", aux3);
                        fclose(fff);
                }
            }

                if(id == 1)
                {
                        FILE *fff;
                        fff = fopen("olha3.out", "w");
                        fprintf(fff, "%d\n", n_atual);
                        fclose(fff);
                }
            if(aux3 == n_atual)
            {
              ok = 0;
              fprintf(stderr, "------------Eliminado------------\n");
              fprintf(stderr, "----Posição: %d Ciclista: %d ----\n", n_atual, id);
              pthread_mutex_lock(&mutex[self->faixa][0]);
              pista[self->faixa][0] = 0;
              pthread_mutex_unlock(&mutex[self->faixa][0]);
              processados += 1;
              n_atual -= 1;

              /*break;*/
            }
            pthread_mutex_unlock(&mutex_volta[self->volta]);

            /* checa se quebrou */
            /*pthread_mutex_lock(&mutex_n);*/
            if(self->volta%6==0 && n_atual > 5 && ok && (rand()%100 >= 95)){
              ok = 0;
              quebrados++;
              fprintf(stderr, "------------Quebrou------------\n");
              fprintf(stderr, "----Posição: %d Ciclista: %d ----\n", n_atual, id);
              pthread_mutex_lock(&mutex[self->faixa][0]);
              pista[self->faixa][0] = 0;
              pthread_mutex_unlock(&mutex[self->faixa][0]);
              processados+=1;
              n_atual -= 1;
              //break;
            }
            pthread_mutex_unlock(&mutex_n);

            if(!ok) break;
        }

        pthread_mutex_lock(&mutex[self->faixa][self->posicao]);
        pista[self->faixa][self->posicao] = 0;
        pthread_mutex_unlock(&mutex[self->faixa][self->posicao]);

        self->posicao++;
        self->posicao = self->posicao % d;
        pthread_mutex_lock(&mutex[i][self->posicao]);
        pista[i][self->posicao] = id;
        pthread_mutex_unlock(&mutex[i][self->posicao]);

        processados += 1;
      //  n_atual -= 1;
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
    self->eliminado = 1;
    
    /*n_atual -= 1;*/

    fprintf(stderr, "terminados agora: %d \n", terminados);
    
    return NULL;
}

void inicia_thread(int id){
    parametros[id].tempo = 0;
    parametros[id].volta = 0;
    parametros[id].dist  = 0;
    parametros[id].velocidade = 30;
    if(id == 1)
        parametros[id].velocidade = 5;
    parametros[id].trecho = 0;
    parametros[id].eliminado = 0;
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
    {
        volta[i] = 0;
        for(j = 0; j <= n; j++)
            colocacao[i][j] = 0;
    }
    debug = fopen("corrida.dat", "w");

    d = atoi(argv[1]);
    n = atoi(argv[2]);

    fprintf(debug, "%d %d\n", d, n);

    /* ------------- inicia as estruturas -----------*/
    n_atual = n;
    quebrados = 0;
    intervalo = 60; 
    terminados = 0;
    processados = 0;
    pthread_barrier_init(&barreira, NULL, n);
    pthread_mutex_init(&mutex_n, NULL);
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

    fprintf(stderr, "eita\n");
    

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
        fclose(debug);
        debug = fopen("corrida.dat", "a");

        processados = 0;
        tempo += intervalo; /* unidade de tempo = 20ms*/
        fprintf(stderr, "tempo: %d\n", tempo);
    }
    fprintf(stderr, "PASSOU\n");
    fclose(debug);

    /* ------------- finalizações das estruturas usadas ---------------- */
    for(aux = 1; aux <= n; aux++){
        if(pthread_join(ciclista[aux], NULL)){
                printf("ERRO ao dar join na Thread\n");
                exit(1);
        }
    }
    pthread_mutex_destroy(&mutex_n);
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