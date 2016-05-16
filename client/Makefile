SOURCES_NP = $(wildcard ../libraries/libnamedpipe/objects/*.c)
SOURCES_S = $(wildcard ../libraries/libsocket/objects/*.c)
SOURCES_G = $(wildcard ../libraries/libgeneral/objects/*.c)
SOURCES_EXE = $(wildcard src/*.c)

ifeq  ($(imp), sockets)
	SOURCES_C = $(SOURCES_S)
else
	SOURCES_C = $(SOURCES_NP)
endif

SOURCES= $(SOURCES_C) $(SOURCES_G) $(SOURCES_EXE)
EXE = client.bin
GCC = gcc
GCCFLAGS = -Wall -I./../libraries/includes -I./src
GCCEXTRAS = -lpthread -ldl
MAIN_C = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: $(EXE)

$(EXE): $(OBJECTS)
	$(GCC) $(GCCFLAGS) $(MAIN_C) -o $(EXE) $(OBJECTS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJECTS)
	rm -rf $(EXE)

.PHONY: clean 
