#!/bin/bash
sys_block_dir=/sys/block/
mvl_block_dir=/dev/mvsa/
mdstat_file=/proc/mdstat
fstab_file=/etc/fstab

vols_dir=/etc/digioceanfs/vols/

system_disks=
disks_used=
disks_to_partition=
disk_specify=
file_system_type=xfs
list=

usage ()
{
	echo "Usage: digi_partition [-d <device_name>] [-f <file_system_type>]"
	echo "partition all disks except system disk, and add these disks to /dev/fstab"
	echo " "
	echo "Options:"
	echo "	-d, --dev       specify device to be partitioned"
	echo "	                use device name,for example 'sda'"
	echo "	                default:all disk except system disk"
	echo " "
	echo "	-f, --fs-type   specify file system type"
	echo "	                default:xfs"
}

find_system_disks_by_lvm ()
{
	root_dev=$(cat ${fstab_file} | grep -v ^# | awk '{print $1" "$2}' | grep -E "/$" | awk '{print $1}')
	if [ $(echo $root_dev | awk -F/ '{print $3}') == mapper ]
	then 
		system_disks_found=$(lvm pvdisplay | grep "PV Name" | awk '{print $3}' | awk -F/ '{printf"%s", $3}' | sed 's/[0-9]//g')
		#echo "system disk:"${system_disks_found}
	else
                system_disks_found=${root_dev:5:3}
	fi
	system_disks=${system_disks_found}
}

find_system_disks_by_uuid ()
{
	root_uuid=$(cat ${fstab_file} | grep -v ^# | awk '{print $1" "$2}' | grep -E "/$" | awk '{print $1}'|awk -F'=' '{print $2}')
	boot_disk_str=$(ls -l /dev/disk/by-uuid/ |grep ef342e2b-5fd3-4a42-ba2d-c9faf8821ac2 | awk '{print $11}'| sed 's/[0-9]//g')
	boot_disk=${boot_disk_str##*/}
	system_disks=$boot_disk
}
find_system_disks_by_blkid()
{
        boot_disk=$(blkid |grep -E 'swap|LVM' |grep -E "sd|hd|md"|awk -F ': ' '{print $1}'| awk -F '/' '{print $3}'|sed 's/[0-9]//g')
        system_disks=$boot_disk
}
find_system_disks ()
{
	find_system_disks_by_blkid
}

init_raid ()
{
        config_str=`mdadm -E -s`
        mdname=''
        f_mdname=''
        uuid=''
        for s in $config_str
        do
                if [ -z $mdname ]
                then
                        mdname=`echo $s |grep -E 'md'`
                fi
                if [ -z $uuid ]
                then
                        uuid=`echo $s |grep -E 'UUID' |awk -F '=' '{print $2}'`
                fi
                if [[ $mdname && $uuid ]]
                then
                        if [ "$mdname" = "$f_mdname" ]
                        then
                                raid_num=`echo $mdname|sed 's/\/dev\/md\///g'`
                                let "raid_num=$raid_num + 1"
                                mdname='/dev/md/'$raid_num
                        else
                                mdadm -A $mdname --uuid=$uuid --force
                                f_mdname=$mdname
                                mdname=''
                                uuid=''
                        fi
                fi
        done
}


find_used_disks ()
{
	disk_initialed=$(ls ${vols_dir})
	for disk in ${disk_initialed}
	do
		used=$(cat ${vols_dir}/${disk}/user)
		if [ ${used} ]
		then
			disks_used=${disks_used}" "${disk}
		fi
	done
}
list_disks ()
{
	mvl_disks_all=$(ls ${mvl_block_dir} |grep "mvsa")	
	disks_all_tmp=$(ls ${sys_block_dir} | grep -E "^sd|^hd|^md")
	if [[ -n ${mvl_disks_all} ]]
	then
		for mdisk in ${mvl_disks_all}
		do
			for ndisk in ${disks_all_tmp}
				do
					if [[ "$mvl_disks_all" == *"$ndisk"* ]]
					then
						break 1
					else
						disks_all=$disks_all' '${ndisk}
					fi
			done
                	realname=$(readlink "/dev/mvsa/${mdisk}")
			rname=$(basename ${realname})
			disks_all=$disks_all' '${rname}	
			ln -sf $realname /dev/disk/by-id/wwn-MARVELLBLCK_"${mdisk}" 
		done
	else
		disks_all=$disks_all_tmp				
	fi
	disks_raid=$(cat /proc/mdstat | grep ^md | sed 's/ /\n/g' | grep -E "\<sd|\<hd" | awk -F[ '{print $1}')
	disks_exclude=${disks_raid}" "${system_disks}
	for disk in ${disks_all}
	do
		if [ ${disk:0:2} == md ]
		then
			slaves=$(ls /sys/block/${disk}/slaves)
			if [ ${#slaves} == 0 ]
			then
				continue
			fi
		fi
		for a in a
		do
			for disk_exclude in ${disks_exclude}
			do
				if [ ${disk} == ${disk_exclude} ]
				then
					break 2
				fi
			done
			disks=${disks}' '${disk}
			echo ${disk}
			break 1
		done
	done
#	echo ${disks}
}
find_raid_disks ()
{
	raid_level=$(cat /proc/mdstat | grep -E "^$1 " | sed 's/ /\n/g' | grep -E "^raid")
	if [ -z ${raid_level} ]
	then
                raid_level="raidNaN"
	fi
	active_string=$(cat /proc/mdstat | grep -E "^$1 " | sed 's/ /\n/g' | grep -E "inactive")
	if [ -z ${active_string} ]
	then
		active="active"
	else
		active=$active_string
	fi
	subdisks=$(cat /proc/mdstat | grep -E "^$1 " | sed 's/ /\n/g' | grep -E "\<sd|\<hd" | awk -F[ '{print $1}')

	echo ${raid_level:4}" "${active}" "${subdisks}

	for adisk in ${subdisks}
	do
		if [ -e "/dev/"$adisk ] ; then
			disk_size=$(($(cat /sys/block/${adisk}/size)/2))
			echo ${adisk}" "${disk_size}
		fi
	done
}
find_disks_to_partition () 
{
	if [ -z ${disks_specify} ]
	then
		disks_all=$(ls ${sys_block_dir} | grep -E "^sd|^hd|^md")
	else
		disks_all=${disks_specify}
	fi
	echo "disk all:"${disks_all}
	disks_raid=$(cat /proc/mdstat | grep ^md | sed 's/ /\n/g' | grep -E "\<sd|\<hd" | awk -F[ '{print $1}')
	disks_not_partition=${disks_raid}" "${system_disks}" "${disks_used}
	for disk in ${disks_all}
	do
		for a in a
		do
			for disk_not_partition in ${disks_not_partition}
			do
#				echo "compare "${disk}" with "${disk_not_partition}
				if [ ${disk} == ${disk_not_partition} ]
				then
					break 2
				fi
			done
			disks_to_partition=${disks_to_partition}' '${disk}
			echo "disks_to_partition:"${disks_to_partition} 
			break 1
		done
	done
}
make_filesystem ()
{
	reply=n
	if [ ${#disks_to_partition} == 0 ]
	then
		echo "there is no disk to partition"
	else
		mkdir /digioceanfs/ 2>/dev/null
		mkdigioceanfs ${disks_to_partition}
	fi
}
mount_and_add ()
{
	disk=$1
	echo "partition in progress"
	disk_base=$(basename ${disk})
	mount_dirs=$(mount | grep ${disk_base} | awk '{print $3}')
	if [ ${#mount_dirs} ]
	then
		for mount_dir in ${mount_dirs}
		do
			umount $mount_dir
		done
	fi
	mkfs.${file_system_type} -f ${disk} 
	fstab_para=$(echo ${disk}"	/digioceanfs/"${disk_base}"	"${file_system_type}"	defaults	0	0")
#check if disk_base is in /etc/fstab
    infstab=$(grep ${disk_base} /etc/fstab)
    if [ -z ${infstab} ]
    then
	    echo ${fstab_para} >> /etc/fstab
    fi
	mkdir /digioceanfs/${disk_base} 2>/dev/null
	mount -t ${file_system_type} ${disk} /digioceanfs/${disk_base}
}

while test $#
do
	case $1 in
		--help | -h )
			usage
			exit 0 ;;
		--dev | -d )
			! test $2 && eval "echo 'missing device for option \"--dev\"'" && exit 1
#			test $(echo $2 | awk -F/ '{print $2}') && eval "echo 'use device name without path name. eg:use sda instead of /dev/sda'" && exit 1
#			echo $2 | awk -F/ '{print $2}'
			disks_specify=$2
			shift ; shift ;;
		--fs-type | -f )
			! test $2 && eval "echo 'missing device for option \"--fs-type\". use default file system type ext3'"
			file_system_type=$2
			shift ; shift ;;
		--list | -l )
			list=y
			shift ; shift ;;
		--init-raid | -i )
			init_raid=y
			list=y
			shift ; shift ;;
		--list-raid | -r )
			list_raid=y
			! test $2 && eval "echo 'missing raid device for option \"--list-raid\".'"
			raid_dev=$2
			shift ; shift ;;
		-* )
			echo "invalid option $1" ; exit 1 ;;
		* )
			break ;;
	esac
done
if [ ${list_raid} ]
then
	find_raid_disks ${raid_dev}
        exit 0
fi

if [ $init_raid ]
then
	init_raid
fi

if [ -z ${list} ]
then
	find_system_disks
	find_used_disks
	find_disks_to_partition
	make_filesystem
else
	find_system_disks
	list_disks
fi


exit 0
echo "characters in disks_all"
for tmp in ${disks_all}
do
	echo ${tmp}
done
echo "characters in disks_raid"
for tmp in ${disks_raid}
do
	echo ${tmp}
done
