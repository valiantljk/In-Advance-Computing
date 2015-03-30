#include"readdb.h"
#include<stdio.h>
#include<string.h>
#include <libmemcached/memcached.h>
#include<pnetcdf.h>
#include<mpi.h>
/*
*OVERLAP DETECTION
*PARAMETERS:server_string, memcached memc, operation op, all_ranges, splited range of orig io
*FUNCTIONS: Check Coarse Overlapping, 
*TODO list: 
*1. Master way of query
*2. Distributed way of query
*3. Coarse query by the op
*/
//readdb, first read the coarse ranges, all_ranges
//second, read the sub results
//return numver of hits of sub-results
#define TRUE 1
#define FALSE 0
#define NDIMS 4
bool overlap(int *,int *);
char * construct_key_search(char *, char *, int);
char * get_range(char *);
void get_split(char **, char *,int *);
int * get_dim(int *,char *);
 

void readdb(memcached_st * memc, memcached_return *rc, char * op, int * new_analysis_range,int total_query,int chunk_size)
{
     uint32_t flags;
     size_t value_length;
     char * all_ranges;
     //read all coarse ranges
     
     all_ranges=memcached_get(memc,op,strlen(op),&value_length,&flags,rc);
     
     char **splited=(char **)malloc(sizeof(char *)*10);
     if(!splited) fprintf(stderr,"malloc splited error\n");
     char **overlapped=(char **)malloc(sizeof(char *)*10);
     int j=0;
     int ii,i;
     for(ii=0;ii<10;ii++)
     {
     	splited[ii]=(char *)malloc(sizeof(char)*50);
	overlapped[ii]=(char *)malloc(sizeof(char)*50);
	if(!splited[ii]&&!overlapped[ii]) fprintf(stderr,"splited[i] or overlapped[ii] malloc error");
     }
     int overlap_index=0;
     int splited_index=0;

	if(all_ranges&&all_ranges[0]!='\0'){
     		get_split(splited,all_ranges,&splited_index);
        
		int * dim_array=malloc(8*sizeof(int));
		if(!dim_array) fprintf(stderr,"malloc error dimarray\n");
        	for(i=0;i<splited_index;i++){
	  		char * temp_split=malloc(sizeof(char)*100);
			if(!temp_split) fprintf(stderr,"malloc error tempsplit\n");
		  	strcpy(temp_split,splited[i]);
			get_dim(dim_array,temp_split);
	        	//if overlap, then save for next finer query
			if(overlap(dim_array,new_analysis_range)){
	      	   		strcpy(overlapped[overlap_index++],splited[i]);
		  	}
        	}


         	/*Query DB for sub_results*/
    		for(i=0;i<overlap_index;i++){
			//query the sub result, by each small range
			//chunk granularity, first, along nserc, step size is x

        		char * current_overlap2=(char *)malloc(sizeof(char)*100);
       			char * current_overlap=(char *)malloc(sizeof(char)*100);
			if(!current_overlap||!current_overlap2) fprintf(stderr,"malloc error current_overlap\n");
			strcpy(current_overlap,overlapped[i]);
			char * chunk_range=get_range(current_overlap);
			
			int steps=atoi(chunk_range);
			int loop_query=steps/chunk_size;
		
			char * key_chunk_range[loop_query];
			char * sub_result;
	
			for(j=0;j<loop_query;j++){
				strcpy(current_overlap2,overlapped[i]);
				key_chunk_range[j]=construct_key_search(current_overlap2, op, chunk_size);
        			sub_result=memcached_get(memc,key_chunk_range[j],strlen(key_chunk_range[j]),&value_length,&flags,rc);
				total_query++;
			}
		
           		if(current_overlap2) free(current_overlap2);
        		if(current_overlap) free(current_overlap);
	
     		}
	}
	for(ii=0;ii<10;ii++)
    	{
     		if(splited[ii]!=NULL) free(splited[ii]);
        	if(overlapped[ii]!=NULL) free(overlapped[ii]);
    	}
	if(splited!=NULL) free(splited);
	if(overlapped!=NULL) free(overlapped);

}


//function to check overlapping
bool overlap(int * a,int * b)
{
   int i,j;
    int overlap=0;
  //check start index
  for(i=0;i<NDIMS;i++)
  {
	if(a[i]>=b[i])
	{
		if((a[i]-b[i])<b[i+NDIMS])
		overlap=1;
	}
	else if(b[i]-a[i]<a[i+NDIMS])
		overlap=1;
  }	
  if(overlap==1)
	return TRUE;
   return FALSE;
}

//function to construct the search key for each sub-chu
char *construct_key_search(char *Original, char * operation, int chunk_size)
{
    char * original=(char *)malloc(sizeof(char)*100);
     if(!original) fprintf(stderr,"malloc error original\n");
    strcpy(original,Original);
    char * temp[9];
   
    char * blank="f";
    int i;
    for(i=0;i<9;i++) 
    {
     temp[i]=(char *)malloc(sizeof(char)*100);
     if(!temp[i]) fprintf(stderr,"malloc error temp[i]\n");
    }
    temp[0]=strtok(original," ");
    temp[1]=strtok(NULL," ");
    temp[2]=strtok(NULL," ");
    temp[3]=strtok(NULL," ");
    sprintf(temp[4],"%d",chunk_size);
    temp[5]=strtok(NULL," ");
    temp[6]=strtok(NULL," ");
    temp[7]=strtok(NULL," ");
    temp[8]=operation;

    char *finalstring=(char *)malloc(sizeof(char)*100);
    if(!finalstring) fprintf(stderr,"malloc error finalstring\n");
    strcpy(finalstring,temp[0]);
    for(i=1;i<9;i++){
    strcat(finalstring,blank);
    strcat(finalstring,temp[i]);
    }
   
   return finalstring;
}

char * get_range(char * current_overlap2){
	char * current_overlap=(char *)malloc(sizeof(char)*100);
	char * chunk_range=(char *)malloc(sizeof(char)*100);
	strcpy(current_overlap,current_overlap2);
	strtok(current_overlap," ");
	strtok(NULL," ");
	strtok(NULL," ");
	strtok(NULL," ");
	chunk_range=strtok(NULL," ");//the 5-th position is the length of time dimension
return chunk_range;
}


void get_split(char **splited, char * all_ranges,int * splited_index)
{
     char * temp_split=(char *)malloc(sizeof(char)*100);
     char * this_ranges=(char *)malloc(sizeof(char)*100);
     if(!this_ranges) fprintf(stderr,"malloc error this_ranges\n");
     strcpy(this_ranges,all_ranges);
     if(this_ranges&&this_ranges[0]!='\0'){
     temp_split=strtok(this_ranges,",");
     strcpy(splited[0],temp_split);
     }
     int i=1;
     while(temp_split){
        temp_split = strtok(NULL,",");
	if(temp_split){
	strcpy(splited[i],temp_split);
	i++;}
     }
     *splited_index=i;
}
int * get_dim(int *dim_array,char * current)
{
	 
	 char  *current_dim=(char *)malloc(sizeof(char));
	 int j=1;
	 char * mycurrent=(char *)malloc(sizeof(char)*100);
	  if(!mycurrent) fprintf(stderr,"malloc error mycurrent\n");
	 strcpy(mycurrent,current);
         current_dim=strtok(mycurrent," ");
         dim_array[0]=atoi(current_dim);
         
         while(current_dim){
	 current_dim=strtok(NULL, " ");
	 if(current_dim)
         dim_array[j]=atoi(current_dim);
	 j++;
         }         

return dim_array;
}

