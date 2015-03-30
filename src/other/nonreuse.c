/*
 * 
 * cross analysis checking result code.
 * Jialin Liu 
 *
  *Date: Jun 14 2013
 */

#include <stdlib.h>
#include <mpi.h>
#include <pnetcdf.h>
#include <stdio.h>
#include <sys/time.h>
#include <float.h>
#include "getopt.h"
#include <time.h>
#include "timer.h"
#define LAT_NAME "latitude"
#define LON_NAME "longitude"
#define REC_NAME "time"
#define LVL_NAME "level"

#define NAME_MAX 255
#define TEMP_NAME "temperature"
#define UNITS "units"
#define DEGREES_EAST "degrees_east"
#define DEGREES_NORTH "degrees_north"
//#define FILENAME "/lustre/scratch/jialliu/crossadir/ca1.nc"
char FILENAME[NAME_MAX];
#define NDIMS 4
static void handle_error(int status)
{
	fprintf(stderr, "%s\n", ncmpi_strerror(status));
	exit(-1);
}

int main(int argc, char **argv) {

    int rank, nprocs;
    int ret, ndims, nvars, ngatts, unlimited;
    size_t start[NDIMS], count[NDIMS];
    MPI_Offset *mpi_start, *mpi_count;
    MPI_Offset *dim_sizes, var_size;
    int ncid, pres_varid, temp_varid;
    int lvl,lat,lon,rec, i;
    float SUM=0;
    struct timeval start_time[20];
    float elapse[20];
    int c;
    int s_time,s_level,s_lat,s_lon;
    int l_time,l_level,l_lat,l_lon;
    int cache_mode=1;
    float * temp_in;
   /*paramaters that needed to be input*/
    /*
     *a:s position on time dim
     *b:start position on level dim
     *c:start position on lat dim
     *d:start position on lon dim
     *e:length on time dim
     *f:length on level dim
     *g:length on lat dim
     *h:length on lon dim
     *p:overlapping percentage, 10%, 20%, 50%, 100%
     *
     */

    /*parse the input*/
    while ((c = getopt (argc, argv, "l:a:b:c:d:e:f:g:h:")) != -1)
    switch (c)
      {
      case 'l':
      strncpy(FILENAME,optarg,NAME_MAX);
      break;
      case 'a':
	s_time = strtol(optarg, NULL, 10);
	break;
      case 'b':
	s_level = strtol(optarg, NULL, 10);
	break;
      case 'c':
	s_lat = strtol(optarg, NULL, 10);
	break;
      case 'd':
	s_lon = strtol(optarg, NULL, 10);
	break;
      case 'e':
	l_time = strtol(optarg, NULL, 10);
	break;
      case 'f':
	l_level = strtol(optarg, NULL, 10);
	break;
      case 'g':
	l_lat = strtol(optarg, NULL, 10);
	break;
      case 'h':
	l_lon = strtol(optarg, NULL, 10);
	break;
      default:
	break;
      }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    /*STEP ONE*/

    MPI_Barrier(MPI_COMM_WORLD);
    timer_on(1);

   /*open raw datasets*/
    ret = ncmpi_open(MPI_COMM_WORLD, FILENAME, NC_NOWRITE, MPI_INFO_NULL,
                    &ncid);
    if (ret != NC_NOERR) handle_error(ret);
    /* Get the varids of the pressure and temperature netCDF variables. */

    if ((ret = ncmpi_inq_varid(ncid, TEMP_NAME, &temp_varid)))
       if (ret != NC_NOERR) handle_error(ret);

    ret = ncmpi_inq(ncid, &ndims, &nvars, &ngatts, &unlimited);
    if (ret != NC_NOERR) handle_error(ret);
    
    dim_sizes = calloc(ndims, sizeof(MPI_Offset));
  
    for(i=0; i<ndims; i++)  {
            ret = ncmpi_inq_dimlen(ncid, i, &(dim_sizes[i]) );
            if (ret != NC_NOERR) handle_error(ret);
    }
 
    /*set dimension size*/
    int NLVL=(int)dim_sizes[0];
    int NLAT=(int)dim_sizes[1];
    int NLON=(int)dim_sizes[2];
    int NREC=(int)dim_sizes[3];
        
    /*set accessing start position and length*/
    mpi_start = (MPI_Offset *)malloc(ndims*sizeof(MPI_Offset));
    mpi_count = (MPI_Offset *)malloc(ndims*sizeof(MPI_Offset));

    /*parallelize along the slowest dimension*/
    mpi_count[0] = l_time/nprocs;    
    /*set up the 3D cube for each process*/
    mpi_count[1] = l_level;
    mpi_count[2] = l_lat;
    mpi_count[3] = l_lon;    
    mpi_start[0] = s_time+rank*mpi_count[0];
    mpi_start[3] = s_lon;  
    mpi_start[2] = s_lat;
    mpi_start[1] = s_level;	      

    /*buffer for each process*/
    size_t request_size=mpi_count[0]*l_lon*l_lat*l_level*sizeof(float);
    temp_in=(float *)malloc(request_size);
  
    MPI_Barrier(MPI_COMM_WORLD);
    timer_off(1);
    timer_on(2);
    ret=ncmpi_begin_indep_data(ncid);
    if (ret != NC_NOERR) handle_error(ret);
    ret = ncmpi_get_vara_float(ncid, temp_varid, mpi_start,
                                    mpi_count, temp_in);
    if (ret != NC_NOERR) handle_error(ret);
    ret=ncmpi_end_indep_data(ncid);
    if (ret != NC_NOERR) handle_error(ret);
    MPI_Barrier(MPI_COMM_WORLD);
    timer_off(2);
    timer_on(3);

    /*perform the calculation*/
    int D3size=l_level*l_lat*l_lon;
    int D2size=l_lat*l_lon;

    float sum=0;
    int inec,ilvl,ilon,ilat;
    for(inec=0;inec<mpi_count[0];inec++)
     for(ilvl=0;ilvl<l_level;ilvl++)
      for(ilat=0;ilat<l_lat;ilat++)
	for(ilon=0;ilon<l_lon;ilon++)
            sum+=*(temp_in+inec*D3size+ilvl*D2size+ilat*l_lon+ilon);
    
    MPI_Reduce(&sum,&SUM,1,MPI_FLOAT,MPI_SUM,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    timer_off(3);

   if(rank==0)
    {
   	printf("Request size per process %dMB\n",(int)(request_size/1024.0/1024.0));
        printf("Dataset size:\ntime:%d\nlevel:%d\nlat:%d\nlon:%d\n",NREC,NLVL,NLAT,NLON);
        printf("Request range per process:\ntime:%d\nlevel:%d\nlat:%d\nlon:%d\n",(int)mpi_count[0],(int)mpi_count[1],(int)mpi_count[2],(int)mpi_count[3]);
        printf("Time for setup access information, malloc memory:%f\n",elapse[1]);
    	printf("Maximum I/O Cost for all processes:%f\n",elapse[2]);
    	printf("Computation cost for current task %f\n",elapse[3]);
    	printf("Final result:%f\n",SUM);
    }
    ret = ncmpi_close(ncid);
    if (ret != NC_NOERR) handle_error(ret);
    MPI_Finalize();
    return 0;
}
