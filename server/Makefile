SOURCES_NP = $(wildcard ../libraries/libnamedpipe/objects/*.c)
SOURCES_S = $(wildcard ../libraries/libsocket/objects/*.c)
SOURCES_G = $(wildcard ../libraries/libgeneral/objects/*.c)
SOURCES_EXE = $(wildcard src/*.c)
SOURCES_DB = $(wildcard src/sqlite3/*.c)


ifeq  ($(imp), sockets)
	SOURCES_C = $(SOURCES_S)
else
	SOURCES_C = $(SOURCES_NP)
endif

SOURCES= $(SOURCES_C) $(SOURCES_G) $(SOURCES_EXE) $(SOURCES_DB)
EXE = server.bin
GCC = gcc
GCCFLAGS = -Wall -I./../libraries/includes -I./src 
GCCEXTRAS = -pthread -ldl
MAIN_C = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: $(EXE)

$(EXE): $(OBJECTS)
	$(GCC) $(GCCFLAGS) $(GCCEXTRAS) $(MAIN_C) -o $(EXE) $(OBJECTS)

%.o: %.c
	$(GCC) $(GCCFLAGS) $(GCCEXTRAS) -c $< -o $@

clean:
	rm -rf $(OBJECTS)
	rm -rf $(EXE)
	rm -rf /tmp/*_w
	rm -rf /tmp/*_r
	rm -rf ../libraries/libsocket/objects/*.o

.PHONY: clean
