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

                echo rm -f $WORK_DIR/file$i*
                rm -f $WORK_DIR/file$i*
        done
}

WORK_DIR=$1
WRITE_CNT=$2

echo $WORK_DIR
echo $WRITE_CNT

write_file
