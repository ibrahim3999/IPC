CC = gcc
CFLAGS = -Wall -Wextra -pedantic

all: stnc 

stnc: object/stnc.o object/server.o object/client.o 
	$(CC) -o bin/stnc object/stnc.o object/server.o object/client.o
	./bin/stnc   # execute the stnc command here


object/stnc.o: stnc.c 
	$(CC) $(CFLAGS) -c stnc.c -o object/stnc.o

object/server.o: src/server.c src/server.h
	$(CC) $(CFLAGS) -c src/server.c -o object/server.o

object/client.o: src/client.c src/client.h
	$(CC) $(CFLAGS) -c src/client.c -o object/client.o


clean:
	rm -f bin/stnc bin/server bin/client  object/*.o