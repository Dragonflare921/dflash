CC=gcc
DEFINES= -DDEBUG1
CFLAGS= -std=gnu99 -Wall $(DEFINES)
CLIBS= -lwiringPi


all: dflash.o MCP23S17.o
	$(CC) $(CFLAGS) $(CLIBS) dflash.o MCP23S17.o -o dflash

dflash.o: dflash.c dflash.h
	$(CC) $(CFLAGS) $(CLIBS) -c dflash.c -o dflash.o

MCP23S17.o: MCP23S17.c MCP23S17.h
	$(CC) $(CFLAGS) $(CLIBS) -c MCP23S17.c -o MCP23S17.o

.PHONY: run
.PHONY: clean

run: dflash
	./dflash -f test

clean:
	rm *.o dflash
