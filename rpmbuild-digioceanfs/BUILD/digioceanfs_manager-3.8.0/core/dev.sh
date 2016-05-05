#!/bin/bash

MANAGER_INSTALL_DIR=/usr/local/digioceanfs_manager

run_node_manager()
{
	cpwd=$(pwd)
	if [ ${cpwd} != ${MANAGER_INSTALL_DIR} ]
	then
		echo "pwd:"${cpwd}" not in "${MANAGER_INSTALL_DIR}", deploy manager first"
		deploy
	fi
	echo "running node manager"
	echo "make sure that digioceanfs_manager has been deployed!"
	pushd ${MANAGER_INSTALL_DIR}/manager
	python node_manager.pyc &
	popd
}

debug_node_manager()
{
	cpwd=$(pwd)
	if [ ${cpwd} != ${MANAGER_INSTALL_DIR} ]
	then
		echo "pwd:"${cpwd}" not in "${MANAGER_INSTALL_DIR}", deploy manager first"
		deploy
	fi
	echo "running node manager"
	echo "make sure that digioceanfs_manager has been deployed!"
	pushd ${MANAGER_INSTALL_DIR}/manager
	python node_manager.pyc -L Debug &
	popd
}

clean_node()
{
	echo "cleaning config files on this node"
	CLEAN_NODE_TMP_FILE=/etc/.tmp-host-`date +"%Y%m%d%I%M%S"`
    rm -rf /etc/digioceanfs/DOMAINNAME
    rm -rf /etc/digioceanfs/ipmi.conf
	rm -rf /etc/digioceanfs/services
	rm -rf /etc/digioceanfs/vols
	rm -rf /etc/digioceanfs/sms.conf
	rm -rf /var/log/digioceanfs_manager/server.log
	rm -rf /var/log/digioceanfs_manager/client.log
	rm -rf /etc/rsyslog.d/99-digioceanfs.conf 
	awk '{if ( $4 !~ "####DigioceanfsNode####") print $0 }' /etc/hosts > $CLEAN_NODE_TMP_FILE
	mv $CLEAN_NODE_TMP_FILE /etc/hosts
	awk '{if ( $4 !~ "####DigioceanfsClient####") print $0 }' /etc/hosts > $CLEAN_NODE_TMP_FILE
	mv $CLEAN_NODE_TMP_FILE /etc/hosts
	awk '{if ( $1 !~ "/dev/disk/by-id/wwn-") print $0 }' /etc/fstab > $CLEAN_NODE_TMP_FILE
	mv $CLEAN_NODE_TMP_FILE /etc/fstab
	awk -F'=' '{if ( $1 != "include") print $0 }' /etc/samba/smb.conf > $CLEAN_NODE_TMP_FILE
	mv $CLEAN_NODE_TMP_FILE /etc/samba/smb.conf
	rm -rf /etc/rsyslog.d/digioceanfs.conf
	cat /dev/null > /etc/exports
	cat /dev/null > /etc/digioceanfs/management_node
	/etc/init.d/rsyslog restart
	/etc/init.d/samba restart
	killall unfsd
	rm -f /usr/local/digioceanfs_backup/*
}

clean_manager()
{
	echo "cleaning config files on this manager"
	rm -rf /etc/digioceanfs_manager/services
	rm -rf /etc/digioceanfs_manager/nodes
	rm -rf /etc/digioceanfs_manager/groups
	rm -rf /var/log/digioceanfs_manager/server.log
	rm -rf /var/log/digioceanfs_manager/client.log
	rm -rf /etc/rsyslog.d/99-digioceanfs.conf 	
	rm -rf /etc/rsyslog.d/digioceanfs.conf
	/etc/init.d/rsyslog restart
	rm -f /usr/local/digioceanfs_backup/*	
}
deploy()
{
	echo "deploy digioceanfs_manager on this machine"
	cp -rf scripts/digi_partition.sh /usr/bin/digi_partition
	chmod +x /usr/bin/digi_partition
	cp -rf scripts/mkdigioceanfs /usr/bin/mkdigioceanfs
	chmod +x /usr/bin/mkdigioceanfs
	cp -rf scripts/disk_plug.pyc /usr/bin/disk_plug
	chmod +x /usr/bin/disk_plug
	cp -rf scripts/clear_xattrs.sh /usr/bin/clear_xattr
	chmod +x /usr/bin/clear_xattr
	cp -rf scripts/rc.digioceanfs /etc/
	cp -rf volgen/digioceanfs-volgen /usr/bin/digioceanfs-volgen
	chmod +x /usr/bin/digioceanfs-volgen
	cp -rf volgen/digioceanfs-afrgen /usr/bin/digioceanfs-afrgen
	chmod +x /usr/bin/digioceanfs-afrgen
	mkdir /usr/share/digioceanfs 2>/dev/null
	cp -rf volgen/CreateBooster.pyc /usr/share/digioceanfs/
	cp -rf volgen/CreateVolfile.pyc /usr/share/digioceanfs/
	cp -rf scripts/handle_hotplug.pyc /usr/bin/handle_hotplug.pyc
	chmod +x /usr/bin/handle_hotplug.pyc
	cp -rf scripts/investigation.sh /usr/bin/investigation
	chmod +x /usr/bin/investigation
	cp -rf scripts/raid.pyc /usr/bin/raid.pyc
	chmod +x /usr/bin/raid.pyc
	cp -rf scripts/backup_restore.pyc /usr/bin/backup_restore.pyc
	chmod +x /usr/bin/backup_restore.pyc
	cp -rf scripts/handle_raid_event.pyc /usr/bin/handle_raid_event.pyc
	chmod +x /usr/bin/handle_raid_event.pyc
	cp -rf scripts/60-blockdev-hotplug.rules /etc/udev/rules.d/60-blockdev-hotplug.rules
	mkdir /etc/mdadm/ 2>/dev/null
	cp -rf scripts/mdadm.conf /etc/mdadm/mdadm.conf
	
	mkdir ${MANAGER_INSTALL_DIR} 2>/dev/null
	cp * ${MANAGER_INSTALL_DIR} -rf
	sys_type=`head -n1 /etc/issue | awk '{print $1}'`
	if [[ $sys_type = "iVCS" || $sys_type = "CentOS" ]];then
		cp scripts/node-manager /etc/init.d/
        chmod +x /etc/init.d/node-manager
                chkconfig --add node-manager
                chkconfig --del digioceand
                chkconfig --level 235 node-manager on
        #update-rc.d node-manager defaults
        ln -s /etc/init.d/node-manager /usr/bin/
	else
		cp scripts/node-manager /etc/init.d/
        chmod +x /etc/init.d/node-manager
        update-rc.d node-manager defaults
        ln -s /etc/init.d/node-manager /usr/bin/
    fi
}
clean()
{
	find . | grep ".pyc$" | xargs rm
	find -name tags | xargs rm
}
stop_p()
{
	ps aux |grep node_manager |grep -v 'grep' |awk '{print $2}'|xargs kill -9
}

while test $#
do
	case $1 in
		--mount-node | -m )
			mount_nodes
			exit 0 ;;
		--re-deploy | -r )
			re_deploy
			exit 0 ;;
		--deploy | -d )
			deploy
			exit 0 ;;
		--clean-node | -cn )
			clean_node
			exit 0 ;;
		--clean-manager | -cm )
			clean_manager
			exit 0 ;;
		--run-node-manager | -n )
			run_node_manager
			exit 0 ;;
		--debug-node-manager )
			debug_node_manager
			exit 0 ;;
		--clean )
			clean
			exit 0 ;;
		--stop | -s)
		    stop_p	
			exit 0 ;;
                --restart )
		    stop_p
		    run_node_manager
		        exit 0 ;;
	        --restart-clean | -rc )
		    stop_p
		    clean_manager
		    clean_node
		    run_node_manager
		        exit 0 ;;
		-* )
			echo "invalid option $1"; exit 1 ;;
		* )
			echo "needs an option "; exit 1 ;;
	esac
done
