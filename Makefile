CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -g -O0
LFLAGS = -lasan -lSDL2 -lSDL2_image

SOURCES = ./src/main.c ./src/data.c

OUTPUT_NAME = mammut

all:
	$(CC) $(CFLAGS) $(LFLAGS) $(SOURCES) -o $(OUTPUT_NAME)
