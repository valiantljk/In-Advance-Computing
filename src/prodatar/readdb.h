#include <libmemcached/memcached.h>
#ifndef READDB_H_INCLUDE
#define READDB_H_INCLUDE
void readdb(memcached_st * memc, memcached_return * rc, char * op, int * new_analysis_range,int total_query,int chunk_size);

#endif
