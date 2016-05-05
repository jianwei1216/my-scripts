#!/bin/bash

function help (){
    echo "Usage: moncmd  OPTION"
    echo "You can input cmd like : a "
    echo "List information about the OPTION"
    echo
    echo " m    Monitor virtual memory of process,when the monitor is started,and you want stop it,you must manually,you can press \"Ctrl+C\"."
    echo " c    Collect the orignal data for other node.But first you need to set up ssh keys-free links."
    echo " a    Analyse the orignal data if the PID of process changed."
    echo " g    Put the data into images,but you have analyse the orignal data first."
    echo " r    Back up the test data,so you better running this command after running m;c;a;g command,to ensure that the backup data is up to date."
    echo " h    Help"
    echo " q    Quit"
    echo
}

function process_mem (){

    mem_data=$6

    if [ -z ${mem_data} ];then

        return

    fi

    fileName=/mnt/monitor_data/orignal_data/$2-$3.dat

    if [ -e ${fileName} ];then

        echo -e "$5\t" "$4\t" "$7\t\t" "$1\t\t" "\t$6">>/mnt/monitor_data/orignal_data/$2-$3.dat

    else

        echo -e "\t""\ttime\t\t" "pid\t\t" "timer\t\t" "$2-$3">>/mnt/monitor_data/orignal_data/$2-$3.dat
        echo -e "$5\t" "$4\t" "$7\t\t" "$1\t\t" "\t$6">>/mnt/monitor_data/orignal_data/$2-$3.dat

    fi
}

# Set up directory to storage monitor data and image.
pwd_path=`pwd`

cd /mnt

dir_name="monitor_data"
data_path="orignal_data"
analy_path="analy_data"
png_path="image"
record_path="record"

if [ -d ${dir_name} ];then
    echo "DIR monitor_data exist"
    cd $dir_name
else
    mkdir $dir_name
    cd $dir_name
fi

if [ -d ${data_path} ];then
    echo "DIR orignal_data exist"
else
    mkdir $data_path
fi

if [ -d ${analy_path} ];then
    echo "DIR analy_data exist"
else
    mkdir $analy_path
fi

if [ -d ${png_path} ];then
    echo "DIR image exist"
else
    mkdir $png_path
fi

if [ -d ${record_path} ];then
    echo "DIR record exist"
else
    mkdir $record_path
fi

help

