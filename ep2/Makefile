CC = gcc
CCFLAGS = #-Wall #-ansi -g #-pedantic #-Wno-unused-result
OBJS = ep2.o
RM = rm
#----------------------------------------

ep2: $(OBJS) 
	$(CC) $(OBJS) -o ep2 -pthread  

ep2.o: ep2.c
	$(CC) $(CCFLAGS) -c ep2.c -pthread  

clean:
	$(RM) *.o ep2