CC = gcc
CFLAGS = -g -Wall -pthread
INCLUDE1=DieWithError.c HandleClientTCP.c CaesarCipher.c random_quote.c
INCLUDE2= Client.c

all: Server Application

Server: Server.c
	$(CC) $(CFLAGS) $(INCLUDE1) -o $@.o $<

Application: Application.c
	$(CC) $(CFLAGS) $(INCLUDE2) -o $@.o $<

clean:
	-rm -f *.o *~ *core* 
