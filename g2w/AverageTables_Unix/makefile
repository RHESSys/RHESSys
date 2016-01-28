PGM = rat
CC  = gcc 
RHESSYS_BIN = /usr/local/bin
GISLIB = -lgrass_gis
CFLAGS = -g
OBJECTS = \
main.o \
log.o

OS := $(shell uname)

$(PGM): $(OBJECTS)
ifeq ($(OS), Darwin)
	$(CC) $(CFLAGS) $(OBJECTS) -L$(GISBASE)/lib -o $(PGM) $(GISLIB) $(XDRLIB) -lm
else ifeq ($(OS), Linux)
	$(CC) $(CFLAGS) $(OBJECTS) -L$(GISBASE)/lib -Wl,-rpath=$(GISBASE)/lib -o $(PGM) $(GISLIB) $(XDRLIB) -lm
else
	$(CC) $(CFLAGS) $(OBJECTS) -L$(GISBASE)/lib -Wl,-rpath=$(GISBASE)/lib -o $(PGM) $(GISLIB) $(XDRLIB) -lm
endif

main.o:
	$(CC) $(CFLAGS) -I$(GISBASE)/include -c main.c -o main.o
log.o:

install:
	cp $(PGM) $(RHESSYS_BIN)

clean:
	rm -f $(OBJECTS)

clobber:
	rm -f $(OBJECTS) $(PGM)

install:
	cp $(PGM) $(RHESSYS_BIN)
