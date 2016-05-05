#!/bin/bash

# [remove.sh]
# aim: delete [write.sh] day1 files

START=$(date "+%s")

remove_test()
{
    NUM=0
    for ((i=1; i<=$DIR_COUNT; i++)); do
    {
        for ((j=1; j<=100; j++)); do
        {
            # delete time more than 24 hours!
            NOW=$(date "+%s") 
            TIME=$((NOW-START))
            echo "The remove.sh runtime is $TIME"
            if (($TIME >= 86400)); then
                echo "delete file more than 24 hours, runtime $TIME" >> /tmp/remove_sh.log
            fi

            # remove the files!
            rm -rf /cluster2/test/client-$HOSTNAME/newdir$i/year/$DAY/client-$HOSTNAME-$j*
            let NUM=$NUM+150    
            echo "The runtime is $TIME, delete $NUM files" >> /tmp/remove_sh.log
        }
        done
    }
    done 
}

case $1 in
day*)
    echo "remove.sh is running on $HOSTNAME!"
    DAY=$1
    DIR_COUNT=$2
    remove_test 
    echo "Total delete $NUM files!"
    ;;
*)
    echo "Usage: $0 day[1/2/3/4/5]"
    ;;
esac
