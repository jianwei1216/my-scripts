#!/bin/sh

ls_while()
{
    while ((1))
    do
        ls -alR /cluster2/test
        ls -alR /cluster2/test | grep ^- | wc -l
        sleep 5
    done
}

ls_while
