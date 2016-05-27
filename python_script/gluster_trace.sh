#!/bin/bash

FILE="/var/lib/digioceand/vols/test/trusted-test.tcp-fuse.vol"

function add_trace_in_glusterfs()
{
        sed -i '150 a\volume test-trace' $FILE
        sed -i '151 a\    type debug/trace' $FILE
        sed -i '152 a\    option log-file yes' $FILE
        sed -i '153 a\    option log-history yes' $FILE
        sed -i '154 a\    subvolumes test-dht' $FILE
        sed -i '155 a\end-volume' $FILE
        sed -i '156 a\zhangjianwei' $FILE
        sed -i 's/zhangjianwei//g' $FILE
        sed -i '160 a\    subvolumes test-trace' $FILE
        sed -i '160d' $FILE
}

case $1 in
yes)
        add_trace_in_glusterfs
        ;;
*)
        echo "Usage: ./gluster_trace.sh yes"
        ;;
esac

