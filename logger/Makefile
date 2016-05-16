SOURCES = $(wildcard *.c)
EXE = logger.bin
GCC = gcc

GCCFLAGS = -Wall
GCCEXTRAS = -lpthread -ldl

all:
	$(GCC) $(GCCFLAGS) $(GCCEXTRAS) $(SOURCES) -o $(EXE)

clean:
	rm -rf $(EXE)
	rm -rf "logs.txt"

.PHONY: clean 
