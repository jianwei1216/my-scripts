#!/usr/bin/python
# -*- conding:UTF-8 -*-

import sys
import os
import tarfile
import ssh
import thread
import time

def get_ssh_client(host):
        client = ssh.SSHClient()
        client.set_missing_host_key_policy(ssh.AutoAddPolicy())
        client.connect(host, port=22, username='root', password=ssh_password)
        return client
        
def __pack_tar(host, lock, args):
        global ssh_password
        packcmd = ''
        time_suffix = str(int(time.time()))
        save_pack_path = args[0]
        need_pack_files = args[1]
        packcmd = 'cd ' + save_pack_path[0] + '; tar -czvf ' + \
                  host + '_' + time_suffix + '.tar.gz'
        for filepath in need_pack_files:
                packcmd += ' ' + filepath
        print host, packcmd
        client = get_ssh_client(host)
        stdin, stdout, stderr =  client.exec_command('mkdir -p ' + save_pack_path[0])
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

        print 'need_pack_nodes:', need_pack_nodes
        print 'save_pack_path:', save_pack_path
        print 'need_pack_files:', need_pack_files
        __multi_thread(need_pack_nodes, __pack_tar, save_pack_path, need_pack_files) 

def unpack_tar(args):
        pass

def __scp_tar(host, lock, args):
        global ssh_password
        print '__scp_tar', host, lock, args
        src_files = args[0]
        dst_nodes = args[1]
        dst_save_path = args[2]
        
        scpcmd = 'scp -r'
        for tmp in src_files:
                scpcmd += ' ' + tmp
        scpcmd += ' ' + dst_nodes[0] + ':' 
        scpcmd += dst_save_path[0]
        print scpcmd

        client = get_ssh_client(host)
        stdin, stdout, stderr =  client.exec_command('mkdir -p ' + dst_save_path[0])
        stdin, stdout, stderr =  client.exec_command(scpcmd)
        print stderr.read()
        print stdout.read()
        client.close()
        lock.release()

def scp_tar(args):
        print args
        is_src_nodes = False
        is_dst_nodes = False
        is_src_files = False
        is_dst_save_path = False
        src_nodes = []
        src_files = []
        dst_nodes = []
        dst_save_path = []

        if len(args) == 0:
                print 'Error: args are zero!'
                exit(-1)

        for arg in args:
                if arg == '--src_nodes':
                        is_src_nodes = True
                        is_dst_nodes = False
                        is_src_files = False
                        is_dst_save_path = False
                elif arg == '--dst_nodes':
                        is_src_nodes = False
                        is_dst_nodes = True
                        is_src_files = False
                        is_dst_save_path = False
                elif arg == '--src_files':
                        is_src_nodes = False
                        is_dst_nodes = False
                        is_src_files = True
                        is_dst_save_path = False
                elif arg == '--dst_save_path':
                        is_src_nodes = False
                        is_dst_nodes = False
                        is_src_files = False
                        is_dst_save_path = True
                else:
                        if is_src_nodes == True:
                                src_nodes.append(arg)
                        elif is_dst_nodes == True:
                                dst_nodes.append(arg)
                        elif is_src_files == True:
                                src_files.append(arg)
                        elif is_dst_save_path == True:
                                dst_save_path.append(arg)

        print "src_nodes=", src_nodes, "src_files=", src_files
        print 'dst_nodes=', dst_nodes, 'dst_save_path=', dst_save_path
        if len(src_nodes) == 0 or len(dst_nodes) == 0 or \
           len(src_files) == 0 or len(dst_save_path) == 0:
                print 'Error: args are invalid!'
                exit(-1)

        __multi_thread (src_nodes, __scp_tar, src_files, dst_nodes, dst_save_path)

if __name__ == '__main__':
        is_password = False
        password_pos = ''
        password = ''
        global ssh_password

        if len(sys.argv) < 2:
                print 'Usage: python %s [--pack --nodes nodes --save_pack_path pathname '\
                      '--need_pack_files pathname] \n'\
                      '\t\t\t[--unpack nodes:/pathname] '\
                      '\n\t\t\t[--scp --src_nodes nodes --src_files pathname --dst_nodes nodes --dst_save_path pathname]'\
                      '\n\t\t\t--password ssh_password'\
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
                scp_tar(args)
        else:
                print 'Error: unknown option!'
                exit (-1)
