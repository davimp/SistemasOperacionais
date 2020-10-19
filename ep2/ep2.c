#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAXN 1000

void * Thread(void * a) {
   
   return NULL;
}

int main(int argc, char* argv[]){ 
    int d, n;
    d = atoi(argv[1]);
    n = atoi(argv[2]);
    int pista[10][MAXN]; /*pista tem tamanho d*/
    pthread_t ciclista[MAXN]; /*devem ser criados n threads para os ciclistas*/


    return 0;
}