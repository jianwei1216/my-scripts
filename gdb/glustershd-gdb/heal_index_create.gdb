set pagination off
set print pretty
set logging on ./gdb-heal-index-create.log

b gf_thread_create_detach 
commands
    bt
    c
end

b ec_notify if event==5
commands
    bt
    c
end

b ec_selfheal_childup
commands
    p child
    bt
    c
end

c
