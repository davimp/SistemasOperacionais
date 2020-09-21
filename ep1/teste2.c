#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <signal.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

void type_prompt();
void read_command(char command[200], char parameters[200][200]);

int main(int agrc, char* argv[])
{
	int status;
	char command[200];
	char parameters[200][200];
	while(1){
		type_prompt();
		read_command(command, parameters); //lê o comando digitado
		printf("olha so isso, alguma coisa esta errada?: %s %s", command, parameters[0]);
		/* roda o comando se for uma das chamadas se sistema descritas no ep */
		if(strcmp(command, "mkdir") == 0){
			/*mkdir <diretorio>*/
			mkdir(parameters[0], 775);
		}
		else if(strcmp(command, "kill") == 0){
			/*kill -9 <PID>*/
			/*int kill(pid_t pid, int sig);*/
			kill(parameters[1], 9);
		}
		else if(strcmp(command, "ln") == 0){
			/*ln -s <arquivo> <link>*/
			/*int link(const char *oldpath, const char *newpath); esse cria um link forte
			Queremos um link fraco, por conta do "-s"*/
			/*int linkat(int olddirfd, const char *oldpath,
                  int newdirfd, const char *newpath, int flags);*/
				/*linkat parece dar pra criar um link fraco mas n seu como*/
		}	
		/* senão cria um processo com o fork e em seguida chama o execve */
		else if (fork() != 0) {
			/* Codigo do pai */
			/* O processo pai só precisa esperar o filho acabar*/
			waitpid(-1, &status,0);
		}
		else {
			/* Codigo do filho */

			char *teste[] = {"/usr/bin/du", "-hs",".",NULL};
			execve("/usr/bin/du", teste, 0);
			
		}
	}
	return 0;
}

void type_prompt()
{
	printf("{%s@%s", getenv("USER"), getenv("PWD"));
}

void read_command(char command[200], char parameters[200][200])
{
	command = readline("} ");
	add_history (command);
	int i = 0, j = 0, k = 0;
	while(command[i] != ' ' && command[i] != '\0') i++;

	if(command[i] != '\0')
	{
		command[i] = '\0';
		i++;
		j = 0;
		while(command[i+j] != '\0')
		{
			parameters[k][j] = command[i+j];
			j++;
		}
		/*printf("DEBUG\n");*/
		parameters[k][j] = '\0';
		k++;
	}
	printf("Comando: %s\nParametros: %s\n", command, parameters[0]);
}
