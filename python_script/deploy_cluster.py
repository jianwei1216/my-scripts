#!/usr/bin/python
# -*- coding:UTF-8 -*-

import sys
import os
import time
import ssh
import thread
import threading

mutex = threading.Lock()

def get_ssh_client(host):
        global ssh_passwd
        client = ssh.SSHClient()
        client.set_missing_host_key_policy(ssh.AutoAddPolicy())
        client.connect(host, port=22, username='root', password=ssh_passwd)
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

def peer_probe_nodes():
    pass

def mount_disks():
    pass

def mount_cluster():
    pass

def create_cluster():
    pass

def help_options():
    print 'Usage: python ' + __file__ + ' --nodes-count <COUNT> --node-NUMBER-devs <IP DEVICE> --service-type <DHT|AFR|EC>\n'\
          '                                  [--redundancy <COUNT>] [--disperse-data <COUNT>] [--password <PASSWD>]\n'

def help_info():
    help_options()
    pass

if __name__ == '__main__':
    global ssh_passwd
    nodes_count = 0
    nodes_list = []
    nodes_devs = []
    node_devs = []
    node_ip_flags = []
    service_type = ''
    disperse_data = 0
    redundancy = 0
    found = False
    ssh_passwd = ''

    if len(sys.argv) < 2:
        help_options()
        exit(0)

    flags = {'--nodes-count':False, '--service-type':False,'--redundancy':False, '--disperse-data':False, '--password':False}
    flags_tmp = flags.copy()

    args = sys.argv[1:]
    for arg in args:
        if '--nodes-count' == arg:
            flags_tmp = flags.copy()
            flags_tmp['--nodes-count'] = True
        else:
            if flags_tmp['--nodes-count']:
                nodes_count = int(arg)
                break

    if nodes_count <= 0:
        print '[line:%d] Error: nodes_count <= 0!' % (sys._getframe().f_lineno_)
        exit(-1)

    for i in range(1, (nodes_count + 1)):
        tmp = '--node-' + str(i) + '-devs'
        flags[tmp] = False
        node_ip_flags.append(tmp)
    
    flags_tmp = flags.copy()
    for arg in args:
        if '--nodes-count' == arg:
            flags_tmp = flags.copy()
            flags_tmp['--nodes-count'] = True
        elif '--service-type' == arg:
            flags_tmp = flags.copy()
            flags_tmp['--service-type'] = True
        elif '--redundancy' == arg:
            flags_tmp = flags.copy()
            flags_tmp['--redundancy'] = True
        elif '--disperse-data' == arg:
            flags_tmp = flags.copy()
            flags_tmp['--disperse-dat'] = True
        elif '--password' == arg:
            flags_tmp = flags.copy()
            flags_tmp['--password'] = True
        elif ('-devs' in arg):
            found = False
            for tmp in node_ip_flags:
                if tmp == arg:
                    flags_tmp = flags.copy()
                    flags_tmp[tmp] = True
                    found = True 
                    if len(node_devs) > 0:
                        nodes_devs.append(node_devs)
                    node_devs = []
            if found == False:
                print '[line:%d] Error: not found -devs in args' % (sys._getframe().f_lineno)
        else:
            if flags_tmp['--nodes-count']:
                nodes_count = int(arg)
            elif flags_tmp['--service-type']:
                service_type = arg
            elif flags_tmp['--redundancy']:
                redundancy = int(arg)
            elif flags_tmp['--disperse-data']:
                disperse_data = int(arg)
            elif flags_tmp['--password']:
                ssh_passwd = arg
            else:
                for tmp in node_ip_flags:
                    if flags_tmp[tmp]:
                        node_devs.append(arg)
    nodes_devs.append(node_devs)

    print nodes_devs
    for i in nodes_devs:
        print 'i:', i
        nodes_list.append(i[0])

    print 'service_type:', service_type
    print 'nodes_count:', nodes_count
    print 'redundancy:', redundancy
    print 'disperse_data:', disperse_data
    print 'nodes_list:', nodes_list
    for i in nodes_devs:
        print i 
