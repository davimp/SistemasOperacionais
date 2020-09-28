CC = gcc
CCFLAGS = -Wall #-ansi -g #-pedantic #-Wno-unused-result
OBJS_BCCSH = bccsh.o
OBJS_SIM = simulador.o fcfs.o
RM = rm
#----------------------------------------
binarios: bccsh simulador

bccsh: $(OBJS_BCCSH)
	$(CC)  $(OBJS_BCCSH) -o bccsh -lreadline -pthread

bccsh.o: bccsh.c 
	$(CC)  $(CCFLAGS) -c bccsh.c -lreadline -pthread

simulador: $(OBJS_SIM) 
	$(CC) $(OBJS_SIM) -o simulador -pthread  

simulador.o: simulador.c fcfs.h
	$(CC) $(CCFLAGS) -c simulador.c -pthread  

fcfs.o: fcfs.c fcfs.h
	$(CC) $(CCFLAGS) -c fcfs.c -pthread  

clean:
	$(RM) *.o bccsh simulador