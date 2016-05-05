#!/usr/bin/python

#import thread
#import time

#def show_time(threadName, delay):
#    count = 0
#
#    while count < 5:
#        time.sleep(delay)
#        count += 1
#        print "%s: %s" % (threadName, time.ctime(time.time()))
#
#def thread_test():
#    try:
#        thread.start_new_thread(show_time, ("Thread-1", 2,))
#        thread.start_new_thread(show_time, ("Thread-2", 4,))
#    except:
#        print 'Error: unable to start thread'
#
#    while True:
#        pass
#
#thread_test()


import threading
import time

exitFlag = 0

class myThread (threading.Thread):
    def __init__(self, threadID, name, counter):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.counter = counter

    def run(self):
        print ("Starting " + self.name)
        show_time2(self.name, self.counter, 5)
        print ("Exiting " + self.name)

def show_time2(threadName, delay, counter):
    while counter:
        if exitFlag:
            thread.exit()
        time.sleep(delay)
        print "%s: %s" % (threadName, time.ctime(time.time()))
        counter -= 1

def  thread_test2():
    thread1 = myThread(1, "Thread-1", 1)
    thread2 = myThread(2, "Thread-2", 2)

    thread1.start()
    thread2.start()

    print "Exiting Main Thread!"

thread_test2()







