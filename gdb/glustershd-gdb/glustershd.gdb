set logging on ./gdb-glustershd.log
set pagination off
set print pretty

set follow-fork-mode child
b main
b create_fuse_mount
b glusterfs_volumes_init
b get_volfp
b glusterfs_process_volfp
r  -s localhost --volfile-id gluster/glustershd -p /var/lib/glusterd/glustershd/run/glustershd.pid -l /var/log/glusterfs/glustershd.log -S /var/run/gluster/23dbcc967cc95885915aa8d37dcf0c01.socket --xlator-option *replicate*.node-uuid=795c4cdc-4d3e-4000-b7f5-c4ee0f194ad5
