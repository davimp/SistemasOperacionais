CC = gcc
CCFLAGS = #-Wall #-ansi -g #-pedantic #-Wno-unused-result
OBJS_BCCSH = bccsh.o
OBJS_SIM = ep1.o fcfs.o srtn.o rr.o
RM = rm
#----------------------------------------
binarios: bccsh ep1

bccsh: $(OBJS_BCCSH)
	$(CC)  $(OBJS_BCCSH) -o bccsh -lreadline -pthread

bccsh.o: bccsh.c 
	$(CC)  $(CCFLAGS) -c bccsh.c -lreadline -pthread

ep1: $(OBJS_SIM) 
	$(CC) $(OBJS_SIM) -o ep1 -pthread  

ep1.o: ep1.c fcfs.h srtn.h rr.h
	$(CC) $(CCFLAGS) -c ep1.c -pthread  

fcfs.o: fcfs.c fcfs.h
	$(CC) $(CCFLAGS) -c fcfs.c -pthread  

srtn.o: srtn.c srtn.h
	$(CC) $(CCFLAGS) -c srtn.c -pthread

rr.o: rr.c rr.h
	$(CC) $(CCFLAGS) -c rr.c -pthread

clean:
	$(RM) *.o bccsh ep1