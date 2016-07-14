#!/usr/bin/python
# -*- conding:UTF-8 -*-

import sys
import os
import ssh
import thread
import time
import argparse
import textwrap

# according ssh get a client for remote exec cmds, close after using
def get_ssh_client(host):
    global args
    client = ssh.SSHClient()
    client.set_missing_host_key_policy(ssh.AutoAddPolicy())
    client.connect(host, port=args.port, username=args.user, password=args.password)
    return client
        
# get multi threads
def __multi_thread(func):
    global args
    locks = []

    for host in args.nodes:
        lock = thread.allocate_lock()
        lock.acquire()
        locks.append(lock)
        thread.start_new_thread(func, (host, lock))

    for lock in locks:
        while lock.locked():
            pass

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

# clean up all cluster environment
def __clean_all_cluster_env(host, lock):
    global args
    cmd_list = []

    clusterfs = args.cluster_keyword + 'fs'
    clusterd = args.cluster_keyword + 'd'
    clusterfsd = args.cluster_keyword + 'fsd'
    cluster = args.cluster_keyword
    cluster_gui = args.cluster_keyword + '_gui'
    cluster_backup = args.cluster_keyword + '_backup'
    cluster_manager = args.cluster_keyword + '_manager'
    cluster_client = args.cluster_keyword + '_client'

    print '__clean_all_cluster_env()', host

    if args.light_cleanup:
        light_cleanup_cmd = 'rm -rf /var/log/' + clusterfs
        cmd_list.append(light_cleanup_cmd)
    else:
        clean_hosts = 'node-manager stop; node-manager clear;'
        cmd_list.append(clean_hosts)
        clean_process = 'killall ' + clusterfs + '; killall ' + clusterd + '; killall ' + clusterfsd + '; killall mongod;'\
                        'killall mdadm; ps -ef | grep node_manager | grep -v grep | awk \'{print $2}\' | xargs kill -9;'\
                        'ps -ef | grep ' + cluster_gui + '| grep -v grep | awk \'{print $2}\' | xargs kill -9;'
        cmd_list.append(clean_process)
        clean_rpm = 'for i in `rpm -qa | grep ' + cluster + '`; do rpm -e $i --nodeps; done'
        cmd_list.append(clean_rpm)
        clean_lib = 'cd /usr/lib64/; rm -rf ' + clusterfs + 'libgfapi.* libgfdb.* libgfxdr.* libgfrpc.* libgfchangelog.*'
        cmd_list.append(clean_lib)
        clean_mgmt = 'cd /usr/local/; rm -rf ' + cluster_gui + ' ' + cluster_backup + ' '  + cluster_manager + ' '\
                     + cluster_client + ' digiserver'
        cmd_list.append(clean_mgmt)
        clean_log_config = 'rm -rf /var/lib/' + clusterd + ' /var/log/' + clusterfs + \
                           ' /etc/' + clusterfs + ' /etc/' + cluster_manager + \
                           ' /etc/systemd/system/multi-user.target.wants/' + clusterfs + '-client.service '\
                           ' /etc/nginx/' + clusterfs + '.* /etc/sudoers.d/' + clusterfs + \
                           ' /var/log/' + cluster_manager + ' /usr/lib/ocf/resource.d/' + clusterfs + \
                           ' /usr/share/doc/'+ clusterfs + ' /usr/share/' + clusterfs + \
                           ' /usr/include/' + clusterfs + ' /usr/libexec/' + clusterfs + ' /var/run/' + cluster + \
                           ' /data /var/log/' + cluster_gui + ' /usr/sbin/' + cluster + '* /usr/bin/' + clusterfs + '-client '\
                           ' /usr/bin/' + cluster_gui + ' /usr/bin/' + clusterfs + '-reporter /usr/bin/' + cluster + 'find '\
                           ' /usr/bin/digi_partition /usr/bin/' + clusterfs + '-volgen /usr/bin/' + clusterfs + '-afrgen '\
                           ' /usr/bin/node-manager'
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

def clean_all_cluster_env():
    global args, command_remainder

    if (args.nodes == None) or (args.password == None):
        print 'Error: invalid arguments!!!\nExample:' + \
              command_remainder['clean_all_cluster_env']
        exit(-1)

    __multi_thread( __clean_all_cluster_env)

