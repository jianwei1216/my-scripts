#!/usr/bin/python

import sys
import os
import time
import logging
import logging.handlers

LOG_FILE = '/fio_test.log'
mylog = ""

class log:
    global LOG_FILE
    logger = ""
    fmt = "[%(asctime)s] [%(filename)s:%(lineno)s %(levelname)s] %(message)s"
    formatter = ""

    def __init__(self, log_level):
        self.handler = logging.handlers.RotatingFileHandler(LOG_FILE,\
                                                    maxBytes=1024*1024*1024,\
                                                    backupCount = 5)
        self.formatter = logging.Formatter(self.fmt)
        self.handler.setFormatter(self.formatter)
        self.logger = logging.getLogger('zjw')
        self.logger.addHandler(self.handler)
        if log_level == 'DEBUG':
            self.logger.setLevel(logging.DEBUG)
        elif log_level == 'ERROR':
            self.logger.setLevel(logging.ERROR)
        else:
            self.logger.setLevel(logging.INFO)

    def debug(self, message):
        self.logger.debug(message)

    def info(self, message):
        self.logger.info(message)

    def error(self, message):
        self.logger.error(message)

def write_file(work_dir, write_cnt):
    begin_time = time.time()
    mylog.info ('The write test BEGIN_TIME is ' + str(begin_time) + 's')
    for i in range(1, write_cnt):
        now_time = time.time()
        diff_time = now_time - begin_time
        mylog.info ('The write test runtime is ' + str(diff_time) + 's')
        if diff_time >= 86400:
            mylog.info ('The write test run more than 24 hours and exit')
            break
        fio_cmd = 'fio --directory=' + work_dir + ' --thread --iodepth=1 '\
                  '--rw=randwrite --ioengine=windowsaio --bs=1m --size=1g '\
                  '--numjobs=10 --group_reporting --direct=1 --name=file' + str(i)
        mylog.info (fio_cmd)
        try:
            os.system(fio_cmd)
        except OSError, e:
            mylog.error (e)
        except WindowsError, e:
            mylog.error (e)
        else:
            mylog.info ('EXEC ' + fio_cmd + ' SUCCESS')

    end_time = time.time()
    mylog.info ('The write test END_TIME is ' + str(end_time) + 's')
    mylog.info ('The write test COST_TIME is ' + str((end_time-begin_time)/3600) + ' hours')

if __name__ == '__main__':
    mylog = log('INFO')

    if len(sys.argv) != 3:
        mylog.error ('args are invalid!')
        exit(-1)
    
    work_dir = sys.argv[1]
    write_cnt = int(sys.argv[2])
     
    write_file(work_dir, write_cnt)

