CC=mpicc
DEBUGS= -g
CFLAGS= -c
LFLAGS= -lmemcached -pthread -I/home/jialliu/pnetcdf/include/ -I/home/jialliu/libmemcached/include/ -L/home/jialliu/libmemcached/lib -L/home/jialliu/pnetcdf/lib -lpnetcdf

all: prodata-read-sim

prodata-read-sim: proDataR-sim.o connectdb.o readdb.o writedb.o
	$(CC) $(DEBUGS) -o prodata-read-sim proDataR-sim.o connectdb.o readdb.o writedb.o $(LFLAGS)

proDataR-sim.o: proDataR-sim.c
	$(CC) $(CFLAGS) $(LFLAGS) proDataR-sim.c

connectdb.o: connectdb.c
	$(CC) $(CFLAGS) $(LFLAGS) connectdb.c

readdb.o: readdb.c
	$(CC) $(CFLAGS) $(LFLAGS) readdb.c

writedb.o: writedb.c
	$(CC) $(CFLAGS) $(LFLAGS) writedb.c

clean:
	rm -rf *.o *.e machine* *btr 