def __add_trace_module (host, lock):
    global args
    cmd_list = []

    path_config = args.configure_path
    start_line = args.start_line
    need_trace_vol_name = args.need_trace_volume_name
    volname = args.volname
    subvolume_trace_name = volname + '-trace'
    need_subvolumes_name = 'subvolumes ' + need_trace_vol_name
    need_replace_trace_name = 'subvolumes ' + subvolume_trace_name
    
    cmd1 = 'sed -i \'' + start_line + 'a\\volume ' + subvolume_trace_name + '\' ' + path_config  
    cmd_list.append(cmd1)
    cmd2 = 'sed -i \'' + str(int(start_line)+1) + 'a\\    type debug/trace\' ' + path_config
    cmd_list.append(cmd2)
    cmd3 = 'sed -i \'' + str(int(start_line)+2) + 'a\\    option log-file yes\' ' + path_config
    cmd_list.append(cmd3)
    cmd4 = 'sed -i \'' + str(int(start_line)+3) + 'a\\    option log-history yes\' ' + path_config
    cmd_list.append(cmd4)
    cmd9 = 'sed -i \'s/' + need_subvolumes_name + '/' + need_replace_trace_name + '/g\' ' + path_config
    cmd_list.append(cmd9)
    cmd5 = 'sed -i \'' + str(int(start_line)+4) + 'a\\    '  + need_subvolumes_name + '\' ' + path_config
    cmd_list.append(cmd5)
    cmd6 = 'sed -i \'' + str(int(start_line)+5) + 'a\\end-volume\' ' + path_config
    cmd_list.append(cmd6)
    cmd7 = 'sed -i \'' + str(int(start_line)+6) + 'a\\zhangjianwei\' ' + path_config
    cmd_list.append(cmd7)
    cmd8 = 'sed -i \'s/zhangjianwei//g\' ' + path_config
    cmd_list.append(cmd8)

    __client_exec_commands(host, cmd_list)

    lock.release()

def add_trace_module():
    global args, command_remainder

    if (args.nodes == None) or (args.start_line == None) \
            or (args.need_trace_volume_name == None) \
            or (args. volname == None) or (args.password == None):
        print 'Error: invalid arguments!!!\nExample: ' + \
              command_remainder['add_trace_module']
        exit(-1)

    __multi_thread(__add_trace_module)

# ssh non-password login
def not_use_ssh_passwd():
    global args
    global command_remainder
    id_rsa_pub = ''

    if (args.nodes==None) or (args.password==None):
        print 'Error: invalid arguments!!!\nExample: ' +\
              command_remainder['not_use_ssh_passwd']
        exit(-1)

    for host in args.nodes:
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
    for host in args.nodes:
        client = get_ssh_client(host)
        echocmd = 'echo \'' + id_rsa_pub + '\' >> /root/.ssh/authorized_keys'
        print echocmd
        stdin, stdout, stderr = client.exec_command(echocmd)
        if len(stderr.read()) > 0:
                print stderr.read()
        client.close()
    print 'success!'

def build_mongodb():
    global args
    global command_remainder
    master_node = ''
    cmd = '/usr/bin/python'
    prefix_str = 'test'
    
    if (args.digi_mongodbdeploy_path == None) or \
            (args.master_node == None) or (args.slave_nodes == None):
        print 'Error: invalid arguments!\nExample: ' + \
              command_remainder['build_mongodb'] 
        exit(-1)

    # python /usr/local/digioceanfs_manager/utils/digi-mongodbdeploy.pyc domain create 
    # test#10.10.21.115 test2#10.10.21.116 test3#10.10.21.91 10.10.21.115 
    exec_cmd = cmd + ' ' + args.digi_mongodbdeploy_path + ' domain create ' + prefix_str + '#' + args.master_node + ' '
    for i in range(0, len(args.slave_nodes)):
            exec_cmd += prefix_str + str(i) + '#' + args.slave_nodes[i] + ' '
    exec_cmd += args.master_node
    print exec_cmd
    os.system (exec_cmd)

    # python /usr/local/digioceanfs_manager/utils/digi-mongodbdeploy.pyc create rs0
    # replset#10.10.21.115#10.10.21.116#10.10.21.91 10.10.21.115
    exec_cmd = cmd + ' ' + args.digi_mongodbdeploy_path + ' create rs0 replset#' + args.master_node
    for slave_node in args.slave_nodes:
            exec_cmd += '#' + slave_node
    exec_cmd += ' ' + args.master_node
    print exec_cmd
    os.system (exec_cmd)

