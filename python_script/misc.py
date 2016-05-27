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
        clean_hosts = 'node-manager stop; node-manager clear;'
        cmd_list.append(clean_hosts)
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
                           '/data /var/log/digioceanfs_gui/ /usr/sbin/digiocean* /usr/bin/digioceanfs-client '\
                           '/usr/bin/digioceanfs-gui /usr/bin/digioceanfs-reporter /usr/bin/digioceanfind '\
                           '/usr/bin/digi_partition /usr/bin/digioceanfs-volgen /usr/bin/digioceanfs-afrgen'
        cmd_list.append(clean_log_config)
        client = get_ssh_client(host)
        for cmd in cmd_list:
                print host, cmd
                stdin, stdout, stderr =  client.exec_command(cmd)
                err = stderr.read()
                out = stdout.read()
                if len(err) > 0:
                        print host, err
                if len(out) > 0:
                        print host, out
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
        __multi_thread(nodes, __clean_all_digioceanfs_env, '')

def __client_exec_commands (host, cmd_list, sleep=0):
        if host == '' or len(cmd_list) == 0:
                print 'Error: args are NULL'
                exit(-1)

        client = get_ssh_client(host)
        for cmd in cmd_list:
                print host, cmd
                stdin, stdout, stderr = client.exec_command(cmd)
                err = stderr.read()
                if len(err) > 0:
                        print host, err,
                if sleep != 0:
                        time.sleep(sleep)

        client.close()

def __add_trace_for_gluster(host, lock, args):
        cmd_list = []
        if len(args) != 5:
                print 'Error: args are zero'
                exit (-1)

        path_config = args[0]
        start_line = args[1]
        prev_module_line = args[2]
        next_module_name = args[3]
        volname = args[4]

        cmd1 = 'sed -i \'' + start_line + 'a\\volume ' + volname + '-trace\' ' + path_config  
        cmd_list.append(cmd1)
        cmd2 = 'sed -i \'' + str(int(start_line)+1) + 'a\\    type debug/trace\' ' + path_config
        cmd_list.append(cmd2)
        cmd3 = 'sed -i \'' + str(int(start_line)+2) + 'a\\    option log-file yes\' ' + path_config
        cmd_list.append(cmd3)
        cmd4 = 'sed -i \'' + str(int(start_line)+3) + 'a\\    option log-history yes\' ' + path_config
        cmd_list.append(cmd4)
        cmd5 = 'sed -i \'' + str(int(start_line)+4) + 'a\\    subvolumes '  + next_module_name + '\' ' + path_config
        cmd_list.append(cmd5)
        cmd6 = 'sed -i \'' + str(int(start_line)+5) + 'a\\end-volume\' ' + path_config
        cmd_list.append(cmd6)
        cmd7 = 'sed -i \'' + str(int(start_line)+6) + 'a\\zhangjianwei\' ' + path_config
        cmd_list.append(cmd7)
        cmd8 = 'sed -i \'s/zhangjianwei//g\' ' + path_config
        cmd_list.append(cmd8)
        cmd9 = 'sed -i \'' + prev_module_line + 'a\\    subvolumes ' + volname + '-trace\' ' + path_config
        cmd_list.append(cmd9)
        cmd10 = 'sed -i \'' + prev_module_line + 'd\' ' + path_config
        cmd_list.append(cmd10)

        __client_exec_commands(host, cmd_list)

        lock.release()

