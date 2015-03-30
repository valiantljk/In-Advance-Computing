#!/bin/bash
#$ -V
#$ -cwd
#$ -S /bin/bash
#$ -N prodata-write
#$ -o prodata-write-$JOB_ID.o
#$ -e prodata-write-$JOB_ID.e
#$ -q normal
#$ -pe fill 1200
#$ -P hrothgar


cmd="mpirun -np 1200 ./prodata-write -f /lustre/scratch/jialliu/data/syn-1200.nc  -c 1200000 -l 100 -t 100 -n 10"
$cmd
