#!/usr/bin/sh

compare_data()
{
    work_dir=$1
    files=$2

    record_dir="../log/"
    mkdir $record_dir

    arr_kb=("4k" "8k" "16k" "32k" "64k" "128k" "256k" "512k" "1024k" "2048k" "4096k" "8192k" "16384k")
    cd $work_dir
    file_list=`ls -U *${files}*`

    for m in {0..12}; do
        record_file=${record_dir}${files}"_"${arr_kb[${m}]}".log"
        rm -f $record_file
        echo ${record_file} > $record_file

        for i in {0..89}; do
            data_arr[$i]=""
        done

        for i in $file_list; do
            if [[ $m == 0 ]]; then
                tmp_str=`cat $i | awk '{print $1}'`
            elif [[ $m == 1 ]]; then
                tmp_str=`cat $i | awk '{print $2}'`
            elif [[ $m == 2 ]]; then
                tmp_str=`cat $i | awk '{print $3}'`
            elif [[ $m == 3 ]]; then
                tmp_str=`cat $i | awk '{print $4}'`
            elif [[ $m == 4 ]]; then
                tmp_str=`cat $i | awk '{print $5}'`
            elif [[ $m == 5 ]]; then
                tmp_str=`cat $i | awk '{print $6}'`
            elif [[ $m == 6 ]]; then
                tmp_str=`cat $i | awk '{print $7}'`
            elif [[ $m == 7 ]]; then
                tmp_str=`cat $i | awk '{print $8}'`
            elif [[ $m == 8 ]]; then
                tmp_str=`cat $i | awk '{print $9}'`
            elif [[ $m == 9 ]]; then
                tmp_str=`cat $i | awk '{print $10}'`
            elif [[ $m == 10 ]]; then
                tmp_str=`cat $i | awk '{print $11}'`
            elif [[ $m == 11 ]]; then
                tmp_str=`cat $i | awk '{print $12}'`
            elif [[ $m == 12 ]]; then
                tmp_str=`cat $i | awk '{print $13}'`
            fi

            n=0
            for j in $tmp_str; do
                if [[ $n == 0 ]]; then
                    tmp_str2=`echo $i|awk -F"[@ _]" '{print $1}'`
                    tmp_str2=${data_arr[${n}]}' '${tmp_str2}"_"${j}
                else
                    tmp_str2=${data_arr[${n}]}' '${j}
                fi

                data_arr[$n]=$tmp_str2
                n=$((n + 1))
            done
        done

        for i in {0..89}; do
            tmp_str=${data_arr[$i]}
            n=1
            for j in $tmp_str; do
                if [[ $n == 2 ]]; then
                    printf "%-8s\n" $j >> $record_file
                else
                    printf "%-8s " $j >> $record_file
                fi
                n=$((n + 1))
            done
        done
    done

    cd -
}

init_write()
{
    files="init_write" 
    compare_data $1 $files 
}

re_write()
{
    files="re_write" 
    compare_data $1 $files 
}

init_read()
{
    files="init_read" 
    compare_data $1 $files 
}

re_read()
{
    files="re_read" 
    compare_data $1 $files 
}

if [[ $1 == "" ]]; then
    printf "Please input the work directory!\n"
    exit 0
else
    work_dir=$1
    echo $work_dir
    init_write $work_dir
    re_write $work_dir
    init_read $work_dir
    re_read $work_dir
fi

