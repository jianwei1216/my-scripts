#!/bin/sh

#echo "Stop test server"
digiocean volume stop test

#echo "Delete old test server"
digiocean volume delete test

echo "Umount /cluster2/test"
umount /cluster2/test
killall digioceanfs

#echo "Delete log file"
#rm -rf /var/log/digioceanfs/cluster2-test* /core.* /cluster2/test/*

#echo "Make -j8; make install"
#cd /root/git/multi_ec_config/test/digioceanfs/xlators/cluster/ec/src
#make -j8; make install

#echo "Create new test server"
#cd /root/shell
#./cluster.sh ec

