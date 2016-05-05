#!/bin/sh

cd /cluster2/test/

COUNT=9999

mkdir_test()
{
    echo "mkdir newdir"
    for ((i=0; i<=${COUNT}; i++))
    do
    {
        mkdir newdir${i}
        cd newdir${i}
        file_test
        symlink_test
        link_test
        cd /cluster2/test
    }
    done
}

file_test()
{
    echo "create newfile"
    for ((x=0; x<=${COUNT}; x++))
    do
    {
        echo "hello world!" >> newfile${x}
        cat newfile${x}
    }
    done
}

symlink_test()
{
    echo "symlink newfile"
    for ((y=0; y<=${COUNT}; y++))
    do
    {
        ln -s newfile${y} symfile${y}
        echo "one world one dream!" >> symfile${y}
        cat symfile${y}
    }
    done
}

link_test()
{
    echo "link newfile"
    for ((z=0; z<=${COUNT}; z++))
    do
    {
        ln newfile${z} linkfile${z}
        echo "Life!" >> linkfile${z}
        cat linkfile${z}
    }
    done
}

unlink_test()
{
    for ((m=0; m<=${COUNT}; m++))
    do
    {
        rm -f newfile${m}
        rm -f symfile${m}
        rm -f linkfile${m}
    }
    done
}

rmdir_test()
{
    for ((n=0; n<=${COUNT}; n++))
    do
    {
        rm -rf newdir${n}
    }
    done
}

mkdir_test
cd /cluster2/test
rmdir_test
