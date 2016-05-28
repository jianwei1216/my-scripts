#!/usr/bin/bash

flags="mknod"
save_file="${flags}.dht_do_force.results"

for i in `find ./ -name "${flags}*" | grep "${flags}*"| grep -v calc.sh | grep -v $save_file`; do 
    echo $i
    /root/git/my-scripts/python_script/misc.py --calc-avg $i >> ${save_file}.tmp
done

cat ${save_file}.tmp | sort -u >  $save_file
rm -f ${save_file}.tmp
