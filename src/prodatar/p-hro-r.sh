#!/bin/bash
#$ -V
#$ -cwd
#$ -S /bin/bash
#$ -N prodata-read
#$ -o prodata-read-$JOB_ID.o
#$ -e prodata-read-$JOB_ID.e
#$ -q normal
#$ -pe fill 96
#$ -P hrothgar

runnp="mpirun -np 96 "
exefile=" ./prodata-read "
datafile=" -l /lustre/scratch/jialliu/data/syn-480.nc "
start=" -a 0 -b 0 -c 0 -d 0 "
length=" -f 100 -g 100 -h 10 -e "
io=" -i 0 "
db=" -j 0 "

cmdflush="mpirun -np 96 ./prodata-read -l /lustre/scratch/jialliu/data/syn-120.nc -b 0 -c 0 -d 0 -a 0 -e 1200000 -f 100 -g 10 -h 10 -i 0 -j 0"

for p in 0 10 20 50 100
do	
	let pp=2400*p
	let l=240000-pp
	echo "Overlapping Percenge:$p1" >> prodata-read-$JOB_ID.o
	
	cmd=$runnp$exefile$datafile$start$length$l$io$db
	$cmd

	echo "The running command is "$cmd >> prodata-read-$JOB_ID.o
	echo "Begin flushing the filesystem and cache" >> prodata-read-$JOB_ID.o

	$cmdflush
        $cmdflush
        $cmdflush

	echo "End flushing" >> prodata-read-$JOB_ID.o

done




##comment##############################
#start
#a:time
#b:level
#c:lat
#d:lon

#length
#e:time
#f:level
#g:lat
#h:lon


#i: io mode, 0 ind, 1 col
#j: db, 0 no connect, 1 connect

