CC := gcc
STD := c99
BIN := bfci
SRC := src/

INSTALL_PATH := /usr/bin/

all: main

main: interpreter.o
	$(CC) -std=$(STD) -I $(SRC) -o $(BIN) -g main.c interpreter.o

.PHONY: interpreter.o install uninstall clean

interpreter.o:
	$(CC) -g -c $(SRC)/interpreter.h $(SRC)/interpreter.c

install: main
	cp $(BIN) $(INSTALL_PATH)
	rm *.o *.out *~ $(BIN) src/*~ src/*.gch

uninstall:
	sudo rm $(INSTALL_PATH)/$(BIN)

clean:
	rm *.o *.out *~ $(BIN) src/*~ src/*.gch
