CC = gcc
CCFLAGS = -g -O0 -lreadline

teste2: teste2.o
	$(CC)  teste2.o -o bccsh -lreadline
teste2.o: teste2.c #provisorio, mudar nome de teste2 pra bccsh futuramente
	$(CC)  teste2.c -g -O0 -c -lreadline