def add_trace_for_gluster(args):
        nodes = []
        start_line = ''
        prev_module_line = ''
        path_config = ''
        next_module_name = ''
        volname = ''
        flags_tmp = {'is_nodes':False, 'is_start_line':False, 'is_prev_module_line':False, \
                     'is_path_config':False, 'is_next_module_name':False, 'is_volname':False}

        flags = {}
        if len(args) == 0:
                print 'Error: args are zero!'
                exit(-1)

        for arg in args:
                if arg == '--nodes':
                        flags = flags_tmp.copy()
                        flags['is_nodes'] = True
                        print '\nflags_tmp:', flags_tmp
                        print '\nflags:', flags
                elif arg == '--next-module-name':
                        flags = flags_tmp.copy()
                        flags['is_next_module_name'] = True
                elif arg == '--prev-module-subvolumes-line':
                        flags = flags_tmp.copy()
                        flags['is_prev_module_line'] = True
                elif arg == '--path':
                        flags = flags_tmp.copy()
                        flags['is_path_config'] = True
                elif arg == '--start-line':
                        flags = flags_tmp.copy()
                        flags['is_start_line'] = True
                elif arg == '--volname':
                        flags = flags_tmp.copy()
                        flags['is_volname'] = True
                else:
                        if len(flags) == 0:
                                print '%d: Error: args are error' % (sys.__getframe().f_lineno)
                                exit(-1)
                        if flags['is_nodes'] == True:
                                nodes.append(arg)
                        elif flags['is_next_module_name'] == True:
                                next_module_name = arg
                        elif flags['is_prev_module_line'] == True:
                                prev_module_line = arg
                        elif flags['is_start_line'] == True:
                                start_line = arg
                        elif flags['is_path_config'] == True:
                                path_config = arg
                        elif flags['is_volname'] == True:
                                volname = arg
                        else:
                                print '%d: Error: args are error' % (sys.__getframe().f_lineno)
                                exit(-1)

        print 'nodes:', nodes
        print 'next_module_name:', next_module_name
        print 'prev_module_line:', prev_module_line
        print 'start_line:', start_line
        print 'path_config:', path_config
        print 'volname:', volname
        if len(nodes) == 0 or next_module_name == '' or prev_module_line == '' \
           or start_line == '' or path_config == '' or volname == '':
                   print '%d: Error: args are error' % (sys._getframe().f_lineno)
                   exit(-1)

        __multi_thread(nodes, __add_trace_for_gluster, path_config,\
                       start_line, prev_module_line, next_module_name, volname)

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

# calc avg
def calc_avg(args):
        allsum = 0.0
        count = 0
        avg = 0.0

        if len(args) == 0:
                print 'Error: args are zero!'
                exit(-1)

        for i in range(0, len(args)):
                if args[i] == '--filename':
                        del args[i]
                        break
        datafile = args[0]

        fp = open (datafile, 'r')

        while True:
                fr = fp.readline().strip('\n') 
                if len(fr) == 0:
                        break
                data = float(fr)
                allsum += data
                count += 1

        avg = allsum / count
        print 'count=%d, sum=%f, avg=%f\n' % (count, allsum, avg)

# help info
def help_option():
        print 'Usage: %s [--help] --password ssh_password\n'\
              '                 [--pack --nodes nodes_ip --save_pack_path pathname --need_pack_files pathname]\n'\
              '                 [--unpack --nodes nodes_ip --files pathname [--dir pathname]]\n'\
              '                 [--scp --src_nodes nodes_ip --src_files pathname --dst_nodes nodes_ip --dst_save_path pathname]\n'\
              '                 [--not-use-ssh-passwd --nodes nodes_ip] [--clean-all-digioceanfs-env --nodes nodes_ip]\n'\
              '                 [--add-trace-for-gluster --path configfile --nodes nodes_ip --start-line linenum\n'\
              '                  --next-module-name modulename --prev-module-subvolumes-line linenum --volname volumename]\n'\
              '                 [--calc-avg --filename datafile]\n'\
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
              '  --calc-avg                     give a data file that one digit takes up a raw and calculate\n'\
              '                                 sum, avg and count\n'\
              '  --add-trace-for-gluster        add\n'\
              'Here are some examples:\n' \
              '(1)' + __file__ + ' --scp --src_nodes 10.10.21.11{1,2,3,4,5} --src_files \'/root/10.10.21.11?_1464071514.tar.gz\' \n' \
              '             --dst_nodes 10.10.12.16 --dst_save_path /root/log/ --password 123456\n' \
              '(2)' + __file__ + ' --pack --nodes 10.10.21.11{1,2,3,4,5} --save_pack_path /root/\n' \
              '             --need_pack_files /var/log/digioceanfs/* /var/lib/digioceand/* --password 123456\n'\
              '(3)' + __file__ + ' --not-use-ssh-passwd --nodes 10.10.21.11{1,2,3,4,5} 10.10.12.16 --password 123456\n' \
              '(4)' + __file__ + ' --clean-all-digioceanfs-env --nodes 10.10.21.9{1,2,3} --password 123456\n'\
              '(5)' + __file__ + ' --calc-avg /root/log/mkdir_create_speed_test/lookup_seconds_dht_is_do_force-10.10.21.111\n'\
              '(6)' + __file__ + ' --add-trace-for-gluster --path /root/git/my-scripts/python_script/trusted-test.tcp-fuse.vol\n'\
              '             --nodes 10.10.21.111 --start-line 150 --next-module-name test-dht --prev-module-subvolumes-line 160\n'\
              '             --volname test --password 123456'

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

        if sys.argv[1] == '--calc-avg':
                calc_avg(sys.argv[2:])
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
        elif cmd == '--add-trace-for-gluster':
                add_trace_for_gluster(args)
        else:
                print 'Error: unknown option!'
                exit (-1)
