#!/bin/sh

newfile0_gfid='path=/newfile0|gfid=437aa661-ef09-4522-be7a-5068dcc1c7ae|ino=13725371225675515822'
newfile1_gfid='path=/newfile1|gfid=05a4abde-bc34-47af-b8fd-76d4c2da2aa7|ino=13329941128260758183'
newfile0_tmp_gfid='path=/newfile0.tmp|gfid=bcd26a46-3799-44a8-9ed4-54ea33446766|ino=11444865917914474342'
newfile1_tmp_gfid='path=/newfile1.tmp|gfid=8e8d32c7-a8d9-4e9a-8117-6ca8e001fdfc|ino=9302023027925581308'

client_log='cluster2-test.log'
server_log="mnt-*"
log=${client_log}

filter_newfile0 ()
{
    cat $log | grep -E $newfile0_gfid | grep -v -E "path=/newfile0.tmp" > newfile0.${log}
}

filter_newfile1 ()
{
    cat $log | grep -E $newfile1_gfid | grep -v -E "path=/newfile1.tmp" > newfile1.${log}
}

filter_newfile0_tmp ()
{
    cat $log | grep -E $newfile0_tmp_gfid > newfile0.tmp.${log}
}

filter_newfile1_tmp ()
{
    cat $log | grep -E $newfile1_tmp_gfid > newfile1.tmp.${log}
}

filter_non_trace ()
{
    cat $log | grep -v -E 'trace' > non_trace.${log}
}

filter_trace ()
{
    cat $log | grep -E 'trace' > trace.${log}
}

filter_cluster2_test_log ()
{
    for ((i=0; i<4; i++));do
    {
        echo ---glusterfs.10.10.21.9${i}---
        cd glusterfs.10.10.21.9${i}
        filter_newfile0
        filter_newfile1
        filter_newfile0_tmp
        filter_newfile1_tmp
        filter_non_trace
        filter_trace
        cd -
    }
    done
}

filter_bricks_mnt_log ()
{
    for ((i=0; i<3; i++));do
    {
        echo ---glusterfs.10.10.21.9${i}/bricks/---
        cd glusterfs.10.10.21.9${i}/bricks/
        filter_newfile0
        filter_newfile1
        filter_newfile0_tmp
        filter_newfile1_tmp
        filter_non_trace
        filter_trace
        cd -
    }
    done
}

case $1 in 
client)
    log=$client_log
    filter_cluster2_test_log
    ;;
server)
    log=$server_log
    filter_bricks_mnt_log
    ;;
*)
    echo 'Usage: $0 [client|server]'
    ;;
esac
