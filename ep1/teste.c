#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//sys
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h> 
// Para o waitpid abaixo
#include <sys/wait.h>

int main (int argc, char **argv) {
   pid_t childpid;
   
   if ( (childpid = fork()) == 0) {
      printf("[Sou o processo filho]\n");
      while (1) {
         sleep(1);
         printf("Primeiro processo filho...\n");
      }
   }
   else {
      printf("[Sou o pai. Criei o %d]\n",childpid);
      //sleep(3);
      waitpid(-1, NULL, 0);
   }
   exit(0);
}
