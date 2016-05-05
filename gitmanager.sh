#!/bin/sh
SERVERD="glusterd"
CMD="gluster"
CLIENT="glusterfs"
BRICKFSD="glusterfsd"
LOGPATH="/var/log/${CLIENT}"
LIBPATH="/var/lib/${SERVERD}"
# svn {{{
#OLD_SVN_PATH="/root/svn/glusterfs-ec-expand-master"
#NEW_SVN_PATH="/root/svn/glusterfs-ec-expand-rename"
#SVN_URL="svn://10.10.105.108/glusterfs-ec-expand-rename ${SVN_USER_PASSWORD} --no-auth-cache"
#SVN_USER_PASSWORD="--username zjw --password zjw"
#SVN_UPDATE="svn update --no-auth-cache ${SVN_USER_PASSWORD}"
#}}}
# git {{{
OLD_GIT_PATH="/root/git/gluster-ec-expand"
NEW_GIT_PATH="/root/git/glusterfs"
GIT_URL="ssh://10.10.103.194/root/git/glusterfs"
GIT_UPDATE="git pull"
#}}}
MOUNT_POINT="/cluster2/test/"
CONFIGURE="./autogen.sh; ./configure --build=x86_64-unknown-linux-gnu --host=x86_64-unknown-linux-gnu --target=x86_64-redhat-linux-gnu --program-prefix= --prefix=/usr --exec-prefix=/usr --bindir=/usr/bin --sbindir=/usr/sbin --sysconfdir=/etc --datadir=/usr/share --includedir=/usr/include --libdir=/usr/lib64 --libexecdir=/usr/libexec --localstatedir=/var --sharedstatedir=/var/lib --mandir=/usr/share/man --infodir=/usr/share/info --enable-systemtap=no --enable-bd-xlator --enable-debug;"
LOCALHOSTNAME="owner"
HOSTNUMBER=5

# mount glusterfs
deploy_clients()
{
    for i in 1 2 3 4 5; do
    {
		echo =======${HOSTNAME}${i}=======	
		ssh ${HOSTNAME}${i} "mount.${CLIENT} 127.0.0.1:/test /cluster2/test; wait && df -h"
    }&
    done
}

local_ec()
{
    ssh ${LOCALHOSTNAME} "service ${SERVERD} stop; killall ${CLIENT} ${BRICKFSD} ${SERVERD}; cd ${NEW_GIT_PATH}; make uninstall; rm -rf ${LOGPATH} ${LIBPATH}; make -j8; make install; service ${SERVERD} restart"
    wait && ./cluster.sh ec
}

local_dht()
{
    ssh ${LOCALHOSTNAME} "service ${SERVERD} stop; killall ${CLIENT} ${BRICKFSD} ${SERVERD}; cd ${NEW_GIT_PATH}; make uninstall; rm -rf ${LOGPATH} ${LIBPATH}; make -j8; make install; service ${SERVERD} restart"
    wait && ./cluster.sh dht
}

local_afr()
{
    ssh ${LOCALHOSTNAME} "service ${SERVERD} stop; killall ${CLIENT} ${BRICKFSD} ${SERVERD}; cd ${NEW_GIT_PATH}; make uninstall; rm -rf ${LOGPATH} ${LIBPATH}; make -j8; make install; service ${SERVERD} restart"
    wait && ./cluster.sh afr
}

deploy_ec()
{
	for ((i = 1; i <= ${HOSTNUMBER}; i++));do
	{
                echo ""
		echo =======${HOSTNAME}${i}=======	
		ssh ${HOSTNAME}${i} "umount ${MOUNT_POINT}; service ${SERVERD} stop; cd ${NEW_GIT_PATH}; ${GIT_UPDATE}; make uninstall; make; rm -rf ${LIBPATH} ${LOGPATH}; make install; service ${SERVERD} start"
	}&
	done	
    wait && ./cluster.sh ec deploy
}

deploy_dht()
{
	for ((i = 1; i <= ${HOSTNUMBER}; i++));do
	{
                echo ""
		echo =======${HOSTNAME}${i}=======	
		ssh ${HOSTNAME}${i} "umount ${MOUNT_POINT}; service ${SERVERD} stop; cd ${NEW_GIT_PATH}; ${GIT_UPDATE}; make; rm -rf ${LOGPATH} ${LIBPATH}; make install; service ${SERVERD} start"
	}&
	done	
    wait && ./cluster.sh dht deploy
}

deploy_afr()
{
	for ((i = 1; i <= ${HOSTNUMBER}; i++));do
	{
                echo ""
		echo =======${HOSTNAME}${i}=======	
		ssh ${HOSTNAME}${i} "umount ${MOUNT_POINT}; service ${SERVERD} stop; cd ${NEW_GIT_PATH}; ${GIT_UPDATE}; make uninstall; make; rm -rf ${LOGPATH} ${LIBPATH}; make install; service ${SERVERD} start"
	}&
	done	
    wait && ./cluster.sh afr deploy
}

