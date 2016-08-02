# shorthand for your MPI C++ compiler
CC = mpicxx
# Your Auryn install path
AURYNDIR = /home/zenke/auryn/
# Path to libauryn.a (see compiling Auryn)
BUILDDIR = $(AURYNDIR)/build/release/src/

CFLAGS= -ansi -Wall -pipe -O3 -DNDEBUG -ffast-math -funsafe-math-optimizations \
        -march=native -mtune=native -pedantic \
        -I$(AURYNDIR)/src -I$(AURYNDIR)/dev/src

LDFLAGS=-L$(BUILDDIR) -lauryn \
        -lboost_program_options -lboost_serialization -lboost_mpi
# # debug
# BUILDDIR = $(AURYNDIR)/build/debug/src/
# CFLAGS= -ansi -Wall -pipe -g \
#         -march=native -mtune=native -pedantic \
#         -I$(AURYNDIR)/src -I$(AURYNDIR)/dev/src

OBJECTS=
SIMS=sim_hopfield sim_hopfield2

.SECONDARY:

all: $(OBJECTS) $(SIMS)


sims: $(SIMS)

tests: $(TESTSIMS)

sim_%: sim_%.o $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $< $(LDFLAGS) -o $(subst .o,,$<)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean: 
	rm -f *.o $(SIMS) $(TESTSIMS)
