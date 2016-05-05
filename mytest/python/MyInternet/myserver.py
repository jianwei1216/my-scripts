#!/usr/bin/python

import socket

def server_test():
    s = socket.socket()
    host = socket.gethostname()
    port = 12345
    s.bind((host, port))

    s.listen(5)
    while True:
        c, addr = s.accept()
        print c
        print 'connect addr: ', addr
        c.send('Welcome to CaiNiao!')
        if cmp(c.recv(1024), "GoodBye") == 0:
            break
        c.close()
    s.close()
