#!/bin/bash

# [exit2.sh]

for ((i=1; i<=10; i++));do
    echo "$i"
    echo "$1"
    while [ 1 ]
    do
        echo "hello"
        sleep 1
    done

    if (($i >= 5)); then
        echo "$i"
        exit 0
    fi

done
