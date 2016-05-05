set pagination off
set print repeats 0
set logging on /root/svn/myscript/gdb/ping_gdb/ping.log

# rpc_clnt_ping_timer_expired
b rpc-clnt-ping.c:156
commands
    bt
    c
end

# call_bail
b rpc-clnt.c:188
commands
    bt
    c
end

# socket_submit_request
b socket.c:3300
commands
    bt
    c
end

# rpc_clnt_submit
b rpc-clnt.c:1568
commands
    bt
    c
end

