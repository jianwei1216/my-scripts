#!/usr/bin/bash


function mkfs_xfs()
{
    #for i in b c d e f g h i j k l m n o p q; do 
    i=0
    for tmp in h i; do 
        i=$(($i+1))
        echo $tmp $i
        #./fast.py "mkfs.xfs -f /dev/sd$tmp"
        ./fast.py "mount /dev/sd${tmp} /mnt/disk${i}"
    done
}

function create_vol()
{
    gluster volume create test disperse 3 disperse-data 2 redundancy 1 10.10.21.11{1,2,3}:/mnt/disk1/ 10.10.21.11{1,2,3}:/mnt/disk2 force
    gluster volume start test
    ./fast.py 'mkdir -p /cluster2/test; mount.glusterfs 127.0.0.1:/test /cluster2/test'
}

create_vol
