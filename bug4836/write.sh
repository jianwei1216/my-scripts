#!/bin/bash

############
# [write.sh]
# aim: write file for 24 hours.
############

START=$(date "+%s")

mkdir_directory()
{
	for ((i=1; i<=$DIR_COUNT; i++)); do
	{
		mkdir -p /cluster2/test/client-$HOSTNAME/newdir${i}/year/$DAY/
	}
	done	
}

write_test()
{
    NUM=0
    for ((i=1; i<$DIR_COUNT; i++)); do
	{
	    for ((j=1; j<=100; j++)); do
        {
            # exit testing when time more than 24 hours.
            NOW=$(date "+%s")  
            TIME=$((NOW-START))
            echo "The test runtime is $TIME"
            if (($TIME >= 86400)); then
                exit 0
            fi

            # create and write file
	        fio --ioengine=sync --rw=write --directory=/cluster2/test/client-$HOSTNAME/newdir$i/year/$DAY/ --bs=64K --iodepth=16 --numjobs=10 --nrfiles=150 --thread=1 --filesize=200k,500k --rate=200K --group_reporting --name=client-$HOSTNAME-$j >> /tmp/fio.result 
            let NUM=$NUM+150
            echo "The test runtime is $TIME, write $NUM files" >> /tmp/write_sh.log
        }
		done	
	}
	done	
}


case $1 in
day*)
    echo "Now is $HOSTNAME"
    DAY=$1
    DIR_COUNT=$2
    mkdir_directory
    write_test
    echo "Total files are $NUM"
    ;;
*)
    echo "Usage: $0 day[1/2/3/4/5]"
    ;;
esac
