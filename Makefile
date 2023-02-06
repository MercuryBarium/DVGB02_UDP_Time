CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -D_POSIX_SOURCE -D_GNU_SOURCE
LFLAGS = -lm
HDR = $(wildcard *.h)
SRC = $(filter-out (server.c), (client.c)) 
OBJ = $(SRC:.c=.o)

%.o