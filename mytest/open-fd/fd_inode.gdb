set pagination off
set print pretty
set print repeats 0
set logging on ./fd_inode.log


b ec_open
commands
    p fd->inode
    p fd->inode->fd_count
    bt
    c
end
