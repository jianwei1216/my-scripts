#!/bin/bash

WORK_DIR=""
# ALL_SIZE = WRITE_CNT * one_write(10G)
WRITE_CNT=4000

write_day()
{
        #clear
        echo bash.exe rm -rf /$HOSTNAME/*
        bash.exe rm -rf /$HOSTNAME/*

        for (( i = 1; i <= 4; i++ )); do
                #mkdir
                bash.exe mkdir -p /$HOSTNAME/day$i
                WORK_DIR="/$HOSTNAME/day$i"

                #write
                echo bash.exe windows_write_file.sh $WORK_DIR $WRITE_CNT
                bash.exe windows_write_file.sh $WORK_DIR $WRITE_CNT

                #delete
                echo bash.exe windows_remove_file.sh $WORK_DIR $WRITE_CNT
                bash.exe windows_remove_file.sh $WORK_DIR $WRITE_CNT &
        done
}

write_day
