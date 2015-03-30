#!/bin/bash
samplesize=1000


for querysize in 10 20 50 100
do
for cachesize in 1 2 5 10 20 30 40 50 100
do
        let ll=cachesize
	let qu=querysize
        cmd1="./mrkov-recom  "
	cmd2=" "
        cmd=$cmd1$samplesize$cmd2$qu$cmd2$ll
        $cmd >> output-recom
        echo $cmd
done
done
