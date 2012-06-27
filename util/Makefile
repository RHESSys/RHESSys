PGM = cxy
CC  = gcc 
CFLAGS  = -g -O0
#CFLAGS  =
RHESSYS_BIN = /usr/local/bin

OBJECTS = cxy.o 

LIBS = -lm
GIS_LIBS = -lgrass_gis

$(PGM): $(OBJECTS)
	$(CC) $(OBJECTS) -L$(GISBASE)/lib -L../cf $(GIS_LIBS) $(LIBS) $(CFLAGS) -o $(PGM) 

install: $(PGM) $(EW_PGM)
	cp $(PGM) $(RHESSYS_BIN)
	cp $(EW_PGM) $(RHESSYS_BIN)

clean:
	rm -f $(OBJECTS)

clobber:	clean
	rm -f $(PGM) $(EW_PGM)

cxy.o: cxy.c
	$(CC) $(CFLAGS) -I$(GISBASE)/include -I../cf -c cxy.c
