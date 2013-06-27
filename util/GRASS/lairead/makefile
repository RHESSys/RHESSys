PGM = lairead
CC  = gcc 
# For grassIO library, we need to know where createflowpaths lives 
# grassIO should be refactored into its own location in the source tree 
CFPATH = ../../../cf
CFLAGS = -O -I$(GISBASE)/include -I$(CFPATH)/include
#CFLAGS = -g -I$(GISBASE)/include -I$(CFPATH)/include
RHESSYS_BIN = /usr/local/bin
GIS_LIBS = -lgrass_gis

OBJECTS = main.o read_allom_table.o build_flow_table.o sys.o fileio.o \
	find_patch.o change_world.o print_flow_table.o zero_flow_table.o link_patch_veg.o grassio.o

LIBS = -lm

$(PGM): $(OBJECTS)
	$(CC) $(OBJECTS) -L$(GISBASE)/lib $(GIS_LIBS) $(LIBS) $(CFLAGS) -o $(PGM) 

clean:
	rm -f $(OBJECTS)

clobber:	clean
	rm -f $(PGM)
	
grassio.o:
	# Ask createflowpaths to compile grassio.c for us
	# Then copy grassio.o to the current directory so we can link to it 
	$(MAKE) -C $(CFPATH) objects
	cp $(CFPATH)/objects/grassio.o .
print_flow_table.o:
	$(CC) $(CFLAGS) -c print_flow_table.c
zero_flow_table.o:
	$(CC) $(CFLAGS) -c zero_flow_table.c
build_flow_table.o:
	$(CC) $(CFLAGS) -c build_flow_table.c
find_patch.o:
	$(CC) $(CFLAGS) -c find_patch.c
link_patch_veg.o:
	$(CC) $(CFLAGS) -c link_patch_veg.c
change_world.o:
	$(CC) $(CFLAGS) -c change_world.c
fileio.o:
	$(CC) $(CFLAGS) -c fileio.c
sys.o:
	$(CC) $(CFLAGS) -c sys.c
read_allom_table.o:
	$(CC) $(CFLAGS) -c read_allom_table.c
main.o: main.c
	$(CC) $(CFLAGS) -c main.c

install:
	cp $(PGM) $(RHESSYS_BIN)

