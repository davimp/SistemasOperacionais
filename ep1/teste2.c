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
void read_command(char command[], char* parameters[]);

int main(int agrc, char* argv[])
{
	int i = 0;
	int status;
	char command[200];
	char *parameters[200];
	int quit = 0;
	pid_t pid;
	
	for(i = 0; i < 200; i++){
		parameters[i] = NULL;
	}
	
	while(1){
		type_prompt();
		/*printf("ponteiro antes: %x\n", command);*/
		read_command(command, parameters); //lê o comando digitado
		
		/*printf("ponteiro depois: %x\n", command);*/
		printf("DEBUG2: %s %s\n", command, parameters[0]);
	/*int i = 0;
	printf("Testando 2: ");
	while(command[i] != '\0')
	{
		printf("(%d|%c)", command[i], command[i]);
		i++;
	}
	puts("");*/
		/* roda o comando se for uma das chamadas se sistema descritas no ep */
		if(strcmp(command, "mkdir") == 0){
			/*mkdir <diretorio>*/
			mkdir(parameters[1], 775);
		}
		else if(strcmp(command, "kill") == 0){
			/*kill -9 <PID>*/
			/*int kill(pid_t pid, int sig);*/
			pid = atoi(parameters[2]);
			kill(pid, 9);
		}
		else if(strcmp(command, "ln") == 0){
			/*ln -s <arquivo> <link>*/
			/*int symlink(const char *target, const char *linkpath);*/
			symlink(parameters[2], parameters[3]);
		}	
		else if(strcmp(command, "quit") == 0){
			/*ln -s <arquivo> <link>*/
			/*int symlink(const char *target, const char *linkpath);*/
			/*symlink(,);*/
			quit = 1;
		}	
		/* senão cria um processo com o fork e em seguida chama o execve */
		else if (fork() != 0) {
			/* Codigo do pai */
			/* O processo pai só precisa esperar o filho acabar*/
			waitpid(-1, &status,0);
		}
		else {
			/* Codigo do filho */

			//char *teste[] = {"/usr/bin/du", "-hs", ".", NULL};
			//execve("/usr/bin/du", teste, 0);
			puts("Entrou aqui");
			execve(command, parameters, 0);

		}
		int ll = 1;
		for(i = 0; i < 200; i++)
		{
			if(parameters[i] != NULL) free(parameters[i]);
			else if(ll) 
			{
				printf("Quantos 2: %d\n", i);
				ll = 0;
			}
			parameters[i] = NULL;
		}

		if(quit)
		{
			rl_clear_history();
			return 0;
		}
	}
	return 0;
}

void type_prompt()
{
	printf("{%s@%s", getenv("USER"), getenv("PWD"));
}

void read_command(char command[], char* parameters[])
{
	int i = 0, j = 0, k = 0;
	int vazio = 1;
	char *string_temp;
	string_temp = readline("} "); // strtok(command, " ")
	strcpy(command, string_temp);

	free(string_temp);

	printf("===> %s\n", command);

	i = 0;

	while(command[i] != '\0' && vazio)
	{
		if(command[i] != ' ')
			vazio = 0;
		i++;
	}
	if(vazio)
	{
		command[0] = '\0';
		parameters[0] = NULL;
		puts("SAIU 2");
		return;
	}
	add_history(command);

	puts("LOOP");
	i = 0; j = 0; k = 0;
	/*command[0] = 'q';
	command[1] = 'u';
	command[2] = 'i';
	command[3] = 't';
	command[4] = '\0';
	return;*/
	while(command[i] != '\0')
	{
		while(command[i] == ' ') i++;

		if(command[i] == '\0') break;

		parameters[j] = malloc(200*sizeof(char));

		k = 0;
		while(command[i] != ' ' && command[i] != '\0')
			parameters[j][k++] = command[i++];
		parameters[j][k] = '\0';
		
		j++;

	}
	parameters[j] = NULL;

	printf("Quantos 1: %d\n", j);

	strcpy(command, parameters[0]);

	/*i = 0;
	printf("Testando: ");
	while(command[i] != '\0')
	{
		printf("(%d|%c)", command[i], command[i]);
		i++;
	}
	puts("");*/


	puts("SAIU");
	printf("Comando: %s\nParametros: %s\n", command, parameters[0]);


	printf("ponteiro durante: %x\n", command);

}
