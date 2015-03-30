#!/bin/bash
#PBS -q debug
#PBS -l mppwidth=20
#PBS -l walltime=00:30:00
#PBS -N prodatar
#PBS -e prodataread.$PBS_JOBID.err
#PBS -o prodataread.$PBS_JOBID.out
#PBS -V
       cmd1="aprun -n 20 ./prodata-system/src/prodataR -a 0 -b 10 -c 0 -d 0 -e 2000 -f 1 -g 100 -h 100 -p 10"
        cmd2=" -l /scratch2/scratchdirs/jialin/orgc100s1/prod2.nc"
        cmd=$cmd1$cmd2
        echo $cmd
        echo "ind"
        $cmd

