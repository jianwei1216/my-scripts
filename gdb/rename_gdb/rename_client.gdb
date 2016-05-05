set pagination off
set print repeats 0
set print pretty
set logging on ./rename_client.log

#b io_stats_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b md_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b ob_rename
#commands
#        p src->inode
#        p dst->inode
#        bt
#        c
#end
#
b dht_rename
commands
        p oldloc->inode
        p newloc->inode
        bt
        c
end

#b ec_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b client_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end

#b inode_new
#commands
#        p table
#        bt
#        c
#end

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
