#/bin/bash

read_count=1

# write
test_write()
{
    echo "--1--cd work directory"
    cd /cluster2/test/
    echo "--2--zero.dat is exist?"
    if test -e zero.dat
    then
        echo "--2.1--remove zero.dat"
        rm -f zero.dat
    fi
    echo "--3--create zero.dat"
    dd if=/dev/zero of=zero.dat bs=10240 count=100000
}

# read 
test_read()
{
    echo "--1--[READ] cd wrok directory"
    cd /cluster2/test
    echo "--2--[READ] read zero.dat"
    for ((i=0; i<$read_count; i++));do
    {
        echo "--3--process ${i} read zero.dat"
        cat zero.dat
    }
    done
}

#case 
case $1 in
write)
    test_write
    ;;
read)
    test_read
    ;;
*)
    echo "Usage: $0 [write|read]"
    ;;
esac
