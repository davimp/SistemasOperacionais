/* Baseado em código de 
 * http://www.csc.villanova.edu/~mdamian/threads/posixsem.html */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include <unistd.h>

#define NITER 1000000000
#define VEZES 5

void * Thread(void * a) {
   long int count;
   clock_t tempo_inicial, tempo_atual;
   int * arg = a;
   tempo_inicial = clock();
   tempo_atual = clock();
   count = -10000000;
   printf("DEBUG: %d\n", (*arg));
   while((tempo_atual - tempo_inicial)/CLOCKS_PER_SEC < (*arg)){
      count++;
      tempo_atual = clock();
   }
   return NULL;
}

int main() {
   /*int i;
   pthread_t tid;
   int * a;
   a = malloc(sizeof(int));
   (*a) = 10;
   if (pthread_create(&tid, NULL, Thread, a)){
         printf("\n ERROR creating thread 1");
         exit(1);
   }

   if (pthread_join(tid, NULL)){
      printf("\n ERROR joining thread");
      exit(1);
   }

   free(a);*/
   sleep(1);

   return 0;
}