# install software
deploy_install_software()
{
    for i in 1 2 3 4 ${HOSTNUMBER}; do
    {
        ssh ${HOSTNAME}${i} "yum install -y ntp autoconf autoheader automake libtool libxml2-devel python-devel libacl-devel lvm2-devel flex bison openssl-devel aclocal lsb_release openssl sqlite-devel svn git vim make gcc gcc-c++"
    }&
    done
}

# remove test volume on localhost
local_remove_old_svn()
{
    ssh ${LOCALHOSTNAME} "cd ${OLD_GIT_PATH}; make uninstall; make clean;cd -; rm -rf ${OLD_GIT_PATH} ${LOGPATH} ${LIBPATH}; cd /root/svn/; git clone ${SVN_URL}; cd ${NEW_GIT_PATH}; ${CONFIGURE} make -j8; make install; service ${SERVERD} restart"
}

# remove svn
deploy_remove_old_git ()
{
    for i in 1 2 3 4 ${HOSTNUMBER};do
    {
        echo =========${HOSTNAME}${i}========= 
        ssh ${HOSTNAME}${i} "killall ${CLIENT} ${SERVERD} ${BRICKFSD};cd ${OLD_GIT_PATH}; make uninstall; make clean;cd -; rm -rf ${OLD_GIT_PATH}; cd /root/git/; git clone ${GIT_URL}; cd ${NEW_GIT_PATH}; ${CONFIGURE}"
    }&
    done
}

# scp log file
deploy_scp_log_file ()
{
    rm -rf /root/work/tmp/tmp/* 
    work_dir=`date +"%s-%T"`
    for i in 1 2 3 4 5; do
    {
	echo =======${HOSTNAME}${i}=======	
        ssh ${LOCALHOSTNAME} "mkdir -p /root/work/bugs/ec-expand/${work_dir}"
		ssh ${HOSTNAME}${i} "scp -r ${HOSTNAME}${i}:/var/log/${CLIENT}/ 10.10.103.194:/root/work/bugs/ec-expand/${work_dir}/${CLIENT}.${HOSTNAME}${i}"
		ssh ${HOSTNAME}${i} "scp -r ${HOSTNAME}${i}:/var/lib/${SERVERD} 10.10.103.194:/root/work/bugs/ec-expand/${work_dir}/${SERVERD}.${HOSTNAME}${i}"
    }&
    done
}

# reboot
deploy_reboot_hosts ()
{
        if [[ $HOSTNAME == "owner" ]];then
                reboot
        else
        {
                for ((i=5; i>=1; i--));do
                {
                    echo =======${HOSTNAME}${i}====== 
                    ssh ${HOSTNAME}${i} "reboot"
                }&
                done
        }
        fi
}

#restart node-manager
deploy_node_manager ()
{
	for ((i=1; i<=${HOSTNUMBER}; i++)); do
		echo =====ssh ${HOSTNAME}${i}=======
		ssh ${HOSTNAME}${i} "node-manager clear; node-manager start"	
		echo ""
		echo ""
	done
}

#ntp time sync
function deploy_ntp_sync ()
{
	for ((i = 1; i <= ${HOSTNUMBER}; i++));do
		if [[ $i == 1 ]];then
			continue
		fi 
		echo ========ssh ${HOSTNAME}${i}=======
		ssh  ${HOSTNAME}${i} "service ntpd stop; ntpdate 10.10.21.90"
		echo ""
		echo ""
	done
}

# umount /mnt/disk
deploy_umount_disks()
{
	for ((i = 1; i <= ${HOSTNUMBER}; i++));do
		echo ========ssh ${HOSTNAME}${i}=======
                m=$[5+${i}]
                echo $m
                ssh  ${HOSTNAME}${i} 'for q in `find /mnt -name "disk*"`;do rm -rf ${q};done'
                ssh  ${HOSTNAME}${i} "mkdir -p /mnt/disk${i} /mnt/disk${m}"
		echo ""
		echo ""
	done
}

if [[ $HOSTNAME == "owner" ]];then
        echo "HOSTNAME is $HOSTNAME"
else
        HOSTNAME="server"
        echo "HOSTNAME is $HOSTNAME"
fi

#server arguments
case $1 in
node-manager)
        deploy_node_manager
        ;;
ntp)
        deploy_ntp_sync
        ;;
reboot)
        deploy_reboot_hosts
        ;;
install)
        deploy_install_software
        ;;
logs)
        deploy_scp_log_file
        ;;
deploy-old)
        deploy_remove_old_git
        ;;
deploy-dht)
        deploy_dht
        ;;
deploy-ec)
        deploy_ec
        ;;
mounts)
        deploy_clients
        ;;
local-old)
        local_remove_old_svn
        ;;
deploy-umount-disks)
        deploy_umount_disks
        ;;
ec)
        local_ec
        ;;
dht)
        local_dht
        ;;
afr)
        local_afr
        ;;
*)
        echo -e "Usage: $0 [option]"
        printf "[options]
       1. node-manager 
       2. mounts
       3. deploy-dht
       4. deploy-ec
       5. install 
       6. ntp
       7. reboot 
       8. local-old
       9. deploy-old 
       10. logs 
       11. ec
       12. dht
       13. afr
       14. deploy-umount-disks\n"

        ;;
esac

exit $?

