#!/usr/bin/sh

work_dir=""
results_dir=""
results_file=""

pub_func ()
{
	echo -e "$1\t$2\t$3\t$4"
	flag_str=$1
	begin_line=$2
	end_line=$3
	results_file=$4
    echo "$begin_line", "$end_line"
	
	echo "$flag_str" >> $results_file

    for i in {0..89}; do
        arr[${i}]=""
    done

	tmp=`ls -U .`
    r=0
    head_str=""
    title_str=""
	for i in $tmp; do
        j=0
        if [[ $r == 0 ]]; then
            title_str=$i
        fi
        tmp_str=`echo $i|awk -F"[@ _]" '{print $13}' | awk -F"[@ .]" '{print $1}'`
        head_str=$head_str"    "$tmp_str
        tmp_str1=`sed -n "${begin_line}, ${end_line}p" $i | awk '{print $9}'`
        n=0
        for m in $tmp_str1; do
            arr[${j}]=${arr[${j}]}' '$m
            j=$((j + 1))
            n=$((n + 1))
        done
        echo "n $n"
        r=$((r + 1))
	done

    echo $j, $r "DEBUG J R"
    echo $head_str

    echo "$title_str" >> $results_file

    r=1
    for i in $head_str; do
        if [[ $r == 13 ]]; then
            printf "%-8s\n" $i >> $results_file
        else
            printf "%-8s " $i >> $results_file
        fi
        r=$((r + 1))
    done

    for i in {0..89}; do
        r=1
        for j in ${arr[$i]}; do
            if [[ $r == 13 ]];then
                printf "%-8s\n" $j >> $results_file
            else
                printf "%-8s " $j >> $results_file
            fi
            r=$((r + 1))
        done
    done
}

init_write()
{
	results_file=$results_dir/results-init-write.txt
	begin_line=36
	end_line=$(($begin_line + 90 - 1))	
	pub_func "INIT_WRITE" $begin_line $end_line $results_file
}

re_write()
{
	results_file=$results_dir/results-re-write.txt
	begin_line=133
	end_line=$(($begin_line + 90 - 1))	
	pub_func "RE-WRITE" $begin_line $end_line $results_file
}

init_read()
{
	results_file=$results_dir/results-init-read.txt
	begin_line=230
	end_line=$(($begin_line + 90 - 1))
	pub_func "INIT-READ" $begin_line $end_line $results_file
}

re_read()
{
	results_file=$results_dir/results-re-read.txt
	begin_line=327
	end_line=$(($begin_line + 90 - 1))	
	pub_func "RE-READ" $begin_line $end_line $results_file
}

if [[ $1 == "" ]]; then
	echo "Please input the file path"
	exit 0
else
	work_dir=$1
	results_dir=`pwd`

	results_init_write_file="$results_dir/results-init-write.txt"
	results_re_write_file="$results_dir/results-re-write.txt"
	results_init_read_file="$results_dir/results-init-read.txt"
	results_re_read_file="$results_dir/results-re-read.txt"
    rm -f $results_init_write_file $results_re_write_file $results_init_read_file $results_re_read_file

	echo "The work dir is $work_dir"
	echo "The results file is $results_file"
	cd $work_dir

	init_write
    re_write
    init_read
    re_read
fi

