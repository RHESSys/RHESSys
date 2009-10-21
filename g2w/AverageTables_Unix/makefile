PGM = rat
CC  = gcc 

OBJECTS = \
main.o \
log.o

$(PGM): $(OBJECTS)
	$(CC) $(OBJECTS) -I $(GISBASE)/include -o $(PGM) $(GISLIB) $(XDRLIB) -lm

main.o:
log.o:

clean:
	rm -f $(OBJECTS)

clobber:
	rm -f $(OBJECTS) $(PGM)
