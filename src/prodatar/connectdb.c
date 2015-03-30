#include"connectdb.h"
#include<stdio.h>
#include<string.h>
#include <libmemcached/memcached.h>
#include<pnetcdf.h>
#include<mpi.h>

connectdb(memcached_st * memc,memcached_return * rc){
   
    memcached_server_st *servers = NULL;
    const char * server_string="10.6.63.56:11211";
    memc= memcached_create(NULL);
    servers=memcached_servers_parse(server_string);
    *rc= memcached_server_push(memc,servers);     
    if (*rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Added server successfully\n");
    else
    fprintf(stderr,"Couldn't add server: %s\n",memcached_strerror(memc, *rc));
    rc = libmemcached_check_configuration(server_string, strlen(server_string),(time_t)0,(uint32_t)0);
    printf("configuring:%s\n",server_string);
    
}

