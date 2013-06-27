PGM = g2w 
CC  = gcc 
CFLAGS = -g
RHESSYS_BIN = /usr/local/bin

OBJECTS = main.o sys.o unit.o 

LIBES = -lm

$(PGM): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBES) $(CFLAGS) -o $(PGM) 

main.o:
	$(CC) $(CFLAGS) -c main.c
sys.o:
	$(CC) $(CFLAGS) -c sys.c
unit.o:
	$(CC) $(CFLAGS) -c unit.c

install:
	cp $(PGM) $(RHESSYS_BIN)

clean:
	rm $(OBJECTS)

clobber: clean
	rm $(PGM)

