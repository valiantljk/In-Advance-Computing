#include"writedb.h"
#include<stdio.h>
#include<string.h>
#include <mpi.h>
#include <libmemcached/memcached.h>
#include<pnetcdf.h>


char *construct_key_store(MPI_Offset *,MPI_Offset *,int,int, char *);

void writedb(memcached_st * memc, memcached_return *rc, char * coarse_key, char * dim, MPI_Offset *mpi_start, MPI_Offset *mpi_count, char * op,int chunk_size){
    uint32_t flags;
     size_t value_length;
    char *coarse_value=(char *)malloc(sizeof(char)*100);
    if(!coarse_value) fprintf(stderr,"malloc error coarse_value");

    strcpy(coarse_value,dim);
    printf("coarse key: value:%s,%s\n",coarse_key,coarse_value);
   
    /*store the coarse range, original IO*/
     if(memcached_get(memc,coarse_key,strlen(coarse_key),&value_length,&flags,rc))
     {
	*rc=memcached_append(memc,coarse_key,strlen(coarse_key),coarse_value,strlen(coarse_value),(time_t)0,(uint32_t)0);	
     }
     else{
   	  *rc=memcached_set(memc,coarse_key,strlen(coarse_key),coarse_value,strlen(coarse_value),(time_t)0,(uint32_t)0);
     }
    if (*rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Added coarse key successfully\n");
    else
    fprintf(stderr,"Couldn't store coarse key: %s\n",memcached_strerror(memc, *rc));
    if(dim) free(dim);
    if(coarse_value) free(coarse_value);

     int j;
    /* sub-results,range+op*/
    int loop_query=(int)mpi_count[0]/chunk_size;
    char * key_chunk_range2;
    for(j=0;j<loop_query;j++)
    {
      // key_chunk_range2=malloc(sizeof(char *));
      int currentstart=mpi_start[0]+chunk_size*j;
      key_chunk_range2=construct_key_store(mpi_start,mpi_count, currentstart,chunk_size, op);
      char * sub_result=(char *)malloc(sizeof(char)*100);
      if(!sub_result) fprintf(stderr,"malloc error sub_result");
      sprintf(sub_result,"%d",j);//fake sub_results;
      //write into the database for each sub-result
      *rc=memcached_set(memc,key_chunk_range2,strlen(key_chunk_range2),sub_result,strlen(sub_result),(time_t)0,(uint32_t)0);
      if (*rc == MEMCACHED_SUCCESS)
      	fprintf(stderr,"Added sub key successfully\n");
      else
      	fprintf(stderr,"Couldn't add sub key: %s\n",memcached_strerror(memc, *rc));
      if(sub_result)
        free(sub_result);
    }
    if(key_chunk_range2)
	free(key_chunk_range2);
}

char *construct_key_store(MPI_Offset * start,MPI_Offset *count,int current_start,int current_length, char * operation)
{
    char * temp[9];
    char * blank="f";
    int i;
    for(i=0;i<9;i++){
    temp[i]=(char *)malloc(sizeof(char)*100);
    if(!temp[i]) fprintf(stderr,"malloc error temp[i]\n");
    }

    sprintf(temp[0],"%d",current_start);
    sprintf(temp[1],"%d",(int)start[1]);
    sprintf(temp[2],"%d",(int)start[2]);
    sprintf(temp[3],"%d",(int)start[3]);
    sprintf(temp[4],"%d",current_length);
    sprintf(temp[5],"%d",(int)count[1]);
    sprintf(temp[6],"%d",(int)count[2]);
    sprintf(temp[7],"%d",(int)count[3]);
    strcpy(temp[8],operation);

    char *finalstring=(char *)malloc(sizeof(char)*100);
    if(!finalstring) fprintf(stderr,"malloc error finalstring\n");
    strcpy(finalstring,temp[0]);
    for(i=1;i<9;i++){
     strcat(finalstring,blank);
    strcat(finalstring,temp[i]);
    }
    
   return finalstring;
}
