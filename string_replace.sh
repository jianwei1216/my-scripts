#!/bin/bash

GlfsString=(gluster GLUSTER Gluster glfs libgfapi)
DlfsString=(digiocean DIGIOCEAN Digiocean dofs libdfapi)

GlfsFileName=(glfs gluster)
DlfsFileName=(dofs digiocean)

help() {
    echo "    Usage: string_replace.sh [option]"
    echo "    option:[gtod|dtog]"
    echo "    gtod: put the gluster replace digiocean"
    echo "    dtog: put the digiocean replace gluster"
}

file_context_replace() {
    echo "|"$1,$2,$3
    sed -i "s/$2/$3/g" `grep $2 -rl $1`
}


file_name_replace() {
    echo "|"$1,$2,$3
    find $1 -name "*$2*"|tac|sed "s/\(.*\)\($2\)\(.*\)/mv \1\2\3 \1$3\3/"|sh
}

#file_name_replace /root/djc/digioceanfs-master digiocean gluster


#file_context_replace /root/djc/glusterfs-master  digiocean gluster

if [ $# -ne 1 ];then
    help
    exit 1
fi

if [ $1 = "gtod" ];then
    finddirectory=`ls|grep gluster`
    echo "---------------replace the file context---------------"
    echo "|"
    echo "|put the gluster replace digiocean begin"
    for index in `seq ${#GlfsString[@]}`;do
        index=`expr $index - 1`
        file_context_replace $finddirectory ${GlfsString[$index]} ${DlfsString[$index]}
    done
    echo "|put the gluster replace digiocean end"
    echo "|"
    echo "-----------------modify the file name-----------------"
    echo "|"
    echo "|"
    echo "|put the gluster replace digiocean begin"
    for index in `seq ${#GlfsFileName[@]}`;do
        index=`expr $index - 1`
        file_name_replace $finddirectory ${GlfsFileName[$index]} ${DlfsFileName[$index]}
    done
    echo "|put the gluster replace digiocean end"
    echo "|"
    echo "------------------------------------------------------"
else
    finddirectory=`ls|grep digiocean`
    echo "---------------replace the file context---------------"
    echo "|"
    echo "|put the digiocean replace gluster begin"
    for index in `seq ${#DlfsString[@]}`;do
        index=`expr $index - 1`
        file_context_replace $finddirectory ${DlfsString[$index]} ${GlfsString[$index]}
    done 
    echo "|put the digiocean replace gluster end"
    echo "|"
    echo "-----------------modify the file name-----------------"
    echo "|"
    echo "|"
    echo "|put the digiocean replace gluster begin"
    for index in `seq ${#DlfsFileName[@]}`;do
        index=`expr $index - 1`
        file_name_replace $finddirectory ${DlfsFileName[$index]} ${GlfsFileName[$index]}
    done 
    echo "|put the digiocean replace gluster end"
    echo "|"
    echo "------------------------------------------------------"
fi 