while((1))
do
    echo "Please input you command:"
    read cmd

    case ${cmd} in

        "m")

            cd $pwd_path

            rm -rf /mnt/monitor_data/orignal_data/*.dat

            #获取数据端的ip

            host_name=`hostname`
            echo $host_name

            local_ip=`cat '/etc/hosts'|grep "####DigioceanfsNode####"|grep -w "$host_name"|awk '{print $1}'|grep -v '#'`
            echo "$local_ip"

            echo "monitor..."

            timer=0
            line=1

            #开始监控
            while((1))
            do
                #获取系统当前时间
                time=`date +%Y/%m/%d-%H:%M:%S`
                n=0

                digioceanfsd_name0=(`ps aux|grep -w digioceanfsd|grep -v disk_plug|grep -wv add|grep -v grep|awk '{print $21}'`)

                touch /mnt/monitor_data/monitor.txt
                #将获取的磁盘的名称
                for i in ${digioceanfsd_name0[@]};do

                    echo $i >> /mnt/monitor_data/monitor.txt
#                    sed -i "s/\//./g"  /mnt/monitor_data/monitor.txt

                done

                sed -i "s/\//./g"  /mnt/monitor_data/monitor.txt
                #将文件数据中的'/'替换为'-',如果不替换，系统会将其默认为目录
#                for j in ${digioceanfsd_name0[@]};do

#                    sed -i "s/\//./g"  /mnt/monitor_data/monitor.txt

#                done

                #解析出磁盘的uuid
#               sed -i "s/.digioceanfs.wwn//g" `grep .digioceanfs.wwn -rl /mnt/monitor_data/monitor.txt`

                #将替换后的字符串写入数组中
                digioceanfsd_name=$(cat /mnt/monitor_data/monitor.txt)

                #删除不用的文件
                rm -rf /mnt/monitor_data/monitor.txt

                #判断数组中的brick对应的数据文件是否已经生成，如果生成，则不用重建，否则创建该数据文件
                for i in ${digioceanfsd_name[@]};do

                    fileName=/mnt/monitor_data/orignal_data/${local_ip}$i.dat

                    if [ -e ${fileName} ];then

                        continue

                    fi

                    echo -e "\t" "\ttime\t\t" "pid\t\t" "timer\t\t" "${local_ip}$i">>/mnt/monitor_data/orignal_data/${local_ip}$i.dat

                done

                #将相应的brick内存数据写入相应的数据文件中
                for i in ${digioceanfsd_name[@]};do

                    brick_mem=`ps aux |grep -w ${digioceanfsd_name0[$n]}|grep -v disk_plug|grep -wv add|grep -v grep|awk '{print $5}'`
                    brick_pid=`ps aux |grep -w ${digioceanfsd_name0[$n]}|grep -v disk_plug|grep -wv add|grep -v grep|awk '{print $2}'`

                    echo -e "$line\t" "$time\t" "$brick_pid\t\t" "$timer\t\t\t\t" "$brick_mem">>/mnt/monitor_data/orignal_data/${local_ip}$i.dat

                    let n=$n+1
                done

                #将其他进程写入相应的数据文件
                digioceand_mem=`ps aux|grep -wv digioceanfsd|grep -w digioceand.pid|grep -w Ssl|grep -wv grep|awk '{print $5}'`
                digioceand_pid=`ps aux|grep -wv digioceanfsd|grep -w digioceand.pid|grep -w Ssl|grep -wv grep|awk '{print $2}'`
                    process_mem $timer $local_ip digioceand $time $line $digioceand_mem $digioceand_pid

                client_mem=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep "volfile-server"|grep -wv grep|awk '{print $5}'`
                client_pid=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep "volfile-server"|grep -wv grep|awk '{print $2}'`
                    process_mem $timer $local_ip digioceanfs $time $line $client_mem $client_pid

                nfs_mem=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep -w "nfs"|grep -wv grep|awk '{print $5}'`
                nfs_pid=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep -w "nfs"|grep -wv grep|awk '{print $2}'`
                    process_mem $timer $local_ip nfs $time $line $nfs_mem $nfs_pid

                self_heal_mem=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep -w "digioceanshd"|grep -wv grep|awk '{print $5}'`
                self_heal_pid=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep -w "digioceanshd"|grep -wv grep|awk '{print $2}'`
                    process_mem $timer $local_ip self_heal $time $line $self_heal_mem $self_heal_pid

                quotad_mem=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep -w quotad|grep -wv grep|awk '{print $5}'`
                quotad_pid=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep -w quotad|grep -wv grep|awk '{print $2}'`
                    process_mem $timer $local_ip quotad $time $line $quotad_mem $quotad_pid

                 quotad_mount_mem=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep quota|grep -wv quotad|grep mount|grep -wv grep|awk '{print $5}'`
                 quotad_mount_pid=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep quota|grep -wv quotad|grep mount|grep -wv grep|awk '{print $2}'`
#                quotad_mount_mem=`ps aux|grep -w digiocean|grep -wv digioceanfsd|grep -wv digioceand|grep -wv rebalance|grep -wv grep|awk '{print $5}'`
#                quotad_mount_pid=`ps aux|grep -w digiocean|grep -wv digioceanfsd|grep -wv digioceand|grep -wv rebalance|grep -wv grep|awk '{print $2}'`
                    process_mem $timer $local_ip quotad_mount $time $line $quotad_mount_mem $quotad_mount_pid

                rebalance_mem=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep -w rebalance|grep -w Ssl|grep -wv grep|awk '{print $5}'`
                rebalance_pid=`ps aux|grep -w digioceanfs|grep -wv digioceanfsd|grep -w rebalance|grep -w Ssl|grep -wv grep|awk '{print $2}'`
                    process_mem $timer $local_ip rebalance $time $line $rebalance_mem $rebalance_pid

                let line=$line+1
                sleep 10
                let timer=$timer+10
            done

            ;;

        "c")

            echo "If you want to copy all, please input 't', and if you want copy part node, please input 'p'."
            echo "Please input your command:"
            read comd

            case ${comd} in

                "t")

                    host_name=`hostname`
                    copy_ip=`cat '/etc/hosts'|grep "####DigioceanfsNode####"|grep -wv "$host_name"|awk '{print $1}'|grep -v '#'`

                    for ip in ${copy_ip[@]};do

                        echo $ip

                        scp @$ip:/mnt/monitor_data/orignal_data/*.dat /mnt/monitor_data/orignal_data/

                    done
                    ;;

                "p")

                    echo "Please input the node ip or hostname that you want copy, the middle separated by spaces:"

                    read copy_ip

                    for ip in $copy_ip;do

                        echo $ip

                        scp @$ip:/mnt/monitor_data/orignal_data/*.dat /mnt/monitor_data/orignal_data/

                    done

                    ;;
            esac

            ;;
		
        "a")
			
            rm -rf /mnt/monitor_data/abnormal.txt

            path=/mnt/monitor_data/orignal_data/
            path_1=/mnt/monitor_data/analy_data/

            #分析报告文件的路径及名称
            analy_file=/mnt/monitor_data/abnormal.txt

            rm -rf ${path_1}*.dat

            fileName=(`ls $path`)

            for i in ${fileName[@]};do
                #数据文件中的pid是否有变化的标志位
                flag=0

                n=0

                #该标志首先指向数据的第一行，下一次指向pid发生变化的第一行，这样就可以获取到这之间pid没有发生变化的行数，将其重新写入一个新的文件，以便区分
                data_start_line=2

                #获取文件的总行数（即包含第一行表头）
                data_line=`sed -n '$=' ${path}$i`

                #设定一个标志位，用来判断下面循环查询pid是否到最后一行
                line_1=2

                echo $i

                pid_array=(`cat $path$i|grep -v pid|awk '{print $3}'`)

                for j in ${pid_array[@]};do


                    #开始的时候只获取到数据的第一行(文件的第二行)的pid，所以不用比较pid有没有变化，所以判断条件是n>0
                    if ((n > 0));then

                    #如果发现前后两个pid不同，则说明pid发生变化，将信息写入报告日志，然后按照pid将数据分成不同的文件
                        if (($j != $last_pid));then

                            let m=$n+1
                            let l=$n+2

                            echo "警告：$i数据文件中第$n行的pid和第$m行的pid发生变化！">>$analy_file
                            echo   >>$analy_file

                            sed -n "1p" "$path$i">>$analy_file
                            sed -n "${m}p" "$path$i">>$analy_file
                            sed -n "${l}p" "$path$i">>$analy_file
                            echo   >>$analy_file
                            echo   >>$analy_file

                            sed -n "1p" "$path$i">>${path_1}$last_pid-$i
                            sed -n "$data_start_line,${m}p" "$path$i">>${path_1}$last_pid-$i

                            flag=1

                            data_start_line=$l

                        fi

                    fi

                    if ((line_1 == data_line));then

                        sed -n "1p" "$path$i">>${path_1}$j-$i
                        sed -n "$data_start_line,${data_line}p" "$path$i">>${path_1}$j-$i

                    fi

                    last_pid=$j
                    let n=$n+1
                    let line_1=$line_1+1

                done

                if ((flag == 0));then

                    cp $path$i ${path_1}$j-$i
                fi

            done

            ;;

        "g")
				
            rm -rf /mnt/monitor_data/image/*.png

            path=/mnt/monitor_data/analy_data/

            ls $path>>/mnt/monitor_data/file_name.txt

            sed -i "s/.dat//g" `grep .dat -rl /mnt/monitor_data/file_name.txt`

            processName=$(cat /mnt/monitor_data/file_name.txt)

            rm -rf /mnt/monitor_data/file_name.txt

            ylabelName="memory"

            for i in ${processName[@]};do

                time_start=`sed -n "2p" "$path${i}.dat"|awk '{print $2}'`

                line=`sed -n '$=' "$path${i}.dat"`

                time_stop=`sed -n "${line}p" "$path${i}.dat"|awk '{print $2}'`

                xlabelName="timer(${time_start}----${time_stop})"


                echo "
                    set xlabel \"$xlabelName\"
                    set ylabel \"$ylabelName\"
                    set title \"$i\"
                    set term png lw 1
                    set output \"/mnt/monitor_data/image/$i.png\"
                    plot \"${path}$i.dat\" using 4:5 w lp pt 0 title \"$i\"
                    set output
                     "|gnuplot

            done

            ;;

        "r")
            cd /mnt/monitor_data
            time0=`date +%Y-%m-%d-%H":"%M":"%S`
            echo "$time0"
            mkdir ./record/$time0

            cp -r ./analy_data ./record/$time0
            cp -r ./image ./record/$time0
            cp -r ./orignal_data ./record/$time0

            filename="abnormal.txt"
            if [ -e ${filename} ];then
                cp -r ./abnormal.txt ./record/$time0
            fi

            help

            ;;

        "h")

            help

            ;;

        "q")

            exit

            ;;

    esac
done
