set pagination off
set print repeats 0
set print pretty
set logging on ./rename_server.log

#b io_stats_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b ro_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b quota_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b barrier_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b marker_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b iot_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b up_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b changelog_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b ctr_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end
#
#b trash_rename
#commands
#        p oldloc->inode
#        p newloc->inode
#        bt
#        c
#end

b posix_rename
commands
        p oldloc->inode
        p newloc->inode
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
