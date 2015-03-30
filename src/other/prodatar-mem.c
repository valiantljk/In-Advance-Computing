#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include<math.h>
#include<string.h>
#include <libmemcached/memcached.h>
#include "timer.h"
int startx;
int starty;
int startz;
int lengthx;
int lengthy;
int lengthz;
int operation;
int main(int argc, char *argv[])
{
  memcached_server_st *servers = NULL;
  memcached_st *memc;
  memcached_return rc;
  char *value= "keyvalue";

  struct timeval start_time[20];
  float elapse[20];
  const char * server_string="localhost:11221";
  memc= memcached_create(NULL);
  servers=memcached_servers_parse(server_string);
  rc= memcached_server_push(memc,servers);
  rc= memcached_flush(memc,0);
  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Added server successfully\n");
  else
    fprintf(stderr,"Couldn't add server: %s\n",memcached_strerror(memc, rc));
  long long i=0; 
  long long total_entry;
  long long total_search;
  int c;
  while ((c = getopt (argc, argv, "n:q:")) != -1)
  switch (c)
  {
    case 'n':
    total_entry = strtol(optarg, NULL, 10);
	break;
    case 'q':
    total_search = strtol(optarg,NULL,10);
    default:
	break;
  }
  char range0[]=",0 0 0 100 200 300 ";
  timer_on(0);
  char *temp_key=malloc(sizeof(char *));
  for(i=0;i<total_entry;i++){
   sprintf(temp_key,"%lld",i);
   rc=memcached_set(memc,temp_key,strlen(temp_key),range0,strlen(range0),(time_t)0,(uint32_t)0);
  }
  size_t size_entry=sizeof(int)+sizeof(range0);
  double total_size=(double)total_entry*(double)size_entry/1024.0/1024.0;//in MBs
  if(rc==MEMCACHED_SUCCESS)
	 fprintf(stderr,"Added value successfully\n");  
  else
	fprintf(stderr,"Added value failed\n",memcached_strerror(memc, rc));
  timer_off(0);

  char *searchkey=malloc(1*sizeof(char));
  char *searchedvalue=malloc(100*sizeof(char));
  uint32_t flags;
  size_t value_length;
  timer_on(1);
  char * mytemp_key=malloc(sizeof(char *));
  if (rc == MEMCACHED_SUCCESS){
   for(i=0;i<total_search;i++){
	sprintf(mytemp_key,"%d",i);
	searchedvalue= memcached_get(memc,mytemp_key,strlen(mytemp_key),&value_length,&flags,&rc);
	}
  }
  if(rc==MEMCACHED_SUCCESS)
  printf("search success\n");
  timer_off(1);
  printf("store100000time:%f\nsearch100000 time:%f entrysize:%d total size:%fMBs\n",elapse[0],elapse[1],size_entry,total_size);
  return 0;
}
