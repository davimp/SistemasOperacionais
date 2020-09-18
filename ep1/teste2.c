#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h> 
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
/*
(*)
O prompt do shell deve conter o nome do usuario, 
seguido de ‘@’ e do diretorio atual entre chave
se seguido de um espaco em branco, como no exemplo 
abaixo que mostra o shell pronto para rodar o comando kill:
{daniel@/tmp/mac0422/} kill -9 12345
*/

void type_prompt();
void read_command(char* command, char* parameters[]);

int main()
{
	int status;
	char command[200];
	char parameters[200][200];
	while(1){

		type_prompt();

		read_command(command, parameters); //lê o comando digitado

		/* roda o comando se for uma das chamadas se sistema descritas no ep */
		//if(strcmp((const char*)command, "/usr/bin/du") == 0)
		//{
			//"-hs ."
			//execv((const char*) command, (char * const*) parameters);
		/* The  exec()  family  of functions replaces the current process 
		image with a new process image.*/

		/* senão cria um processo; daniel disse que não precisava tratar os erros */
		if (fork() != 0) {
			/* Codigo do pai */
			/* processo pai eh o shell ??*/
			waitpid(-1, &status,0);
		} 
		else {
			/* Codigo do filho */

			char *teste[] = {"-hs",".",NULL};
			execve("/usr/bin/du", teste, 0);
		//}
			//execve(command,parameters,0);
		}
	}

}

void type_prompt()
{
	printf("{%s@%s", getenv("USER"), getenv("PWD"));
}

void read_command(char command[], char* parameters[])
{
	size_t tam = 32;
	//getline(&command, &tam, stdin);
	command = readline("} ");
	add_history (command);
	//scanf("%s", command);

	int i = 0, j = 0;
	while(command[i] != ' ' && command[i] != '\0') i++;
	if(command[i] != '\0')
	{
		command[i] = '\0';
		i++;
		j = 0;
		while(command[i+j] != '\0')
		{
			//parameters[j] = command[i+j];
			j++;
		}
		//parameters[j] = '\0';
	}
	else
		0;//parameters[0] = '\0';


	//printf("Comando: %s\nParametros: %s\n", command, parameters);
}