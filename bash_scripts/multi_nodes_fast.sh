#!/usr/bin/sh

giturl=""

fast_delete_service()
{
	fast rm -rf /data/*
	fast killall mongod
	fast umount /cluster2/test
	fast node-manager clear
	fast node-manager stop

	cluster uninstall

	fast rm -rf /root/git/digioceanfs

	mkdir /cluster2/test
}

fast_prepare_service()
{
	#fast node-manager stop
	#fast systemctl start digioceand
	#digiocean volume start test force
	fast mkdir /cluster2/test
    fast mkdir /cluster2/test2
    fast mkdir /cluster2/test3
    fast mkdir /cluster2/test4
	#fast mount.digioceanfs 127.0.0.1:/test /cluster2/test
    fast mount -t digioceanfs -o background-qlen=256,congestion-threshold=192 127.0.0.1:/test /cluster2/test
    fast mount -t digioceanfs -o background-qlen=256,congestion-threshold=192 127.0.0.1:/test /cluster2/test2
    fast mount -t digioceanfs -o background-qlen=256,congestion-threshold=192 127.0.0.1:/test /cluster2/test3
    fast mount -t digioceanfs -o background-qlen=256,congestion-threshold=192 127.0.0.1:/test /cluster2/test4
	mkdir /cluster2/test/test/
}

fast_create_service()
{
	cluster /root/git ${giturl} 10.10.21.11{1,2,3,4} clone
	cluster install
	fast systemctl daemon-reload
	fast systemctl restart digioceand
	fast node-manager stop
	fast killall mongod
	fast rm -rf /data/*
    #misc.py --build-mongodb --master-node 10.10.178.101 --slave-nodes 10.10.178.10{2,3}
	fast node-manager start
}

case $1 in
create)
	if [[ $2 == "" ]];then
		echo "please give the giturl!"
		echo
		exit
	fi
	giturl=$2
	fast_create_service
	;;
delete)
	fast_delete_service
	;;
prepare)
	fast_prepare_service
	;;
*)
	echo "Usage: $0 {create giturl|delete|prepare}"
	echo
	;;
esac
