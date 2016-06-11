#!/bin/sh

for i in `ps aux|grep -v grep|grep digiocean|grep -v python|grep -v mdadm|awk '{print $2}'`; do
    kill -USR1 $i
done
