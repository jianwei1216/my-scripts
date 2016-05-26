#!/usr/bin/python
# -*- conding:UTF-8 -*-

import sys
import os
import ssh
import thread
import time

# according ssh get a client for remote exec cmds, close after using
def get_ssh_client(host):
        global ssh_password
        client = ssh.SSHClient()
        client.set_missing_host_key_policy(ssh.AutoAddPolicy())
        client.connect(host, port=22, username='root', password=ssh_password)
        return client
        
# get multi threads
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

# compress files to .tar.gz
def __pack_tar(host, lock, args):
        cmd_list = []
        save_pack_path = args[0]
        need_pack_files = args[1]
        time_suffix = args[2]
        log_pack_path = 'digioceanfs.' + host
        tmp_pack_path = save_pack_path[0] + '/' + log_pack_path
        mkdir_cmd = 'mkdir -p ' + tmp_pack_path
        cmd_list.append(mkdir_cmd)
        cp_cmd = 'cp -rf'
        for filepath in need_pack_files:
                cp_cmd += ' ' + filepath
        cp_cmd += ' ' + tmp_pack_path
        cmd_list.append(cp_cmd)
        packcmd = 'cd ' + save_pack_path[0] + ';tar -czvf ' + \
                  'digioceanfs_' + host + '_' + time_suffix + '.tar.gz' + ' ' + log_pack_path
        cmd_list.append(packcmd)
        client = get_ssh_client(host)
        for cmd in cmd_list:
                print host, cmd
                stdin, stdout, stderr = client.exec_command(cmd)
                err = stderr.read()
                if len(err) > 0:
                        print host, err,
        client.close()
        lock.release()

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
        timestamp = str(int(time.time()))
        __multi_thread(need_pack_nodes, __pack_tar, save_pack_path, need_pack_files, timestamp)

# decopress tar.gz
def unpack_tar(args):
        pass

# remote copy files
def __scp_tar(host, lock, args):
        print '__scp_tar', host, lock, args
        src_files = args[0]
        dst_nodes = args[1]
        dst_save_path = args[2]
        
        scpcmd = 'scp -r'
        for tmp in src_files:
                scpcmd += ' ' + tmp
        scpcmd += ' ' + dst_nodes[0] + ':' 
        scpcmd += dst_save_path[0]

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

# clean up all digioceanfs environment
def __clean_all_digioceanfs_env(host, lock, args):
        cmd_list = []
        print '__clean_all_digioceanfs_env()', host
        clean_process = 'killall digioceanfs; killall digioceand; killall digioceanfsd; killall mongod;'\
                        'killall mdadm; ps -ef | grep node_manager | grep -v grep | awk \'{print $2}\' | xargs kill -9;'\
                        'ps -ef | grep digioceanfs_gui | grep -v grep | awk \'{print $2}\' | xargs kill -9;'
        cmd_list.append(clean_process)
        clean_rpm = 'for i in `rpm -qa | grep digioceanfs`; do rpm -e $i --nodeps; done'
        cmd_list.append(clean_rpm)
        clean_lib = 'cd /usr/lib64/; rm -rf digioceanfs libgfdb.* libgfxdr.* libgfrpc.* libgfchangelog.*'
        cmd_list.append(clean_lib)
        clean_mgmt = 'cd /usr/local/; rm -rf digioceanfs_backup digioceanfs_manager '\
                     'digioceanfs_client digioceanfs_gui digiserver'
        cmd_list.append(clean_mgmt)
        clean_log_config = 'rm -rf /var/lib/digioceand /var/log/digioceanfs '\
                           '/etc/digioceanfs /etc/digioceanfs_manager '\
                           '/etc/systemd/system/multi-user.target.wants/digioceanfs-client.service '\
                           '/etc/nginx/digioceanfs.* /etc/sudoers.d/digioceanfs '\
                           '/var/log/digioceanfs_manager/ /usr/lib/ocf/resource.d/digioceanfs '\
                           '/usr/share/doc/digioceanfs /usr/share/digioceanfs '\
                           '/usr/include/digioceanfs /usr/libexec/digioceanfs /var/run/digiocean '\
                           '/data /var/log/digioceanfs_gui/'
        cmd_list.append(clean_log_config)
        client = get_ssh_client(host)
        for cmd in cmd_list:
                print cmd
                stdin, stdout, stderr =  client.exec_command(cmd)
                print stderr.read()
                print stdout.read()
        client.close()
        lock.release()

def clean_all_digioceanfs_env(args):
        nodes = []
        if len(args) == 0:
                print 'Error: args are zero!'
                exit(-1)
        
        for i in range(0, len(args)):
                if args[i] == '--nodes':
                        del args[i]
                        break

        nodes = args
        print 'debug:', nodes
        __multi_thread(nodes, __clean_all_digioceanfs_env, '')

