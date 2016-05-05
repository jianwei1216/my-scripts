#!/bin/sh

DEPLOY_HOST_NAME="node-"
DEPLOY_NODE_BEGIN=1
DEPLOY_NODE_COUNT=4
CONFIG_INSTALL="./autogen.sh; ./configure --build=x86_64-unknown-linux-gnu --host=x86_64-unknown-linux-gnu --target=x86_64-redhat-linux-gnu --program-prefix= --prefix=/usr --exec-prefix=/usr --bindir=/usr/bin --sbindir=/usr/sbin --sysconfdir=/etc --datadir=/usr/share --includedir=/usr/include --libdir=/usr/lib64 --libexecdir=/usr/libexec --localstatedir=/var --sharedstatedir=/var/lib --mandir=/usr/share/man --infodir=/usr/share/info --enable-systemtap=no --enable-bd-xlator --enable-debug; make; make install; sleep 4; service glusterd restart"
BRICKS="node-2:/mnt/disk2/brick node-3:/mnt/disk3/brick node-4:/mnt/disk4/brick"

GIT_URL="10.10.88.88:/root/git/multi_ec_config/glusterfs/"

GIT_PATH="/root/git"

deploy_make_install()
{
    for((i = DEPLOY_NODE_BEGIN; i <= DEPLOY_NODE_COUNT; i++)); do
    {
        echo ========ssh ${DEPLOY_HOST_NAME}${i}========    
        ssh ${DEPLOY_HOST_NAME}${i} "cd /root/git/glusterfs; ${CONFIG_INSTALL}"
    }&
    done
}

deploy_git_clone()
{
    for((i = DEPLOY_NODE_BEGIN; i <= DEPLOY_NODE_COUNT; i++)); do
    {
        echo ========ssh ${DEPLOY_HOST_NAME}${i}========    
        ssh ${DEPLOY_HOST_NAME}${i} "if [ ! -d ${GIT_PATH} ]; then mkdir ${GIT_PATH}; fi; cd ${GIT_PATH}; git clone ${GIT_URL}"
    }
    done
}

deploy_git_pull()
{
    for((i = DEPLOY_NODE_BEGIN; i <= DEPLOY_NODE_COUNT; i++)); do
    {
        echo ========ssh ${DEPLOY_HOST_NAME}${i}========    
        ssh ${DEPLOY_HOST_NAME}${i} "cd ${GIT_PATH}; git pull; make -j8;make install; sleep 4; service glusterd restart"
    }
    done
}


deploy_mkfs_bricks()
{
    gluster peer probe node-2
    gluster peer probe node-3
    gluster peer probe node-4
    
    for((i = DEPLOY_NODE_BEGIN; i <= DEPLOY_NODE_COUNT; i++)); do
    {
        echo ========ssh ${DEPLOY_HOST_NAME}${i}========    
        ssh ${DEPLOY_HOST_NAME}${i} 'for n in `find /mnt/ -name brick`; do rm -rf ${n};done'
        ssh ${DEPLOY_HOST_NAME}${i} 'for m in `find /mnt/ -name disk*`; do mkdir -p ${m}/brick;done'
    }
    done
}

deploy_ec_volume()
{
    deploy_mkfs_bricks

    gluster volume create test disperse 3 disperse-data 2 redundancy 1 ${BRICKS} force

    gluster volume start test force
}

deploy_ec_expand()
{
    read -p "Please enter the bricks count:" ALL_BRICKS_COUNT
    read -p "Please enter the bricks's host name:" ADD_HOST_NAME
    read -p "Please enter the add brick path:" ADD_BRICK_PATH

    ssh ${ADD_HOST_NAME} "rm -rf ${ADD_BRICK_PATH}; mkdir -p ${ADD_BRICK_PATH}"
    
    gluster volume add-brick test disperse ${ALL_BRICKS_COUNT} redundancy 1 ${ADD_HOST_NAME}:${ADD_BRICK_PATH}
    gluster volume info
}

case $1 in
clone)
    deploy_git_clone
    wait && deploy_make_install
    ;;
pull)
    deploy_git_pull
    ;;
ec)
    deploy_ec_volume
    ;;
expand)
    deploy_ec_expand
    ;;
*)
    echo "Usage: $0 [option]"
    printf "[options]
    1. clone
    2. pull
    3. ec
    4. expand
    example: $0 ec\n"
    ;;
esac

exit $?
