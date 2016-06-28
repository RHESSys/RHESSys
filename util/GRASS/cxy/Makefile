PGM = cxy
CC  = gcc 
# For grassIO library, we need to know where createflowpaths lives 
# grassIO should be refactored into its own location in the source tree 
CFPATH = ../../../cf
CFLAGS = -g -O0 -I$(GISBASE)/include -I$(CFPATH)/include
RHESSYS_BIN = /usr/local/bin

OBJECTS = cxy.o grassio.o

LIBS = -lm
GIS_LIBS = -lgrass_gis

$(PGM): $(OBJECTS)
	$(CC) $(OBJECTS) -L$(GISBASE)/lib $(GIS_LIBS) $(LIBS) $(CFLAGS) -o $(PGM) 

install: $(PGM)
	cp $(PGM) $(RHESSYS_BIN)

clean:
	rm -f $(OBJECTS)

clobber:	clean
	rm -f $(PGM)

grassio.o:
	# Ask createflowpaths to compile grassio.c for us
	# Then copy grassio.o to the current directory so we can link to it 
	$(MAKE) -C $(CFPATH) objects
	cp $(CFPATH)/objects/grassio.o .
cxy.o: cxy.c
	$(CC) $(CFLAGS) -c cxy.c
