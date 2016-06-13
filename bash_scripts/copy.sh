#!/bin/sh

copy_dir="/var/lib/digioceand/ /var/log/digioceanfs/ /var/run/digiocean/"
save_dir=""
ip_pre="10.10.21.11"
ip_save="10.10.12.16"

function mycopy()
{
    for i in 1 2 3; do
        tmp_dir1="digioceanfs.${ip_pre}${i}"
        tmp_dir2="/var/${tmp_dir1}"
        echo ssh ${ip_pre}${i} "mkdir -p $tmp_dir2; cp -rf ${copy_dir} $tmp_dir2; cd /var/; tar -czvf ${tmp_dir1}.tar.gz $tmp_dir1; scp ${tmp_dir1}.tar.gz ${save_dir}; rm -rf ${tmp_dir1}.tar.gz ${tmp_dir1}" 
        ssh ${ip_pre}${i} "mkdir -p $tmp_dir2; ps aux|grep digiocean|grep -v grep > ${tmp_dir2}/digioceanfs.ps.aux; cp -rf ${copy_dir} $tmp_dir2; cd /var/; tar -czvf ${tmp_dir1}.tar.gz $tmp_dir1; scp ${tmp_dir1}.tar.gz ${save_dir}; rm -rf ${tmp_dir1}.tar.gz ${tmp_dir1}" 
        echo
    done
}
case $1 in
  --save-dir)
      save_dir=${ip_save}:$2
      mycopy
      ;;
  *)
      echo "Usage: $0 --save-dir <DIR>"
      ;;
esac
