set pagination off
set print repeats 0
set print pretty
set logging on ./create_server.log

b posix_create
commands
        p loc->inode
        bt
        c
end

b inode_new
commands
        p table
        bt
        c
end

b inode.c:650
commands
        p inode
        bt
        c
end

b __inode_passivate
commands
        p inode
        bt
        c
end

b __inode_retire
commands
        p inode
        bt
        c
end
