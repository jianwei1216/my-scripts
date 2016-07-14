#!/usr/bin/python

# new
import argparse
import sys
import os
import ssh
import time
sys.path.append('./')
from log import fastlog

def get_ssh_client(host):
    global args

    try:
        client = ssh.SSHClient()
        client.set_missing_host_key_policy(ssh.AutoAddPolicy())
        client.connect(host, port=int(args.port), username='root', password=args.password)
    except Exception, e:
        print host, e
        fastlog.error ("%s:%s" % (host, e))
        return None
    return client

def __despatch_cmd(host, cmd):
    client = get_ssh_client(host)
    if client == None:
        return None

    stdin, stdout, stderr = client.exec_command(cmd)
    err = stderr.read()
    out = stdout.read()
    if len(out) > 0:
        print host, out,
        out = out[0:-1]
        fastlog.info ("%s: %s" % (host, out))
    if len(err) > 0:
        print host, err,
        err = err[0:-1]
        fastlog.error ("%s: %s" % (host, err))
    client.close()

def multi_fork(nodes, cmd):
    pids = []
    for host in nodes:
        try:
            pid = os.fork () 
            if pid == 0:
                # CHILD
                __despatch_cmd (host, cmd) 
                exit ()
            else:
                pids.append(pid)
        except Exception, e:
            print e
            fastlog.error ("%s" % e)
            return None

    for pid in pids:
        try:
            os.waitpid(pid, 0)
        except Exception, e:
            print e
            fastlog.error ("%s" % e)
            return None

def exec_commands():
    global args 
    cmd = args.command
    for arg in args.args:
        cmd += ' ' + arg
    multi_fork(args.nodes, cmd)

if __name__ == '__main__':
    global args, mylog
    config_file_path = os.path.expanduser('~') + '/.' + sys.argv[0].split('/')[1] + '.config'

    parser = argparse.ArgumentParser()
    parser.add_argument('--nodes', nargs='+', default=open(config_file_path, "ab+"))
    parser.add_argument('-p', '--port', nargs='?', type=int, default='22')
    parser.add_argument('--password', default=open(config_file_path, "ab+"))
    parser.add_argument('command')
    parser.add_argument('args', nargs=argparse.REMAINDER)
    args = parser.parse_args()

    if (type(args.nodes) == list and type(args.password) == file) or\
       (type(args.nodes) == file and type(args.password) == list):
        print 'Error: please give all args!'
        exit(-1)

    if type(args.nodes) == list and type(args.password) == str:
        fp = open (config_file_path, "wb+")
        fp.truncate(0)   
        fp.write(args.password + ' ')
        for i in range(0, len(args.nodes)):
            if i == len(args.nodes) - 1:
                fp.write(args.nodes[i])
            else:
                fp.write(args.nodes[i] + ' ')
        fp.write(' ' + str(args.port))
        fp.close()

    if type(args.nodes) == file and type(args.password) == file:
        fp = open (config_file_path, "rb")
        fargs = fp.readline()
        if len(fargs) == 0:
                print 'Fatal error: ./.fast.py.config is empty,'\
                      'please give all args!'
                exit(-1)
        args.password = fargs.split(' ')[0]
        args.nodes = fargs.split(' ')[1:-1]
        args.port = fargs.split(' ')[-1]
        fp.close()

    print args
    fastlog.info ('>>>>>>>>>>>>>>>>NEW COMMANDS START>>>>>>>>>>>>>>\nnargs = %s' % args)
    exec_commands()
    fastlog.info ('<<<<<<<<<<<<<<<<NEW COMMANDS END<<<<<<<<<<<<<<<<\n\n')
    exit()
