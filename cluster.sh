#!/bin/sh
DEPLOY_FLAG=""
#CLIENT="glusterfs"
CLIENT="digioceanfs"
#SERVERD="glusterd"
SERVERD="digioceand"
#BRICKFSD="glusterfsd"
BRICKFSD="digioceanfsd"
#CMD="gluster"
CMD="digiocean"
MOUNTDIR="/cluster2/test"
BRICKS=""
#BRICKS="worker:/mnt/disk1/brick worker:/mnt/disk2/brick worker:/mnt/disk3/brick"
#ADD_BRICKS="worker:/mnt/disk4/brick worker:/mnt/disk5/brick worker:/mnt/disk6/brick"
#BRICKS="10.10.21.90:/mnt/disk1/brick 10.10.21.90:/mnt/disk2/brick 10.10.21.91:/mnt/disk1/brick 10.10.21.91:/mnt/disk2/brick 10.10.21.92:/mnt/disk1/brick 10.10.21.92:/mnt/disk2/brick 10.10.21.93:/mnt/disk1/brick 10.10.21.93:/mnt/disk2/brick"
#BRICKS="10.10.21.90:/mnt/disk1/brick 10.10.21.91:/mnt/disk3/brick 10.10.21.92:/mnt/disk5/brick"
LOCALHOSTNAME="worker"

#start volume
start_test()
{
    ${CMD} volume start test
}

#mkfs
mkfs ()
{
        echo "rm -rf brick"
        rm -rf `find /mnt/ -name brick`

        for m in `find /mnt/ -name disk*`; do
            echo "mkdir ${m}/brick"
            mkdir ${m}/brick
        done
}

#deploy_mkfs
deploy_mkfs ()
{
    ${CMD} peer probe ${HOSTNAME}2 
    ${CMD} peer probe ${HOSTNAME}3
    ${CMD} peer probe ${HOSTNAME}4 
    ${CMD} peer probe ${HOSTNAME}5

    for ((i = 1; i <= 5; i++));do
        { 
            echo ==========ssh ${HOSTNAME}${i}===========
            ssh ${HOSTNAME}${i} 'for n in `find /mnt/ -name brick`; do rm -rf ${n};done'
            ssh ${HOSTNAME}${i} 'for m in `find /mnt/ -name disk*`; do mkdir -p ${m}/brick;done'
        }
    done
}

#dht
dht ()
{
        echo "---dht: mkfs---"
        if [[ ${DEPLOY_FLAG} == "deploy" ]];then
            deploy_mkfs
        else
            mkfs
        fi
        echo "---dht: create---"
        ${CMD} volume create test ${BRICKS} force
        echo "---dht: start---"
        start_test
}

#afr
afr ()
{
        echo "---afr: mkfs---"
        if [[ ${DEPLOY_FLAG} == "deploy" ]];then
            deploy_mkfs
        else
            mkfs
        fi
        echo "---afr: create---"
        ${CMD} volume create test replica 4 ${BRICKS} force
        echo "---afr: start---"
        start_test
}


#ec
ec ()
{
        echo "---ec: mkfs---"
        if [[ ${DEPLOY_FLAG} == "deploy" ]];then
            deploy_mkfs
        else
            mkfs
        fi
        echo "---ec: create (2+1)---"
        #${CMD} volume create test disperse 3 disperse-data 2 redundancy 1 ${BRICKS} force
        echo ${CMD} volume create test disperse 3 disperse-data 2 redundancy 1 ${BRICKS} force
        echo "---ec: start---"
        start_test
}

#expand ec
expand_ec()
{
    host_number=""
    read -p "Please enter the add brick number:" brick_number
    if [[ $HOSTNAME == "worker" ]];then
        BRICK_HOST_NAME="worker"
    else
        BRICK_HOST_NAME="server"
        read -p "Please enter the brick's host number:" host_number
        ssh ${BRICK_HOST_NAME}${brick_number} "for i in `find /mnt/ -name disk${brick_number}`; do {rm -rf ${i}/brick; mkdir ${brick}/brick};done"
    fi
    echo "mkfs add brick"
    for i in `find /mnt/ -name disk${brick_number}`; do mkdir ${i}/brick;done
    echo "expand ec volume"
    ${CMD} volume add-brick test disperse ${brick_number} redundancy 1 ${BRICK_HOST_NAME}${host_number}:/mnt/disk${brick_number}/brick
    ${CMD} volume info
}

#expand ec
remove_ec()
{
    echo "expand ec volume"
    ${CMD} volume remove-brick test ${ADD_BRICKS} start
    ${CMD} volume remove-brick test ${ADD_BRICKS} commit
    ${CMD} volume info
}

replace_disk ()
{
        for ((i = 0; i < 10; i++));do
                echo "$i $i $i $i $i $i $i $i $i $i"
                fuser -kvm /mnt/disk1
                ssh server3 "rm -rf /mnt/disk1/newbrick; mkdir /mnt/disk1/newbrick"
                digiocean volume replace-brick test 10.10.21.92:/mnt/disk1/brick 10.10.21.93:/mnt/disk1/newbrick commit force
                digiocean volume status
                read -p "Enter continue" $v1
                ssh server3 "fuser -kvm /mnt/disk1"
                digiocean volume replace-brick test 10.10.21.93:/mnt/disk1/newbrick 10.10.21.92:/mnt/disk1/brick commit force
                digiocean volume status
                read -p "Enter continue" $v2
                echo ""
                echo ""
        done
}
# deploy or local
if [[ $2 == "deploy" ]];then
    HOSTNAME="server"
    DEPLOY_FLAG=$2
    BRICKS="server1:/mnt/disk1/brick server2:/mnt/disk2/brick server3:/mnt/disk3/brick server4:/mnt/disk4/brick server5:/mnt/disk5/brick server1:/mnt/disk6/brick server2:/mnt/disk7/brick server3:/mnt/disk8/brick server4:/mnt/disk9/brick server5:/mnt/disk10/brick"
    echo BRICKS="server1:/mnt/disk1/brick server2:/mnt/disk2/brick server3:/mnt/disk3/brick server4:/mnt/disk4/brick server5:/mnt/disk5/brick server1:/mnt/disk6/brick server2:/mnt/disk7/brick server3:/mnt/disk8/brick server4:/mnt/disk9/brick server5:/mnt/disk10/brick"
else
    BRICKS="worker:/mnt/disk1/brick worker:/mnt/disk2/brick worker:/mnt/disk3/brick"
    #worker:/mnt/disk4/brick worker:/mnt/disk5/brick worker:/mnt/disk6/brick"
fi

#server arguments
case $1 in
dht)
    dht
    ;;
afr)
    afr
    ;;
ec)
    ec
    ;;
expand-ec)
    expand_ec
    ;;
remove-ec)
    remove_ec
    ;;
*)
        echo $"Usage: $0 [option]"
        printf "[options]
        1. dht
        2. afr
        3. ec
        4. expand-ec: ec(2+1) --> add 1 brick --> ec(3+1).
        5. remove-ec
        notes: if input \$2==deploy, indicating more than one hosts.
        example: $0 dht deploy or not input \$2, indicating only one host
        example: $0 dht\n"
        ;;
esac

exit $?
