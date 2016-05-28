#!/usr/bin/bash

save_file_name=""

function filter_op()
{
        source /etc/sysconfig/network
        suffix="`ifconfig  | grep inet | grep -v inet6 | grep 10.10 | awk '{print $2}'`"
        echo $suffix
        grep MKNOD /var/log/digioceanfs/cluster2-test.log| awk '{print $6}' | awk -F '[':']' '{print $2}' > /var/log/digioceanfs/${save_file_name}.mknod.${HOSTNAME}
        grep MKDIR /var/log/digioceanfs/cluster2-test.log| awk '{print $6}' | awk -F '[':']' '{print $2}' > /var/log/digioceanfs/${save_file_name}.mkdir.${HOSTNAME}
        grep LOOKUP /var/log/digioceanfs/cluster2-test.log| awk '{print $6}' | awk -F '[':']' '{print $2}' > /var/log/digioceanfs/${save_file_name}.lookup.${HOSTNAME}

}

case $1 in
  yes)
          save_file_name=$2
          filter_op
          ;;
  *)
          echo "Usage: ./filter_op.sh yes save_file_name"
          ;;
esac
