set print pretty
set pagination off
set logging on ./xattr.log3

b dict.c:152
commands
    p key1
    c
end
