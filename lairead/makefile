PGM = lairead
CC  = gcc 
CFLAGS  = -O -I include
RHESSYS_BIN = /usr/local/bin

OBJECTS = main.o read_allom_table.o build_flow_table.o sys.o fileio.o \
	find_patch.o change_world.o print_flow_table.o  zero_flow_table.o link_patch_veg.o 

LIBES = -lm

$(PGM): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBES) $(CFLAGS) -o $(PGM) 

clean:
	rm -f $(OBJECTS)

clobber:	clean
	rm -f $(PGM)

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
main.o:
	$(CC) $(CFLAGS) -c main.c

install:
	cp $(PGM) $(RHESSYS_BIN)