# main
if __name__ == '__main__':
    global args
    global command_remainder
    sys_argv_0 = sys.argv[0]

    command_remainder = {}
    command_remainder = {'build_mongodb':sys_argv_0 + ' --build-mongodb --master-node 10.10.21.111 --slave-nodes 10.10.21.11{2,3,4,5}',\
                         'add_trace_module':sys_argv_0 + ' --add-trace-module --nodes 10.10.21.111 --configure-path /var/lib/digioceand/vols/test'\
                         '/trusted-test.tcp-fuse.vol --start-line 150 --need-trace-volume-name test-dht --volname test --password 123456',\
                         'clean_all_cluster_env':sys_argv_0 + ' --clean-all-cluster-env --nodes 10.10.21.9{1,2,3} --password 123456',\
                         'not_use_ssh_passwd':sys_argv_0 + ' --not-use-ssh-passwd --nodes 10.10.21.11{1,2,3,4,5} 10.10.12.16 --password 123456'}
    #print 'debug command_remainder', command_remainder

    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter,\
                                     epilog=textwrap.dedent('(1) ' + command_remainder['build_mongodb'] + '\n'     \
                                                            '(2) ' + command_remainder['not_use_ssh_passwd'] +'\n' \
                                                            '(3) ' + command_remainder['add_trace_module'] + '\n'  \
                                                            '(4) ' + command_remainder['clean_all_cluster_env']))
    parser.add_argument ('--clean-all-cluster-env', action='store_true', help='cleanup all the cluster env on the specified nodes')
    parser.add_argument ('--cluster-keyword', nargs=1, default='digiocean', help='gluster or digiocean (default is digiocean)')
    parser.add_argument ('--light-cleanup', action='store_true', help='only delete the /var/log/digioceanfs')

    parser.add_argument ('--not-use-ssh-passwd', action='store_true', help='ssh free password login on specified nodes')

    parser.add_argument ('--add-trace-module', action='store_true', help='add trace module for cluster')
    parser.add_argument ('--configure-path', nargs=1, help='the cluster configure absolute path')
    parser.add_argument ('--start-line', nargs=1, type=int, help='the start line to add trace module')
    parser.add_argument ('--need-trace-volume-name', nargs=1, type=str, help='the module\'s name that you want to debug')
    parser.add_argument ('--volname', nargs=1, type=str, help='the cluster service name')

    parser.add_argument ('--build-mongodb', action='store_true', help='quick create a mongodb cluster')
    parser.add_argument ('--master-node', type=str, help='the master node of mongodb cluster')
    parser.add_argument ('--slave-nodes', nargs='+', help='the slave nodes of mongodb cluster')
    parser.add_argument ('--digi-mongodbdeploy-path', nargs=1, type=str, default='/usr/local/digioceanfs_manager/utils/digi-mongodbdeploy.pyc')

    parser.add_argument ('--nodes', nargs='+', help='need to operate nodes IP')
    parser.add_argument ('-p', '--port', nargs='?', type=int, default=22, help='ssh port')
    parser.add_argument ('--user', default='root', help='ssh user name')
    parser.add_argument ('--password', type=str, help='ssh password of the specified nodes')

    args = parser.parse_args ()
    #print 'debug-args:', args

    #Namespace(add_trace_module=False,
    #          build_mongodb=False,
    #          clean_all_cluster_env=False,
    #          cluster_keyword='digiocean',
    #          configure_path=None,
    #          master_node=None,
    #          need_trace_volume_name=None,
    #          nodes=None,
    #          not_use_ssh_passwd=False,
    #          password=None,
    #          port=22,
    #          slave_nodes=None,
    #          start_line=None,
    #          volname=None)
    #          digi_mongodbdeploy_path=
    #          light_cleanup=None
    
    if args.add_trace_module:
        add_trace_module()
    elif args.build_mongodb:
        build_mongodb()
    elif args.clean_all_cluster_env:
        clean_all_cluster_env()
    elif args.not_use_ssh_passwd:
        not_use_ssh_passwd()
    else:
        #print 'Error: unkown keyword!!!'
        parser.print_usage()
