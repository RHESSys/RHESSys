PGM = cf9.1
EW_PGM = ew_horizon
CC  = gcc 
CFLAGS  = 
RHESSYS_BIN = /usr/local/bin

OBJECTS = main.o build_flow_table.o fileio.o find_patch.o check_neighbours.o zero_flow_table.o \
print_flow_table.o sort_flow_table.o compute_gamma.o remove_pits.o \
find_top.o in_list.o adjust_pit.o add_roads.o find_stream.o \
find_max_flna.o find_min_flna.o route_roads_to_patches.o compute_upslope_area.o \
print_drain_stats.o compute_drainage_density.o compute_dist_from_road.o print_stream_table.o \
grassio.o

LIBS = -lm
GIS_LIBS = -lgrass_gis

$(PGM): $(EW_PGM) $(OBJECTS)
	$(CC) $(OBJECTS) -L$(GISBASE)/lib $(GIS_LIBS) $(LIBS) $(CFLAGS) -o $(PGM) 

$(EW_PGM): ew_horizon.o fileio.o grassio.o
	$(CC) ew_horizon.o fileio.o grassio.o $(CFLAGS) -L$(GISBASE)/lib $(LIBS) $(GIS_LIBS) -o $(EW_PGM)	

install: $(PGM) $(EW_PGM)
	cp $(PGM) $(RHESSYS_BIN)
	cp $(EW_PGM) $(RHESSYS_BIN)

clean:
	rm -f $(OBJECTS) ew_horizon.o 

clobber:	clean
	rm -f $(PGM) $(EW_PGM)

print_drain_stats.o:
	$(CC) $(CFLAGS) -c print_drain_stats.c
print_stream_table.o:
	$(CC) $(CFLAGS) -c print_stream_table.c
print_flow_table.o:
	$(CC) $(CFLAGS) -c print_flow_table.c
zero_flow_table.o:
	$(CC) $(CFLAGS) -c zero_flow_table.c
in_list.o:
	$(CC) $(CFLAGS) -c in_list.c
adjust_pit.o:
	$(CC) $(CFLAGS) -c adjust_pit.c
ew_horizon.o: ew_horizon.c
	$(CC) -I$(GISBASE)/include $(CFLAGS) -c ew_horizon.c
add_roads.o:
	$(CC) $(CFLAGS) -c add_roads.c
find_top.o:
	$(CC) $(CFLAGS) -c find_top.c
find_stream.o:
	$(CC) $(CFLAGS) -c find_stream.c
find_min_flna.o:
	$(CC) $(CFLAGS) -c find_min_flna.c
find_max_flna.o:
	$(CC) $(CFLAGS) -c find_max_flna.c
remove_pits.o:
	$(CC) $(CFLAGS) -c remove_pits.c
sort_flow_table.o:
	$(CC) $(CFLAGS) -c sort_flow_table.c
build_flow_table.o: build_flow_table.c
	$(CC) $(CFLAGS) -c build_flow_table.c
compute_upslope_area.o:
	$(CC) $(CFLAGS) -c compute_upslope_area.c
compute_dist_from_road.o:
	$(CC) $(CFLAGS) -c compute_dist_from_road.c
compute_drainage_density.o:
	$(CC) $(CFLAGS) -c compute_drainage_density.c
compute_gamma.o:
	$(CC) $(CFLAGS) -c compute_gamma.c
route_roads_to_patches.o:
	$(CC) $(CFLAGS) -c route_roads_to_patches.c
find_patch.o:
	$(CC) $(CFLAGS) -c find_patch.c
fileio.o:
	$(CC) $(CFLAGS) -c fileio.c
grassio.o:
	$(CC) $(CFLAGS) -I$(GISBASE)/include -c grassio.c
main.o: main.c
	$(CC) $(CFLAGS) -I$(GISBASE)/include -c main.c