# ssh non-password login
def not_use_ssh_passwd(args):
        id_rsa_pub = ''
        nodes = []
        is_nodes = False

        if len(args) == 0:
                print 'Error: args are zero!'
                exit(-1)

        for i in range(0, len(args)):
                if args[i] == '--nodes':
                        del args[i]
                        break
        nodes = args

        for host in nodes:
                client = get_ssh_client(host)
                #stdin, stdout, stderr = client.exec_command('rm -rf /root/.ssh/authorized_keys')
                #if len(stderr.read()) > 0:
                        #print stderr.read()
                stdin, stdout, stderr = client.exec_command('cat /root/.ssh/id_rsa.pub')
                if len(stderr.read()) > 0:
                        print stderr.read()
                id_rsa_pub += stdout.read()
                client.close()

        print id_rsa_pub
        for host in nodes:
                client = get_ssh_client(host)
                echocmd = 'echo \'' + id_rsa_pub + '\' >> /root/.ssh/authorized_keys'
                print echocmd
                stdin, stdout, stderr = client.exec_command(echocmd)
                if len(stderr.read()) > 0:
                        print stderr.read()
                client.close()
        print 'success!'

# help info
def help_option():
        print 'Usage: %s [--help] --password ssh_password\n'\
              '                 [--pack --nodes nodes_ip --save_pack_path pathname --need_pack_files pathname]\n'\
              '                 [--unpack --nodes nodes_ip --files pathname [--dir pathname]]\n'\
              '                 [--scp --src_nodes nodes_ip --src_files pathname --dst_nodes nodes_ip --dst_save_path pathname]\n'\
              '                 [--not-use-ssh-passwd --nodes nodes_ip] [--clean-all-digioceanfs-env --nodes nodes_ip]'\
              % (__file__)

def help_info():
        help_option()
        print ''
        print 'Mandatory arguments.\n' \
              '  --help                         display this help and exit\n' \
              '  --scp                          login the specified source host(--src_nodes) and\n' \
              '                                 copy the specified files(--src_files) to destination\n' \
              '                                 save directory(--dst_save_path) on the destination\n' \
              '                                 host(--dst_nodes)\n' \
              '  --pack                         login the specified host(--nodes) and compress the\n' \
              '  --unpack                       not implemented, cannot use\n' \
              '                                 specified files(--need_pack_files) to the specified\n' \
              '                                 directory(--save_pack_path), the compressed filename\n' \
              '                                 is host_currentTimeSeconds.tar.gz\n' \
              '  --not-use-ssh-passwd           used to implement non-password ssh login\n'\
              '                                 login the specified host(--nodes) and get all nodes\n' \
              '                                 /root/.ssh/id_rsa.pub keys, write to all nodes\'s\n' \
              '                                 /root/.ssh/authorized_keys. (Note: if exec the cmd, but\n' \
              '                                 ssh login host still need to input password, in case, please\n' \
              '                                 manual exec ssh-keygen on all nodes and re-exec the cmd!)\n' \
              '  --clean-all-digioceanfs-env    loggin the specified host(--nodes) and clean up all digioceanfs\n' \
              '                                 related process(MGMT/digioceanfs), library and configure files\n' \
              '  --password                     used to login the specified host, all nodes\'s password must\n' \
              '                                 be same, because only recieve one password! (Note: all that need\n' \
              '                                 to login, must specify this option!\n' \
              'Here are some examples:\n' \
              '(1)' + __file__ + ' --scp --src_nodes 10.10.21.11{1,2,3,4,5} --src_files \'/root/10.10.21.11?_1464071514.tar.gz\' \n' \
              '             --dst_nodes 10.10.12.16 --dst_save_path /root/log/ --password 123456\n' \
              '(2)' + __file__ + ' --pack --nodes 10.10.21.11{1,2,3,4,5} --save_pack_path /root/\n' \
              '             --need_pack_files /var/log/digioceanfs/* /var/lib/digioceand/* --password 123456\n'\
              '(3)' + __file__ + ' --not-use-ssh-passwd --nodes 10.10.21.11{1,2,3,4,5}\n' \
              '(4)' + __file__ + ' --clean-all-digioceanfs-env --nodes 10.10.21.9{1,2,3} --password 123456\n'


# main
if __name__ == '__main__':
        is_password = False
        password_pos = ''
        password = ''
        global ssh_password

        if len(sys.argv) < 2:
                help_option()
                exit (-1)

        if sys.argv[1] == '--help':
                help_info()      
                exit(0)

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
        elif cmd == '--not-use-ssh-passwd':
                not_use_ssh_passwd(args)
        elif cmd == '--clean-all-digioceanfs-env':
                clean_all_digioceanfs_env(args)
        else:
                print 'Error: unknown option!'
                exit (-1)
