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
        print cmd
        nodes = args[1:-1]
        print nodes
        multi_thread(__despatch_cmd, nodes, cmd)

if __name__ == '__main__':
        if len(sys.argv) < 2:
                print "python " + __file__ + " your_nodes" + " your_commands\n" \
                      + "e.g. python " + __file__ + " node-1 node-2 node-3 \"date\"\n"
                exit()
        
        despatch_cmd(sys.argv)
