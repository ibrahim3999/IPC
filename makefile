CC = gcc
CFLAGS = -std=c99 -Wall
LDFLAGS =

SERVER_SRCS = src/server.c
CLIENT_SRCS = src/client.c
STNC_SRCS = stnc.c

SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
STNC_OBJ = $(STNC_SRCS:.c=.o)

STNC_EXEC = stnc

.PHONY: all clean

all: $(STNC_EXEC)

$(STNC_EXEC): $(SERVER_OBJS) $(CLIENT_OBJS) $(STNC_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(SERVER_OBJS): $(SERVER_SRCS)
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJS): $(CLIENT_SRCS)
	$(CC) $(CFLAGS) -c $< -o $@

$(STNC_OBJ): $(STNC_SRCS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(STNC_OBJ) $(STNC_EXEC)
