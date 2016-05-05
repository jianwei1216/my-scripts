#!/bin/bash

DEFAULT_DIR=/tmp/investigation_`date +"%Y%m%d%H%M%S"`/
BRIEF_INFO=brief
#IPC_INFO=ipc
NETWORK_INFO=network
PROC_INFO=proc
FILESYS_INFO=filesys
SYSLOG_INFO=syslog
COREDUMP_INFO=coredump
MOD_INFO=modules
SYSCONF_INFO=sysconf
NFS=nfs

DIR=$DEFAULT_DIR
CPATH=`pwd`

# prepare
mkdir -p $DIR

#### GENERAL INFO
mkdir -p $DIR/$BRIEF_INFO

date >> $DIR/$BRIEF_INFO/date 2>&1

uname -a >> $DIR/$BRIEF_INFO/uname 2>&1

cat /proc/version  >> $DIR/$BRIEF_INFO/proc_version 2>&1

smbstatus -v >> $DIR/$BRIEF_INFO/smbstatus_v 2>&1

nfsstat -o all -234 >> $DIR/$BRIEF_INFO/nfsstat_oall_234 2>&1

#### FILESYSTEM
mkdir -p $DIR/$FILESYS_INFO

df -Th >> $DIR/$FILESYS_INFO/df_Th 2>&1

fdisk -l >> $DIR/$FILESYS_INFO/fdisk_l 2>&1

mount -l -v >> $DIR/$FILESYS_INFO/mount_l_v 2>&1

iostat -h -t -n -N -p ALL >> $DIR/$FILESYS_INFO/iostat_h_t_n_N_pALL 2>&1

for i in `ls /dev/md* ` ; do
    mdadm -D $i >> $DIR/$FILESYS_INFO/RAID 2>&1
done

#### coredump
mkdir -p $DIR/$COREDUMP_INFO

function cp_corefile () {
	
	__TMP=`strings $1 | grep _= | awk -F'/' '{print $4}'`
	
	if [ $__TMP = "digioceanfs" ] ; then
		cp $1 $DIR/$COREDUMP_INFO
	elif [ $__TMP = "digioceanfsd" ] ; then
		cp $1 $DIR/$COREDUMP_INFO
	fi

}

if [ `ulimit -c` != "0" ] ; then
	__TMP=`cat /proc/sys/kernel/core_pattern|awk '{print index($0,"/")}'`
	case  "$__TMP" in
		0)
			for i in `ls /*core*` ; do
				cp_corefile $i ;
			done
			;;
		1)
			__TMP="${__TMP%\/*}"
			for i in `find $__TMP -name "*core*"` ; do
				cp_corefile $i ;
			done
			;;
		*)
			echo "[FAIL] core dump file path had been manually specified, Could you please include it manully? Thx. "
			;;
	esac
fi

#### network config.
mkdir -p $DIR/$NETWORK_INFO/

ip address show >> $DIR/$NETWORK_INFO/ip_ad_sh 2>&1

ip ro sh >> $DIR/$NETWORK_INFO/ip_ro_sh 2>&1

ifconfig -a >> $DIR/$NETWORK_INFO/ifconfig_a 2>&1

netstat -s >> $DIR/$NETWORK_INFO/netstat_s 2>&1

netstat -a >> $DIR/$NETWORK_INFO/netstat_a 2>&1

#### proc. info
mkdir -p $DIR/$PROC_INFO/

mpstat -P ALL >> $DIR/$PROC_INFO/mpstat_PALL 2>&1

ps aux --forest >> $DIR/$PROC_INFO/ps_aux__forest 2>&1

ipcs -a >> $DIR/$PROC_INFO/ipcs_a 2>&1

cat /proc/cpuinfo >> $DIR/$PROC_INFO/cpuinfo 2>&1

cat /proc/crypto >> $DIR/$PROC_INFO/crypto 2>&1

cat /proc/slabinfo >> $DIR/$PROC_INFO/slabinfo 2>&1

cat /proc/timer_stats >> $DIR/$PROC_INFO/timer_stats 2>&1

cat /proc/filesystems >> $DIR/$PROC_INFO/filesystems 2>&1

cat /proc/buddyinfo >> $DIR/$PROC_INFO/buddyinfo 2>&1

cat /proc/devices >> $DIR/$PROC_INFO/devices 2>&1

cat /proc/locks >> $DIR/$PROC_INFO/locks 2>&1

cat /proc/cgroups >> $DIR/$PROC_INFO/cgroups 2>&1

