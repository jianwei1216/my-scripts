#!/usr/bin/python

import socket

def client_test():
    s = socket.socket()
    host = socket.gethostname()
    port = 12345

    s.connect((host, port))
    print s.recv(1024)
    s.send("GoodBye")
    s.close()
