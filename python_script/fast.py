#!/usr/bin/python

import os
import sys
import thread

def __despatch_cmd(host, lock, cmd):
        cmd = "ssh " + host + " \"" + cmd + "\""
        print cmd
        os.system (cmd)
        lock.release()

def multi_thread(func, nodes, cmd):
        locks = []
        for host in nodes:
                try:
                        lock = thread.allocate_lock()
                        lock.acquire()
                        locks.append(lock)
                        thread.start_new_thread(func, (host, lock, cmd))
                except:
                        print "Error: unable to start thread!"
        for i in locks:
                while i.locked():
                        pass

def despatch_cmd(args):
        cmd = args[-1]
        config_file = './.fast.py.config'
        if os.path.isfile(config_file) == False:
                os.mknod(config_file)
        print cmd
        nodes = args[1:-1]
        if len(nodes) == 0:
                fp = open (config_file, 'rb')
                fnodes = fp.readline()
                nodes = fnodes.split(' ') 
        else:
                fp = open (config_file, 'wb')
                fp.truncate(0)
                for i in range(0, len(nodes)):
                        if i == len(nodes)-1:
                                fp.write(nodes[i])
                        else:
                                fp.write(nodes[i] + ' ')
        print nodes
        multi_thread(__despatch_cmd, nodes, cmd)

if __name__ == '__main__':
        if len(sys.argv) < 2:
                print "Usage: python " + __file__ + " your_nodes" + " your_commands\n" \
                      + "e.g. python " + __file__ + " 10.10.21.9{1,2,3} \"date\""
                print 'Notes: When python ' + __file__ + ' 10.10.21.9{1,2,3} date,\n'\
                      + 'it can record your_nodes in the .fast.py.config,\n'\
                      + 'and next time you can directly execute ./fast.py \"date\"'
                exit()
        
        despatch_cmd(sys.argv)
