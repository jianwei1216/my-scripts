#!/usr/bin/sh

unix2dos_and_rename()
{
    work_dir=$1
    echo $work_dir
    cd $work_dir
    for i in `ls -U .`; do
        tmp_file_names+=$i
        tmp_file_names+=' '
        tmp+=`echo $i|awk -F"[@ _]" '{print $3}'|awk -F"[@ .]" '{print $1}'`
        tmp+=' '
    done

    for i in $tmp_file_names; do
        #dos2unix $i
        unix2dos $i
    done

    j=0
    for i in $tmp; do
        arr[$j]=$i
        j=$((j+1))
    done

    for i in {0..12}; do
        #echo $i
        echo ${arr[${i}]}
    done

    newname_prefix=$2
    j=0
    for i in $tmp_file_names; do
        newfilename=$newname_prefix${arr[$j]}.log
        sed -i "1i$newfilename" $i
        mv $i $newfilename
        j=$((j + 1))
    done
}

if [[ $1 == "" ]]; then
    echo "Please input the work directory!"
    exit 0
else
    work_dir=$1
    newname_prefix=$2
    unix2dos_and_rename $work_dir $newname_prefix
fi
