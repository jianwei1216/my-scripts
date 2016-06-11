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

def delete_file(work_dir, write_cnt):
    begin_time = time.time()
    mylog.info ('The delete test BEGIN_TIME is ' + str(begin_time) + 's')

    if not os.path.isdir (work_dir):
        return
    files = os.listdir (work_dir)
    try:
        for tmpfile in files:
            file_path = os.path.join(work_dir, tmpfile)
            if os.path.isfile(file_path):
                os.remove(file_path)
                mylog.info ('remove FILE ' + file_path)
            elif os.path.isdir(file_path):
                delete_file(file_path)
                mylog.info ('remove DIR ' + file_path)
        os.rmdir(work_dir)
    except Exception, e:
        mylog.error (e)

    end_time = time.time()
    mylog.info ('The delete test END_TIME is ' + str(end_time) + 's')
    mylog.info ('The delete test COST_TIME is ' + str((end_time-begin_time)/3600) + ' hours')

if __name__ == '__main__':
    mylog = log('INFO')

    if len(sys.argv) != 3:
        mylog.error ('args are invalid!')
        exit(-1)
    
    work_dir = sys.argv[1]
    write_cnt = int(sys.argv[2])
     
    delete_file (work_dir, write_cnt)

