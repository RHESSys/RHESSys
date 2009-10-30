PGM = rat
CC  = gcc 
RHESSYS_BIN = /usr/local/bin
GISLIB = -lgrass_gis

OBJECTS = \
main.o \
log.o

$(PGM): $(OBJECTS)
	$(CC) $(OBJECTS) -L$(GISBASE)/lib -o $(PGM) $(GISLIB) $(XDRLIB) -lm

main.o:
	$(CC) -I$(GISBASE)/include -c main.c -o main.o
log.o:

install:
	cp $(PGM) $(RHESSYS_BIN)

clean:
	rm -f $(OBJECTS)

clobber:
	rm -f $(OBJECTS) $(PGM)

install:
	cp $(PGM) $(RHESSYS_BIN)
