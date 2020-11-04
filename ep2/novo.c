#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAXN 1010  /* tamanho maximo de d*/

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
int volta_completa;
pthread_t ciclista[MAXN]; /*devem ser criados n threads para os ciclistas*/
Parametros parametros[MAXN];
_Atomic int processados;
_Atomic int terminados;
_Atomic int n_atual;
_Atomic int quebrados;
_Atomic int volta[MAXN]; // volta[i] representa quantos ciclistas já realizaram a volta i até o momento
_Atomic int quebraram[MAXN]; // guarda os que quebraram
_Atomic int completaram[MAXN]; // guarda os que completaram
_Atomic int finalizaram[MAXN];
_Atomic int colocacao[3*MAXN+20][MAXN]; //colocacao[i][j] vale o id do (j+1)º ciclista que passou pela volta i
_Atomic int prox_volta;
_Atomic int penultima_volta;
_Atomic int sorteia;
_Atomic int rapido;
_Atomic int imprime;
_Atomic int num_completados;
_Atomic int num_quebrados;
_Atomic int num_finalizados;
pthread_mutex_t mutex_n, lock, lock_main;
pthread_mutex_t mutex_volta[MAXN];
pthread_mutex_t mutex[15][MAXN];
pthread_cond_t cond, cond_main;
int posicao = 1;
FILE *debug;

