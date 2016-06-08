set pagination off
set print pretty
set logging on ./gdb-glusterd-glustershd.log

b glusterd_shdsvc_create_volfile
