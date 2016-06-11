#!/bin/sh

for i in `ls ./`; do
    del_flag=0
    line_count=`cat $i | wc -l`
    for ((j=1; j <= $line_count; j++));do
        if (( $del_flag != 0 ));then
            j=$del_flag
            del_flag=0
        fi
        word_count=`sed -n "${j}p" $i | wc -w`
        if (( $word_count != 5 )); then
            del_flag=$j
            line_count=$(($line_count - 1))
            echo sed -i "${j}d" $i
            sed -i "${j}d" $i
        fi
    done
done
