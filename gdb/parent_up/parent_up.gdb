set pagination off
set print pretty
set logging on ./parent_up.log

b dofs_mongodb_insert_ec_down
commands
        p this->mgmt_parent_down
        bt
        c
end

b ec_notify
commands
        p this->mgmt_parent_down
        bt
        c
end

b dofs_mongodb_insert_client_down
commands
        p this->mgmt_parent_down
        bt
        c
end

b client.c:2349
commands
        p this->mgmt_parent_down
        bt
        c
end

b client.c:2362
commands
        p this->mgmt_parent_down
        bt
        c
end
