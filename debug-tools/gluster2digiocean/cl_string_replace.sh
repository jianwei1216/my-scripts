#!/bin/bash

GlfsString=(gluster GLUSTER Gluster glfs libgfapi)
DlfsString=(digiocean DIGIOCEAN Digiocean dofs libdfapi)

GlfsFileName=(glfs gluster)
DlfsFileName=(dofs digiocean)

help() {
    echo "    Usage: cl_string_replace.sh [source_path] --[option]"
    echo "    option:[gtod|dtog]"
    echo "    --gtod: put the gluster replace digiocean"
    echo "    --dtog: put the digiocean replace gluster"
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

if [ $# -ne 2 ];then
    help
    exit 1
fi

if [ $2 = "--gtod" ];then
    cd $1
    str=`pwd`
    str2=`echo ${str##*/}`
    cd ../
    rm -rf have-changed
    mkdir have-changed
    cp $str2 have-changed -a
    cd have-changed
    cd $str2
    finddirectory=`pwd`
    cd ../
    sed -i 's/strncmp (base, "glusterfsd", 10)/strncmp (base, "digioceanfsd", 12)/g' `grep "strncmp (base, \"glusterfsd\", 10)" -rl $finddirectory`
    sed -i 's/strncmp (base, "glusterd", 8)/strncmp (base, "digioceand", 10)/g' `grep "strncmp (base, \"glusterd\", 8)" -rl $finddirectory`
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
elif [ $2 = "--dtog" ];then
    cd $1
    str=`pwd`
    str2=`echo ${str##*/}`
    cd ../
    rm -rf have-changed
    mkdir have-changed
    cp $str2 have-changed -a
    cd have-changed
    cd $str2
    finddirectory=`pwd`
    cd ../
    sed -i 's/strncmp (base, "digioceanfsd", 12)/strncmp (base, "glusterfsd", 10)/g' `grep "strncmp (base, \"digioceanfsd\", 12)" -rl $finddirectory`
    sed -i 's/strncmp (base, "digioceand", 10)/strncmp (base, "glusterd", 8)/g' `grep "strncmp (base, \"digioceand\", 10)" -rl $finddirectory`
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
else
    help
    exit 1
fi 
