#!/usr/bin/python
# -*- conding:UTF-8 -*-

import sys
import os
import tarfile
import ssh
import thread
import socket
import time

def get_ssh_client():
        client = ssh.SSHClient()
        client.set_missing_host_key_policy(ssh.AutoAddPolicy())
        return client
        
def __pack_tar(host, lock, args):
        global ssh_password
        packcmd = ''
        #time_suffix = time.strftime("%Y-%m-%d_%H:%M:%S", time.localtime())
        time_suffix = str(int(time.time()))
        save_pack_path = args[0]
        need_pack_files = args[1]
        print 'debug1:', save_pack_path
        print 'debug2:', need_pack_files
        packcmd = 'cd ' + save_pack_path[0] + '; tar -czvf ' + \
                  socket.gethostname() + '_' + time_suffix + '.tar.gz'
        for filepath in need_pack_files:
                packcmd += ' ' + filepath
        print host, packcmd
        client = get_ssh_client()
        client.connect(host, port=22, username='root', password=ssh_password)
        stdin, stdout, stderr =  client.exec_command(packcmd)
        print stderr.read()
        print stdout.read()
        client.close()
        lock.release()

def __multi_thread(nodes, func, *args):
        locks = []
        for host in nodes:
                lock = thread.allocate_lock()
                lock.acquire()
                locks.append(lock)
                thread.start_new_thread(func, (host, lock, args))

        for lock in locks:
                while lock.locked():
                        pass
        pass

def pack_tar(args):
        is_nodes = False
        is_need_pack_files = False
        is_save_pack_path = False
        need_pack_nodes = []
        save_pack_path = []
        need_pack_files = []
        if len(args) == 0:
                print 'Error: args are zero!'
                exit (-1)
        for arg in args:
                if arg == '--nodes':
                        is_nodes = True
                        is_need_pack_files = False
                        is_save_pack_path = False
                        continue
                elif arg == '--need_pack_files':
                        is_need_pack_files = True
                        is_nodes = False
                        is_save_pack_path = False
                        continue
                elif arg == '--save_pack_path':
                        is_save_pack_path = True
                        is_need_pack_iles = False
                        is_nodes = False
                        continue
                else:
                        if is_nodes == True:
                                need_pack_nodes.append(arg)
                        elif is_need_pack_files == True:
                                need_pack_files.append(arg)
                        elif is_save_pack_path == True:
                                save_pack_path.append(arg)

        if need_pack_nodes == [] or need_pack_files == [] or save_pack_path == []:
                print 'Error: args are invalid!'
                exit(-1)

        print 'debug:', need_pack_nodes
        print 'debug:', save_pack_path
        print 'debug:', need_pack_files
        __multi_thread(need_pack_nodes, __pack_tar, save_pack_path, need_pack_files) 

def unpack_tar(args):
        pass

def scp_tar(args):
        pass

if __name__ == '__main__':
        is_password = False
        password_pos = ''
        password = ''
        global ssh_password

        if len(sys.argv) < 2:
                print 'Usage: python %s \n\t\t[--pack --nodes nodes --save_pack_path pathname --need_pack_files pathname] \n\t\t[--unpack nodes:/pathname] '\
                      '\n\t\t[--scp --src nodes:/pathname --dst nodes:pathname] \n\t\t--password ssh_password'\
                      % (__file__)
                exit (-1)

        for i in range(0, len(sys.argv)):
                if sys.argv[i] == '--password':
                       password_pos = i 
                       is_password = True
                       continue
                else:
                        if is_password == True:
                                password = sys.argv[i]

        if password == '' or password_pos == '':
                print 'Error: ssh_password is invalid!'
                exit(-1)

        ssh_password = password
        cmd = sys.argv[1]
        args = sys.argv[2:password_pos]
        print 'debug:', cmd, args

        if len(cmd) == 0 or len(args) == 0:
                print 'Error: args are invalid!'
                exit (-1)

        if cmd == '--pack':
                pack_tar(args)
        elif cmd == '--unpack':
                pass
        elif cmd == '--scp':
                pass
        else:
                print 'Error: unknown option!'
                exit (-1)
