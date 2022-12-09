CC := gcc
STD := c99
BIN := bfci
SRC := src/interpreter.h src/interpreter.c
OBJ := interpreter.o

INSTALL_PATH := /usr/bin/

all: main

main:
	$(CC) -std=$(STD) -c -g $(SRC)
	$(CC) -std=$(STD) -o $(BIN) -g main.c $(OBJ)

.PHONY: install uninstall clean

install: main
	cp $(BIN) $(INSTALL_PATH)
	rm *.o *.out *~ $(BIN) src/*~ src/*.gch

uninstall:
	sudo rm $(INSTALL_PATH)/$(BIN)

clean:
	rm *.o *.out *~ $(BIN) src/*~ src/*.gch
