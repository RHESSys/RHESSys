PGM = rat
CC  = gcc 
GISLIB = -lgrass_gis
RHESSYS_BIN = /usr/local/bin

OBJECTS = \
main.o \
log.o

$(PGM): $(OBJECTS)
	$(CC) $(OBJECTS) -I$(GISBASE)/include -L$(GISBASE)/lib -o $(PGM) $(GISLIB) $(XDRLIB) -lm

main.o: main.c
	$(CC) -c main.c -I$(GISBASE)/include -o main.o
log.o: log.c
	$(CC) -c log.c -o log.o

clean:
	rm -f $(OBJECTS)

clobber:
	rm -f $(OBJECTS) $(PGM)

install:
	cp $(PGM) $(RHESSYS_BIN)
