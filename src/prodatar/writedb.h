#include <libmemcached/memcached.h>
#include <mpi.h>
#ifndef WRITEDB_H_INCLUDE
#define WRITEDB_H_INCLUDE
void writedb(memcached_st * memc, memcached_return *rc, char * coarse_key, char * dim, MPI_Offset *mpi_start, MPI_Offset *mpi_count,char * op,int chunk_size);

#endif
