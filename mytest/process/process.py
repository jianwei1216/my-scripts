#!/usr/bin/python

from multiprocessing import Process, Queue, Pipe, Lock, Value, Array
import os

def info(title):
    print title
    print 'module name:', __name__
    if hasattr(os, 'getppid'): # only available on unix
        print 'parent process:', os.getppid()
    print 'process id:', os.getpid()

def f(name):
    info('function f')
    print 'hello', name

def process_test():
    p = Process(target=f, args=('bob', ))
    p.start()
    p.join()

# Queue
def f1(q):
    q.put([42, None, 'hello'])

def process_test1():
    q = Queue()
    p = Process(target=f1, args=(q,))
    p.start()
    print q.get()
    p.join()

# Pipe
def pipe_test(conn):
    conn.send([42, None, 'hello'])
    conn.close()
    
def process_pipe():
    parent_conn, child_conn = Pipe()
    p = Process(target=pipe_test, args=(child_conn,))
    p.start()
    print parent_conn.recv()
    p.join()
    parent_conn.close()

# Lock
def lock_test(l, i):
    l.acquire()
    print 'hello world', i
    l.release()

def process_lock():
    lock = Lock()
    for num in range (10):
        Process(target=lock_test, args=(lock, num)).start()

# share memory
def share_memory_test(n, a):
    n.value = 3.14
    for i in range(len(a)):
        a[i] = -a[i]

def process_share_memory_test():
    num = Value('d', 0.0)
    arr = Array('i', range(10))

    p = Process(target=share_memory_test, args=(num, arr))
    print 'debug1:', num.value, arr[:]
    p.start()
    print 'debug2:', num.value, arr[:]
    p.join()
    print 'debug3:', num.value, arr[:]

if __name__ == '__main__':
    #process_test1()
    #process_pipe()
    #process_lock()
    process_share_memory_test()


