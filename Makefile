CC = g++ -std=c++0x
OPT = -O3
OPT = -g
WARN = -Wall
#ERR = -Werror

CFLAGS = $(OPT) $(WARN) $(ERR) $(INC) $(LIB)

SIM_SRC = main.cc cache.cc io_function.cc msi.cc mosi.cc moesi.cc mi.cc MSI_up.cc firefly.cc mesi.cc write_through.cc directory.cc fbv.cc ssci.cc dragon.cc
SIM_OBJ = main.o cache.o io_function.o msi.o mosi.o moesi.o mi.o MSI_up.o firefly.o mesi.o write_through.o directory.o fbv.o ssci.o dragon.o

all: simulate_cache
	@echo "Compilation Done ---> nothing else to make :) "

simulate_cache: $(SIM_OBJ)
	$(CC) -o simulate_cache $(CFLAGS) $(SIM_OBJ) -lm
	@echo "---------------------------------------------------------------"
	@echo "-----------SP2024-506 BUS BASED CACHE SIMULATOR     -----------"
	@echo "---------------------------------------------------------------"

.cc.o:
	$(CC) $(CFLAGS)  -c $*.cc

clean:
	rm -f *.o simulate_cache

