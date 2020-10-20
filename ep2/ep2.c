#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAXN 1000

/* parametros de cada  */
typedef struct 
{
    int tempo; //1 a cada 20ms
    int volta; //volta
    int dist; //distancia percorrido
    int velocidade; // 1 2 ou 3, 1 a cada 30km/h
} Parametros;

int pista[10][MAXN]; /* pista tem tamanho d */
int tempo;
pthread_t ciclista[MAXN]; /*devem ser criados n threads para os ciclistas*/
Parametros parametros[MAXN];
_Atomic int processados;
int prox_volta;

void inicia_thread();
//cicilista.tempo = 0;


/* a thread receberá como argumento o id do ciclista?*/
void * Thread(void * a) {
    int* arg = a;
    int id = (*arg);

    /*free(arg);*/ 
    while(1)
    {
        /* verifica se nesse intervalo de tempo já executou as ações */
        if(parametros[id].tempo > tempo)
            continue; /*continue*/ 


        /*  */
        parametros[id].velocidade = 1;//30km/h

        /* verifica se pode ultrapassar */

        /* Andar pela pista */
        //P(protege_pista)

        //V(protege_pista)

        // 3 5 7 9 11
        
        //proteger
        /* se chegou  */
        // if(prox_volta == parametros[id].volta)
        // {
        //     //flag 
        //     prox_volta += 2;
        // }


        //P()
            
        //

        /* proteger */
        processados++;
        parametros[id].tempo++;
    }

    
    return NULL;
}

int main(int argc, char* argv[]){ 
    int d, n, faixa, metro, aux;
    d = atoi(argv[1]);
    n = atoi(argv[2]);


    srand(8080);



    /*inicia todo mundo na pista*/
    aux = 0;
    for(metro = 0; metro < d; metro++){
        for(faixa = 0; faixa < 5; faixa++){
            pista[faixa][metro] = aux++; 
        }
    }

    while(1)
    {

        /*while(processados != n_atual) continue;*/

        /*se a volta for impar*/
        /*se flag_eliminar entao elimina*/ 
        /*V()*/

        tempo++;
    }


    return 0;
}