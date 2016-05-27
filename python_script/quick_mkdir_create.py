#!/usr/bin/python
# -*- coding:UTF-8 -*-

import time
import os
import sys
import logging
import logging.handlers
import socket

TEST_DIR = "quick_mkdir_create"
LOG_FILE = '/var/log/digioceanfs/quick_mkdir_create.log'
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

def create_files(workdir, dir_count, one_dir_file_count):
    global hours
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
            # limit time
            if hours != -1:
                tmp_time = time.time()
                if (tmp_time - begin_create_time) > (hours * 3600):
                    mylog.info ('already run more than ' + str(hours) + ' hours')
                    exit(0)

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
        print 'Usage: ' + __file__ + ' --workdir' + ' --dir-count' + ' --file-count' + ' [--log-level]' + ' [--hours]\n'\
              + 'Description: This python script used for fast-create test.\n'\
              + '--workdir: work directory;\n'\
              + '--dir_count: need create directory count;\n'\
              + '--file_count: need create all file count;\n'\
              + '--LogLevel: DEBUG INFO ERROR, default is INFO(log_path=' + LOG_FILE + ')\n'\
              + '--hours: you want to run this py time, default not set hours\n'\
              + 'Note: one_dir_file_count = file_count / dir_count\n'\
              + 'TEST_DIRECTORY: workdir/quick_mkdir_create\n'
        exit(0)

    args = sys.argv[1:]
   
    workdir = ''
    log_level = ''
    dir_count = -1
    file_count = -1
    global hours
    hours = -1
    commands = ''

    flags = {'is_workdir':False, 'is_dir_count':False,\
             'is_file_count':False, 'is_log_level':False, 'is_hours':False}
    flags_tmp = flags.copy()

    for arg in args:
        if arg == '--workdir':
            flags_tmp = flags.copy()
            flags_tmp['is_workdir'] = True
        elif arg == '--dir-count':
            flags_tmp = flags.copy()
            flags_tmp['is_dir_count'] = True
        elif arg == '--file-count':
            flags_tmp = flags.copy()
            flags_tmp['is_file_count'] = True
        elif arg == '--log-level':
            flags_tmp = flags.copy()
            flags_tmp['is_log_level'] = True
        elif arg == '--hours':
            flags_tmp = flags.copy()
            flags_tmp['is_hours'] = True
        else:
            if flags_tmp['is_workdir']:
                workdir = arg
            elif flags_tmp['is_dir_count']:
                dir_count = int(arg)
            elif flags_tmp['is_file_count']:
                file_count = int(arg)
            elif flags_tmp['is_log_level']:
                log_level = arg
            elif flags_tmp['is_hours']:
                hours = int(arg)
            else:
                print '%d: Error: args are invalid' % (sys.__getframe().f_lineno)
                exit(-1)
    
    if workdir == '' or dir_count == -1 or file_count == -1:
        print '%d: Error: args are not enough' % (sys.__getframe().f_lineno)
        exit(-1)

    commands += sys.argv[0] + ' ' + workdir + ' dir_count=' + str(dir_count) + \
                ' file_count=' + str(file_count) + ' '

    if log_level == '':
        log_level = 'INFO'
    mylog = log(log_level)

    commands += 'log_levle=' + log_level
    if hours != -1:
        commands += ' hours=' + str(hours)

    if len(commands) != 0:
        mylog.info (commands)

    if os.path.exists(workdir) == False:
        print 'filepath:' + workdir + ' is not exists!'
        exit(0)
    
    # create test directory quick_mkdir_create
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

    one_dir_file_count = file_count / dir_count
    create_files(workdir, dir_count, one_dir_file_count)