void continua_main()
{
    /*while(n_atual && processados < n_atual && terminados < n) */
    if(processados >= n_atual)
    {   
        pthread_mutex_lock(&lock_main);
        pthread_cond_signal(&cond_main);
        pthread_mutex_unlock(&lock_main);   
    }
}

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
        pthread_mutex_lock(&lock);
        while(self->tempo > tempo)
            pthread_cond_wait(&cond, &lock);
        pthread_mutex_unlock(&lock);

        self->trecho += self->velocidade * intervalo / 12;

        /* caso 2: se ainda não avançou o suficiente para trocar de posição */
        if(self->trecho < 300)
        {
            self->tempo += intervalo;
            processados += 1;
            continua_main();
            continue;
        }

        self->trecho = 0;


        aux = -1;
        while(aux != 0)
        {
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

        /* caso 3: se ele não pode ultrapassar/avançar */
        if(aux2 == 10 - self->faixa)
        {
            self->tempo += intervalo;
            self->trecho = 300;
            processados += 1;
            continua_main();
            continue;
        }
        /* caso 4: avança */
        self->faixa = i;

        /* se ele está no último metro da pista, ou seja, mudará de volta */
        if(self->posicao == d-1)
        {
            self->volta++;
            
            if(self->velocidade == 30)
                self->velocidade = 30 + 30*((int) (rand()%10 >= 2));
            else if(self->velocidade == 60)
                self->velocidade = 30 + 30*((int) (rand()%10 >= 4));

            pthread_mutex_lock(&mutex_n);
            if(self->volta < 3*n + 9)
            {
                j = 0;
                pthread_mutex_lock(&mutex_volta[self->volta]);
                while(colocacao[self->volta][j] != 0) j++;
                colocacao[self->volta][j] = id;
                pthread_mutex_unlock(&mutex_volta[self->volta]);
            }
            imprime = 1;
            ok = 1;


            /* checa se foi eliminado */
            aux3 = 0;
            pthread_mutex_lock(&mutex_volta[self->volta]);
            /* se ja passou da volta em que havera a proxima eliminação
            fazemos um preprocessamento  para saber se ele foi o ultimo */

            if(ok && self->volta >= prox_volta)
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
            if(ok && aux3 == n_atual)  /* se foi o ultimo a passar naquela volta, eliminamos */
            {
              ok = 0;
              fprintf(stdout, "------------Eliminado------------\n");
              fprintf(stdout, "----Volta: %d Ciclista: %d Tempo: %d----\n", self->volta, id, tempo);
              pthread_mutex_lock(&mutex[self->faixa][0]);
              pista[self->faixa][0] = 0;
              pthread_mutex_unlock(&mutex[self->faixa][0]);
              prox_volta += 2;
              processados += 1;
              completaram[num_completados] = id;
              num_completados++;
              continua_main();
              n_atual -= 1;
            }
            pthread_mutex_unlock(&mutex_volta[self->volta]);


            /* checa se acabou a corrida */
            if(ok && penultima_volta != -1 && self->volta >= penultima_volta+1){
                ok = 0;
                fprintf(stdout, "------------Finalizou------------\n");
                fprintf(stdout, "------Tempo: %d Ciclista: %d ----\n", tempo, id);
                pthread_mutex_lock(&mutex[self->faixa][0]);
                pista[self->faixa][0] = 0;
                pthread_mutex_unlock(&mutex[self->faixa][0]);
                processados += 1;
                finalizaram[num_finalizados] = id;
                num_finalizados++;
                continua_main();
                n_atual -= 1;
            }

            /* checa se quebrou */
            if(self->volta%6==0 && n_atual > 5 && ok && (rand()%100 >= 95)){
              ok = 0;
              quebrados++;
              fprintf(stdout, "------------Quebrou------------\n");
              fprintf(stdout, "----Volta: %d Ciclista: %d Tempo: %d ----\n", self->volta+1, id, tempo);
              pthread_mutex_lock(&mutex[self->faixa][0]);
              pista[self->faixa][0] = 0;
              pthread_mutex_unlock(&mutex[self->faixa][0]);
              processados+=1;
              quebraram[num_quebrados] = id;
              num_quebrados++;
              continua_main();
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

            if(!ok) /* se quebrou ou foi eliminado, acabamos a execução da thread*/
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
        continua_main();

        self->tempo += intervalo;
    }
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


void debuga()
{
    int i, j;
    fprintf(stderr, "tempo: %d\n", tempo);

    fprintf(stderr, " ");
    for(i = 0; i < d; i++)
        fprintf(stderr, "_____");
    fprintf(stderr, "\n");

    for(j = 0; j < 10; j++)
    {
        fprintf(stderr, "|");
        for(i = 0; i < d; i++)
        {
            if(pista[j][i] != 0)
                fprintf(stderr, "\033[0;31m%4d\033[0m|", pista[j][i]);
            else
                fprintf(stderr, "   0|");
        }
        fprintf(stderr, "\n");
    }

    fprintf(stderr, " ");
    for(i = 0; i < d; i++)
        fprintf(stderr, "_____");
    fprintf(stderr, "\n");
}

void imprime_volta(){
    int aux, k, i;
    aux = -1;
    for(k = 1; k <= n; k++)
    {
        if(parametros[k].eliminado == 0 && (parametros[k].volta < aux || aux == -1))
            aux = parametros[k].volta;
        }

    for(i = volta_completa+1; i <= aux; i++)
    {
        fprintf(stdout, "---------volta: %d----------\n", i);
        for(k = 0; colocacao[i][k] != 0; k++)
        {
            fprintf(stdout, "%dº colocado: ciclista %d\n", k+1, colocacao[i][k]);
        }
        fprintf(stdout, "----------------------------\n");
    }
    if(aux != -1)
        volta_completa = aux;
}


int main(int argc, char* argv[]){
    int i, j, k, contador;
    int faixa, metro, aux, aux2;
    int modo_debug = 0;

    d = atoi(argv[1]);
    n = atoi(argv[2]);
    if(argc > 3) modo_debug = 1;
    /*fprintf(stdout, "teste: arg: %d modo: %d \n", argc, modo_debug);*/

    for(i = 0; i <= 3*n + 10; i++)
    {
        volta[i] = 0;
        for(j = 0; j <= n; j++)
            colocacao[i][j] = 0;
    }

    /* ------------- inicia as estruturas e variaveis -----------*/
    n_atual = n;
    num_completados = 0;
    num_quebrados = 0;
    num_finalizados = 0;
    imprime = 0;
    rapido = 0;
    volta_completa = 0;
    prox_volta = 2;
    penultima_volta = -1;
    quebrados = 0;
    tempo = 0;
    intervalo = 60;
    terminados = 0;
    processados = 0;
    sorteia = 0;
    pthread_mutex_init(&mutex_n, NULL);
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lock_main, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&cond_main, NULL);
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

    if(modo_debug) debuga();

    while(terminados < n)
    {
        /*processa todo mundo*/
        pthread_mutex_lock(&lock_main);
        while(n_atual && processados < n_atual && terminados < n) 
        {
            pthread_cond_wait(&cond_main, &lock_main);
        }
        processados = 0;
        pthread_mutex_unlock(&lock_main);

        if(modo_debug) debuga();

        /* imprime a saída de debug */
        if(imprime){
            imprime_volta();
        }
        imprime  = 0;

        if(intervalo == 60 && rapido) 
        {
            intervalo = 20;

            pthread_mutex_lock(&lock);
            tempo += 60;
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
        }
        else{
            pthread_mutex_lock(&lock);
            tempo += intervalo;
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
        } 
    }

    imprime_volta();
    

    /*----------------imprime as colocações finais-----------------------*/

    fprintf(stdout, "\n-----------Classificação final----------\n");
    for(i = num_finalizados-1; i >= 0; i--){
        aux = finalizaram[i];
        fprintf(stdout, "%dº colocado: ciclista %d (tempo em pista: %dms)\n", num_finalizados-i, aux, parametros[aux].tempo);
    }
    for(i = num_completados-1; i >= 0; i--){
        aux = completaram[i];
        fprintf(stdout, "%dº colocado: ciclista %d (tempo em pista: %dms)\n", num_completados-i+num_finalizados, aux, parametros[aux].tempo);
    }
    for(i = 0; i < num_quebrados; i++){
        aux = quebraram[i];
        fprintf(stdout, "quebrou: ciclista %d na volta %d\n", aux, parametros[aux].volta+1);
    }
    fprintf(stdout, "----------------------------------------\n");

    /* ------------- finalizações das estruturas usadas ---------------- */
    for(aux = 1; aux <= n; aux++){
        if(pthread_join(ciclista[aux], NULL)){
                printf("ERRO ao dar join na Thread\n");
                exit(1);
        }
    }
    pthread_mutex_destroy(&mutex_n);
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&lock_main);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&cond_main);

    for(i = 0; i < 10;i++){
        for(j = 0; j < d; j++){
            pthread_mutex_destroy(&mutex[i][j]);
        }
    }
    for(i = 0; i < MAXN; i++)
        pthread_mutex_destroy(&mutex_volta[i]);

    return 0;
}