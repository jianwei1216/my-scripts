#!/usr/bin/python

import os
import sys
import ssh
import thread
import threading
from colors import *

mutex = threading.Lock()

def get_ssh_client(host):
        global ssh_passwd
        client = ssh.SSHClient()
        client.set_missing_host_key_policy(ssh.AutoAddPolicy())
        client.connect(host, port=9999, username='root', password=ssh_passwd)
        return client

def __despatch_cmd(host, lock, cmd):
        client = get_ssh_client(host)
        stdin, stdout, stderr = client.exec_command(cmd)
        err = stderr.read()
        out = stdout.read()
        if mutex.acquire(1):
                if len(err) > 0:
                        print host, err,
                if len(out) > 0:
                        print host, out,
                mutex.release()
        client.close()
        lock.release()

def multi_thread(func, nodes, cmd):
        locks = []
        for host in nodes:
                try:
                        lock = thread.allocate_lock()
                        lock.acquire()
                        locks.append(lock)
                        print host, cmd
                        thread.start_new_thread(func, (host, lock, cmd))
                except:
                        print "Error: unable to start thread!"
        for i in locks:
                while i.locked():
                        pass

def despatch_cmd(args):
        is_passwd = False

        global ssh_passwd
        ssh_passwd = ''
        index1 = ''
        index2 = ''
        config_file = './.fast.py.config'
        if os.path.isfile(config_file) == False:
                os.mknod(config_file)

        for i in range(0, len(args)):
                if args[i] == '--passwd':
                        is_passwd = True 
                        index1 = i
                elif is_passwd == True:
                        ssh_passwd = args[i]
                        index2 = i
                        break
        if index1 != '' and index2 != '':
                del args[index1]
                del args[index2 - 1]
        cmd = args[-1]
        nodes = args[1:-1]
        if len(nodes) == 0:
                fp = open (config_file, 'rb')
                fargs = fp.readline()
                if len(fargs) == 0:
                        print 'Fatal error: ./.fast.py.config is empty,'\
                              'please give all args!'
                        exit(-1)
                ssh_passwd = fargs.split(' ')[0]
                nodes = fargs.split(' ')[1:]
        else:
                fp = open (config_file, 'wb')
                fp.truncate(0)
                fp.write(ssh_passwd + ' ')
                for i in range(0, len(nodes)):
                        if i == len(nodes)-1:
                                fp.write(nodes[i])
                        else:
                                fp.write(nodes[i] + ' ')
        print nodes
        print cmd
        print ssh_passwd
        if ssh_passwd == '':
                print 'Error: password is NULL!'
                exit(-1)
        multi_thread(__despatch_cmd, nodes, cmd)

if __name__ == '__main__':
        if len(sys.argv) < 2:
                print 'Usage: ' + RED + 'python ' + __file__ + ' your_nodes' + ' your_commands --passwd password\n' + ENDC\
                      + 'e.g. python ' + __file__ + ' 10.10.21.9{1,2,3} \"date\"'
                print 'Notes:\nwhen give all all args,\n'\
                      + 'it can record your_nodes and password in the .fast.py.config,\n'\
                      + 'and next time you can directly execute ' + RED + './fast.py \"date\"' + ENDC
                exit(0)

        despatch_cmd(sys.argv)
