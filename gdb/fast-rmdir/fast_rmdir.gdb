set pagination off
set print pretty
set logging on ./fast_rmdir.log


b client_getxattr if (strcmp(name, "trusted.glusterfs.fast-rmdir") == 0)
commands
        p this->name
        bt
        c
end



