set pagination off
set print repeats 0
set logging on /var/log/glusterfs/pl_gdb.log

b fetch_pathinfo
commands
    bt
    c
end

c
