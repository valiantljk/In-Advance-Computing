
#include <stdlib.h>

#include <stdio.h>
#include <sys/time.h>
#include <float.h>
#include "getopt.h"
#include <time.h>
#include "timer.h"
#include <string.h>
#define CACHE_FILE "cachefile1"

void main(int argc, char ** argv)
{
 int i;
   FILE *fp;
  struct timeval start_time[20];
  float elapse[20];
  char range0[]=",0 0 0 100 200 300 ";
  int option;
 option = strtol(argv[1], NULL, 10);
   timer_on(0);
  if(option==0){
   if((fp=fopen(CACHE_FILE,"a"))==NULL)
       printf("cannot open file\n");
   else{
    for(i=0;i<1000000;i++){
   	 if(fwrite(&range0,sizeof(range0),1,fp)!=1)
   	 {
   	   if(feof(fp))
   	     printf("Premature end of file\n");
   	   else
   	   printf("File write error.\n");
   	 }
    }
    }
    fclose(fp);
  }
  timer_off(0);
  timer_on(1);
 if(option==1){
 long lSize;
char *buffer;
char *temp_buffer;
fp = fopen (CACHE_FILE , "rb" );
if( !fp ) perror("blah.txt"),exit(1);

fseek( fp , 0L , SEEK_END);
lSize = ftell( fp );
rewind( fp );

/* allocate memory for entire content */
buffer = calloc( 1, lSize+1 );
printf("entir size : %ld\n",lSize);
if( !buffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);

/* copy the file into the buffer */
if( 1!=fread( buffer , lSize, 1 , fp) )
  fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);

for(i=0;i<10000;i++)
{
 temp_buffer=malloc(sizeof(char *));
 memcpy(temp_buffer,buffer+i,sizeof(char *));
}
free(buffer);
fclose(fp);
}
timer_off(1);
   printf("storetime:%f\nsearchtime:%f\n",elapse[0],elapse[1]);

}
