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
   kill(1241, 9);
}
