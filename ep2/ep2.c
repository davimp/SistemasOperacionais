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
    int tempo; //1 a cada 20ms ou 60ms
    int volta; //número de voltas completadas
    int dist; //distancia percorrido
    int velocidade; // 1 2 ou 3, 1 a cada 30km/h
    int posicao; //posicao na pista
    int trecho; // o quanto percorreu de um metro, em centimetros/3
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
_Atomic int prox_volta;
_Atomic int penultima_volta;
_Atomic int sorteia;
_Atomic int rapido;
pthread_mutex_t mutex_n;
pthread_mutex_t mutex_volta[MAXN];
pthread_mutex_t mutex[15][MAXN];
int posicao = 1;
FILE *debug;



/* a thread receberá como argumento o id do ciclista*/
void * Thread(void * a){
    int i, j, aux, aux2, aux3;
    int* arg = a;
    int id = (*arg);
    int ok = 1;
    Parametros *self;
    self = parametros + id;
    free(arg);

    while(1)
    {
        /* verifica se nesse intervalo de tempo já executou as ações */
        /* caso 1: ja foi processado nesse periodo */
        //fprintf()
        //fprintf(stderr, "processados --> %d \n", processados);
        if(self->tempo > tempo)
            continue; /*continue*/

        self->trecho += self->velocidade * intervalo / 12;

        //fprintf(stderr, "------> trecho: %d\n", self->trecho);
        /* caso 2: se ainda não avançou o suficiente para trocar de posição */
        if(self->trecho < 300)
        {
            self->tempo += intervalo;
            processados += 1;
            continue;
        }

        self->trecho = 0;


        aux = -1;
        while(aux != 0)
        {
            //fprintf(stderr, "ANTES %d %d %d\n", id, self->faixa, (self->posicao+1)%d);
            //fprintf(stderr, "outro loop aux2: %d faixa: %d \n", aux2, self->faixa);

            pthread_mutex_lock(&mutex[self->faixa][(self->posicao+1)%d]);
            aux = pista[self->faixa][(self->posicao+1)%d];
            i = self->faixa + 1;
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
            //fprintf(stderr, "PASSOU DAQUI %d\n", id);

        /* caso 3: se ele não pode ultrapassar/avançar */
        if(aux2 == 10 - self->faixa)
        {
            self->tempo += intervalo;
            self->trecho = 300;
            processados += 1;
            //fprintf(stderr, "------------DEU RUIM------------ aux2: %d faixa: %d\n", aux2, self->faixa);
            continue;
        }
        //fprintf(stderr, "id: %d posicao: %d volta: %d\n", id, self->posicao, self->volta);
        self->faixa = i;

        /* se e está no último metro da pista, ou seja, mudara de volta */
        if(self->posicao == d-1)
        {
            self->volta++;
            
            if(self->velocidade == 30)
                self->velocidade = 30 + 30*((int) (rand()%10 >= 2));
            else if(self->velocidade == 60)
                self->velocidade = 30 + 30*((int) (rand()%10 >= 4));


            if(self->volta < 3*n + 9)
            {
                j = 0;
                pthread_mutex_lock(&mutex_volta[self->volta]);
                while(colocacao[self->volta][j] != 0) j++;
                colocacao[self->volta][j] = id;
                pthread_mutex_unlock(&mutex_volta[self->volta]);
            }

            /* checa se foi eliminado */
            ok = 1;
            aux3 = 0;
            pthread_mutex_lock(&mutex_n);
            pthread_mutex_lock(&mutex_volta[self->volta]);
            if(self->volta >= prox_volta)
            {
                aux3 = 0;
                j = 0;
                
                while(aux3 < n_atual && colocacao[prox_volta][j] != 0)
                {
                    if(!parametros[colocacao[prox_volta][j]].eliminado)
                        aux3++;
                    j++;
                }
            }
            if(aux3 == n_atual)
            {
              ok = 0;
              fprintf(stderr, "------------Eliminado------------\n");
              fprintf(stderr, "----Posição: %d Ciclista: %d ----\n", n_atual, id);
              pthread_mutex_lock(&mutex[self->faixa][0]);
              pista[self->faixa][0] = 0;
              pthread_mutex_unlock(&mutex[self->faixa][0]);
              prox_volta += 2;
              processados += 1;
              n_atual -= 1;
            }
            pthread_mutex_unlock(&mutex_volta[self->volta]);

            /* checa se quebrou */
            if(self->volta%6==0 && n_atual > 5 && ok && (rand()%100 >= 95)){
              ok = 0;
              quebrados++;
              fprintf(stderr, "------------Quebrou------------\n");
              fprintf(stderr, "----Posição: %d Ciclista: %d Tempo: %d ----\n", n_atual, id, tempo);
              pthread_mutex_lock(&mutex[self->faixa][0]);
              pista[self->faixa][0] = 0;
              pthread_mutex_unlock(&mutex[self->faixa][0]);
              processados+=1;
              n_atual -= 1;
            }
            
            if(!ok && n_atual == 5)
            {
                if(self->volta%2 ==0)
                    penultima_volta = self->volta + 3*2+1;
                else 
                    penultima_volta = self->volta + 3*2;

                if(rand()%10 == 0)
                {
                    sorteia = rand()%2 + 1;
                }
            }
            pthread_mutex_unlock(&mutex_n);

            if(!ok)
                break;
            
        }
        
        /* sorteio dos 90km/h */
        if(penultima_volta != -1 && self->volta >= penultima_volta 
            && (colocacao[penultima_volta][0] == 0 && sorteia == 1 
            ||(colocacao[penultima_volta][0] != 0 && colocacao[penultima_volta][1] == 0
                && sorteia == 2)) )
        {
            sorteia = 0;
            rapido = 1;
            self->velocidade = 90;
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
        self->tempo += intervalo;
    }

    fprintf(stderr, "id: %d\n terminou no tempo %d\n", id, tempo);
    pthread_mutex_lock(&mutex[self->faixa][self->posicao]);
    pista[self->faixa][self->posicao] = 0;
    pthread_mutex_unlock(&mutex[self->faixa][self->posicao]);
    self->eliminado = 1;
    terminados += 1;

    return NULL;
}

void inicia_thread(int id){
    parametros[id].tempo = 0;
    parametros[id].volta = 0;
    parametros[id].dist  = 0;
    parametros[id].velocidade = 30;
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

void debuga(){
    int i, j;
    fprintf(debug, "%d\n", tempo);
    for(j = 0; j < 10; j++)
        for(i = 0; i < d; i++)
            fprintf(debug, "%d ", pista[j][i]);
    fprintf(debug, "\n");
    fclose(debug);
    debug = fopen("corrida.dat", "a");
}

int main(int argc, char* argv[]){
    int i, j, contador;
    int faixa, metro, aux;
    d = atoi(argv[1]);
    n = atoi(argv[2]);
    
    for(i = 0; i <= 3*n + 10; i++)
    {
        volta[i] = 0;
        for(j = 0; j <= n; j++)
            colocacao[i][j] = 0;
    }
    debug = fopen("corrida.dat", "w");


    fprintf(debug, "%d %d\n", d, n);

    /* ------------- inicia as estruturas -----------*/
    n_atual = n;
    rapido = 0;
    prox_volta = 2;
    penultima_volta = -1;
    quebrados = 0;
    intervalo = 60;
    terminados = 0;
    processados = 0;
    pthread_mutex_init(&mutex_n, NULL);
    sorteia = 0;
    srand(8080);

    for(i = 0; i < 10;i++){
        for(j = 0; j < d; j++){
            pthread_mutex_init(&mutex[i][j], NULL);
        }
    }

    for(i = 0; i < MAXN; i++)
        pthread_mutex_init(&mutex_volta[i], NULL);


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

    debuga();
    while(terminados < n)
    {
        /*fprintf(stderr, "TEMPO AGORA: %d\n", tempo);*/
        /*processa todo mundo*/
        while(n_atual && processados < n_atual && terminados < n) 
        {
            continue;
        }
        //fprintf(stderr, "\n preocessados: %d\n", processados);
       
        debuga();
        processados = 0;
        if(intervalo == 60 && rapido) 
        {
            intervalo = 20;
            tempo += 60;
        }
        else
            tempo += intervalo; 

        //fprintf(stderr, "tempo: %d\n", tempo);
    }
    //fprintf(stderr, "PASSOU\n");
    fclose(debug);

    /*-------------- teste para a saida pedida no enunciado -------------*/
    i = j = 1;
    while(colocacao[i][0] != 0){
        j = 0;
        fprintf(stderr, "volta: %d\n", i);
        while(colocacao[i][j] != 0){
            fprintf(stderr, "%dº colocado: %d\n", j+1, colocacao[i][j]);
            j++;
        }
        fprintf(stderr, "\n"); 
        i++;
    }
    fprintf(stderr, "penultima volta: %d\n", penultima_volta);
    /* ------------- finalizações das estruturas usadas ---------------- */
    for(aux = 1; aux <= n; aux++){
        if(pthread_join(ciclista[aux], NULL)){
                printf("ERRO ao dar join na Thread\n");
                exit(1);
        }
    }
    pthread_mutex_destroy(&mutex_n);
    for(i = 0; i < 10;i++){
        for(j = 0; j < d; j++){
            pthread_mutex_destroy(&mutex[i][j]);
        }
    }
    for(i = 0; i < MAXN; i++)
        pthread_mutex_destroy(&mutex_volta[i]);

    return 0;
}