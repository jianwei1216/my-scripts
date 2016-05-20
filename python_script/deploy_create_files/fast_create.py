#!/usr/bin/python
# -*- coding:UTF-8 -*-

import time
import os
import sys
import logging
import logging.handlers
import socket

TEST_DIR = "test_fast_create"
LOG_FILE = "/var/log/digioceanfs/test_fast_create.log"
mylog = ""

class log:
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

def create_files(workdir, dir_count, one_dir_file_count):
    # MKDIR HOSTNAME dir
    hostname = socket.gethostname()
    workdir = workdir + '/' + hostname
    if os.path.exists(workdir) == False:
        try:
            os.mkdir(workdir)
        except OSError, data:
            mylog.error(data)
            exit(-1)

    # begin create time
    begin_create_time = time.time()
    mylog.info("BEGIN_CREATE_TIME=" + str(begin_create_time))
    # create 100 dir
    for i in range(0, dir_count):
        dirname = workdir + "/newdir" + str(i)
        try:
            os.mkdir(dirname)
        except OSError, data:
            mylog.error(data)
        except IOError, data:
            mylog.error(data)
        else:
            mylog.info ('Mkdir ' + dirname + ' success!')
            # create 10000 files      
            for j in range(0, one_dir_file_count):
                filename = dirname + '/newfile' + str(j)
                try:
                    os.mknod(filename)
                except OSError, data:
                    mylog.error(data)
                except IOError, data:
                    mylog.error(data)
                else:
                    mylog.debug ('Create ' + filename + ' success!')

    # end create time
    end_create_time = time.time()
    mylog.info("END_CREATE_TIME=" + str(end_create_time))
    # speed how many time when create 1000000 files
    cost_create_time = end_create_time - begin_create_time
    mylog.info('COST_CREATE_TIME=' + str(cost_create_time))

def delete_files():
    pass
    # begin delete time
    # delete 1000000 files
    # end delete time
    # speed how many time when delete 1000000 files

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print 'Usage: ' + __file__ + ' workdir' + ' dir_count' + ' file_count' + ' [LogLevel]\n'\
              + 'Description: This python script used for fast-create test.\n'\
              + '--workdir: work directory;\n'\
              + '--dir_count: need create directory count;\n'\
              + '--file_count: need create all file count;\n'\
              + '--LogLevel: DEBUG INFO ERROR, default is INFO(log_path=' + LOG_FILE + ')\n'\
              + 'Note: one_dir_file_count = file_count / dir_count\n'\
              + 'TEST_DIRECTORY: workdir/test_fast_create\n'
        exit(0)

    workdir = sys.argv[1]
    if os.path.exists(workdir) == False:
        print 'filepath:' + workdir + ' is not exists!'
        exit(0)
    
    # create test directory test_fast_create
    workdir = workdir + '/' + TEST_DIR
    if os.path.exists(workdir) == False:
        try:
            os.mkdir(workdir)
        except OSError, data:
            print 'Create directory ' + workdir + 'failed', data
            mylog.error(data)
            exit(-1)
        else:
            print 'TEST_PATH=' + workdir

    if len(sys.argv) == 5:
        log_level = sys.argv[-1]
    else:
        log_level = 'INFO'
    mylog = log(log_level)

    dir_count = sys.argv[2] 
    file_count = sys.argv[3] 
    one_dir_file_count = int(file_count) / int(dir_count)
    create_files(workdir, int(dir_count), one_dir_file_count)

