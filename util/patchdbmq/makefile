PGM = patchdbmq
DOCDIR = docs
RHESSYS_BIN = /usr/local/bin
CC  = g++
INCLUDES = -Iinclude -I/usr/local/include -I../../rhessys/output/filtering
CFLAGS = -g -Wall
#CFLAGS_TESTS = `pkg-config --cflags glib-2.0` -g -Wall -std=c99
LDLIBS = -L/usr/local/lib -lzmq -lcassandra `pkg-config --cflags --libs protobuf`

#if pkg-config libzmq --exists; then
#  LDLIBS=$(pkg-config --libs --cflags libczmq libzmq)
#fi

#LDLIBS_TESTS = `pkg-config --libs glib-2.0` -L$(GISBASE)/lib -lm -lgrass_gis

#ifdef COVERAGE
#CFLAGS += -fprofile-arcs -ftest-coverage
#CFLAGS_TESTS += -fprofile-arcs -ftest-coverage
#LDLIBS += -lgcov
#LDLIBS_TESTS += -lgcov
#endif
#COVERAGE_FILES = *.gcov

SRCDIR = src
SRCS := $(shell find $(SRCDIR) -name '*.cc')
OBJDIR = objects
OBJECTS := $(patsubst $(SRCDIR)/%.cc,$(OBJDIR)/%.o,$(SRCS))
OBJECTS := $(OBJECTS) ../../rhessys/objects/patch.pb.o

#TESTS_ROOTDIR = test
#SRCDIR_TESTS = $(TESTS_ROOTDIR)/src
#SRCS_TESTS := $(shell find $(SRCDIR_TESTS) -name '*.c')
#TESTSDIR = $(TESTS_ROOTDIR)
#OBJDIR_TESTS = $(TESTS_ROOTDIR)/objects
#OBJECTS_TESTS := $(patsubst $(SRCDIR_TESTS)/%.c,$(OBJDIR_TESTS)/%.o,$(SRCS_TESTS))
#TESTS := $(patsubst $(SRCDIR_TESTS)/%.c,$(OBJDIR_TESTS)/%,$(SRCS_TESTS))
#OBJECTS_NO_MAIN := $(patsubst $(OBJDIR)/main.o,,$(OBJECTS))
#TESTS_TO_RUN = $(shell find $(TESTS_ROOTDIR) -type f -perm +111 -maxdepth 1)

OS := $(shell uname)

all: dir $(PGM)

dir:
	mkdir -p $(DOCDIR)
	mkdir -p $(OBJDIR)
#	mkdir -p $(OBJDIR_TESTS)

$(PGM): $(OBJECTS)
ifeq ($(OS), Darwin)
	$(CC) $(OBJECTS) $(CFLAGS) $(INCLUDES) $(LDLIBS) -o $(PGM)
else ifeq ($(OS), Linux)
	$(CC) $(OBJECTS) $(CFLAGS) $(INCLUDES) $(LDLIBS) -o $(PGM)
else
	$(CC) $(OBJECTS) $(CFLAGS) $(INCLUDES) $(LDLIBS) -o $(PGM)
endif

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $< 

objects: $(OBJECTS)

#test: $(TESTS) 
#	# Run each unit test
#	$(patsubst %,%;,$(TESTS_TO_RUN))

#$(TESTS): $(OBJECTS) $(OBJECTS_TESTS) 
#ifeq ($(OS), Darwin)
#	$(CC) $(OBJECTS_NO_MAIN) $@.o $(CFLAGS_TESTS) $(INCLUDES) $(LDLIBS_TESTS) -o $@
#else ifeq ($(OS), Linux)
#	$(CC) $(OBJECTS_NO_MAIN) $@.o $(CFLAGS_TESTS) $(INCLUDES) $(LDLIBS_TESTS) -lbsd -Wl,-rpath=$(GISBASE)/lib -o $@
#else
#	$(CC) $(OBJECTS_NO_MAIN) $@.o $(CFLAGS_TESTS) $(INCLUDES) $(LDLIBS_TESTS) -Wl,-rpath=$(GISBASE)/lib -o $@
#endif
#	cp $@ $(TESTS_ROOTDIR)
#
#$(OBJDIR_TESTS)/%.o: $(SRCDIR_TESTS)/%.c
#	$(CC) $(CFLAGS_TESTS) $(INCLUDES) -c -o $@ $< 
#
#functest:
#	# Run Python-based functional testing
#	LD_LIBRARY_PATH=$(GISBASE)/lib python -m unittest discover -s test

doc:
	doxygen

install: $(PGM)
	cp $(PGM) $(RHESSYS_BIN)

clean:
	rm -f $(PGM) $(OBJECTS) $(TESTS) $(OBJECTS_TESTS) $(TESTS_TO_RUN) $(COVERAGE_FILES)

docclean:
	rm -rf $(DOCDIR)/html