cat /proc/cmdline >> $DIR/$PROC_INFO/cmdline 2>&1

cat /proc/diskstats >> $DIR/$PROC_INFO/diskstats 2>&1

cat /proc/dma >> $DIR/$PROC_INFO/dma 2>&1

cat /proc/execdomains >> $DIR/$PROC_INFO/execdomains 2>&1

cat /proc/interrupts >>  $DIR/$PROC_INFO/interrupts 2>&1

cat /proc/iomem >> $DIR/$PROC_INFO/iomem 2>&1

cat /proc/ioports >> $DIR/$PROC_INFO/ioports 2>&1

cat /proc/kallsyms >> $DIR/$PROC_INFO/kallsyms 2>&1

cat /proc/key-users >> $DIR/$PROC_INFO/key-users 2>&1

cat /proc/loadavg >> $DIR/$PROC_INFO/loadavg 2>&1

cat /proc/misc >> $DIR/$PROC_INFO/misc 2>&1

cat /proc/locks >> $DIR/$PROC_INFO/locks 2>&1

cat /proc/mdstat >> $DIR/$PROC_INFO/mdstat 2>&1

cat /proc/meminfo >> $DIR/$PROC_INFO/meminfo 2>&1

cat /proc/modules >> $DIR/$PROC_INFO/modules 2>&1

cat /proc/mtrr >> $DIR/$PROC_INFO/mtrr 2>&1 

cat /proc/pagetypeinfo >> $DIR/$PROC_INFO/pagetypeinfo 2>&1

cat /proc/partitions >> $DIR/$PROC_INFO/partitions 2>&1

cat /proc/sched_debug >> $DIR/$PROC_INFO/sched_debug 2>&1

cat /proc/slabinfo >> $DIR/$PROC_INFO/slabinfo 2>&1

cat /proc/stat >> $DIR/$PROC_INFO/stat 2>&1

cat /proc/swaps >> $DIR/$PROC_INFO/swaps 2>&1

cat /proc/timer_list >> $DIR/$PROC_INFO/timer_list 2>&1

cat /proc/timer_stats >> $DIR/$PROC_INFO/timer_stats 2>&1

cat /proc/uptime >> $DIR/$PROC_INFO/uptime 2>&1

cat /proc/vmallocinfo >> $DIR/$PROC_INFO/vmallocinfo 2>&1

cat /proc/vmstat >> $DIR/$PROC_INFO/vmstat 2>&1

cat /proc/zoneinfo >> $DIR/$PROC_INFO/zoneinfo 2>&1

#### modules in use
mkdir -p $DIR/$MOD_INFO

lsmod >> $DIR/$MOD_INFO/lsmod 2>&1

#### sys. log & file sys. log & dmsg
mkdir -p $DIR/$SYSLOG_INFO

sar -A >> $DIR/$SYSLOG_INFO/sar_A 2>&1

cp -rf /var/log/dmesg* $DIR/$SYSLOG_INFO/

cp -rf /var/log/digioceanfs* $DIR/$SYSLOG_INFO/

cp -rf /var/log/auth* $DIR/$SYSLOG_INFO/

cp -rf /var/log/daemon* $DIR/$SYSLOG_INFO/

cp -rf /var/log/debug* $DIR/$SYSLOG_INFO/

cp -rf /var/log/dpkg* $DIR/$SYSLOG_INFO/

cp -rf /var/log/kern* $DIR/$SYSLOG_INFO/

cp -rf /var/log/message* $DIR/$SYSLOG_INFO/

cp -rf /var/log/syslog* $DIR/$SYSLOG_INFO/

cp -rf /var/log/user* $DIR/$SYSLOG_INFO/

cp -rf /var/log/apt* $DIR/$SYSLOG_INFO/

#### NFS ####
mkdir -p $DIR/$NFS

cp -rf /var/lib/nfs/* $DIR/$NFS
cp -rf /etc/exports   $DIR/$NFS

#### SAMBA ####
mkdir -p $DIR/$SAMBA

cp -rf /var/lib/nfs/* $DIR/$SAMBA

cp -rf /var/log/samba* $DIR/$SYSLOG_INFO/

#### conf. files
mkdir -p $DIR/$SYSCONF_INFO

cp -rf /etc/* $DIR/$SYSCONF_INFO

#### pack up
cd $DIR/ && tar -czf $CPATH/`date +"%Y%m%d%H%M%S"`.tar.gz *

cd $CPATH && rm -rf $DIR
