PGM = g2w 
CC  = gcc 
CFLAGS  = 
RHESSYS_BIN = /usr/local/bin

OBJECTS = grass2world.o sys.o unit.o 

LIBES = -lm

$(PGM): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBES) $(CFLAGS) -o $(PGM) 

grass2world.o:
	$(CC) $(CFLAGS) -c grass2world.c
sys.o:
	$(CC) $(CFLAGS) -c sys.c
unit.o:
	$(CC) $(CFLAGS) -c unit.c

clean:
	rm $(OBJECTS)

clobber:
	rm $(PGM) $(OBJECTS)

install: $(PGM)
	cp $(PGM) $(RHESSYS_BIN)	

