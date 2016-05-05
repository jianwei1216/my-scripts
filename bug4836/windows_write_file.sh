#!/bin/bash

START=$(date "+%s")

write_file()
{
        for ((i = 1; i < $WRITE_CNT; i++)); do
                NOW=$(date "+%s")  
                TIME=$((NOW-START))
                echo "The test runtime is $TIME"
                if (($TIME >= 86400)); then
                        break
                fi
                fio.exe --directory=$WORK_DIR --thread --iodepth=1 --rw=randwrite --ioengine=windowsaio --bs=1m --size=1g --numjobs=10 --group_reporting --direct=1 --name=file$i
                #fio.exe --directory=/cluster2/test/client-$HOSTNAME/day1 --thread --iodepth=16 --rw=write --ioengine=windowsaio --bs=64K --numjobs=10 --nrfiles=150 --filesize=200k,500k --rate=200K --group_reporting --name=file1_day
        done
}

WORK_DIR=$1
WRITE_CNT=$2

echo $WORK_DIR
echo $WRITE_CNT

write_file
