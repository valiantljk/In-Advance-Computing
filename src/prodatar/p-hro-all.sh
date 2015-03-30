#!/bin/bash
#$ -V
#$ -cwd
#$ -S /bin/bash
#$ -N prodata-read-sim
#$ -o prodata-read-sim-$JOB_ID.o
#$ -e prodata-read-sim-$JOB_ID.e
#$ -q normal
#$ -pe fill 120
#$ -P hrothgar

runnp="mpirun -np "
exefile=" ./prodata-read-sim "
datafile=" -l /lustre/scratch/jialliu/data/syn-480.nc "
start=" -a 0 -b 0 -c 0 -d 0 "
length=" -f 100 -g 100 -h 10 -e 120000"
io=" -i 0 "
db=" -j 1 "
ndb=" -n "
query=" -k 1000000 -m 100000"
cmd=$runnp$exefile$datafile$start$length$io$db$query$ndb
cmdflush="mpirun -np 120 ./prodata-read -l /lustre/scratch/jialliu/data/syn-120.nc -b 0 -c 0 -d 0 -a 0 -e 1200000 -f 100 -g 10 -h 10 -i 0 -j 0"

for i in 24 48 96 120
do
	let np=i
	cmdi=$runnp$np$exefile$datafile$start$length$io$db$query$ndb$i
	$cmdi
	$cmdflush
        $cmdflush
	$cmdflush
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
#k: total
#m: query
