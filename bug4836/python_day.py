#!/usr/bin/python

import os
import sys
import socket
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

def write_day(write_count, day_count):
    # clear 
    dir_name = '/' + socket.gethostname()
    try:
        os.rmdir (dir_name)            
    except OSError, e:
        mylog.error (e)
    except WindowsError, e:
        mylog.error (e)
    except IOError, e:
        mylog.error (e)
    else:
        mylog.info ('RMDIR ' + dir_name + ' SUCCESS')

    for i in range(0, day_count):
        # mkdir 
        work_dir = dir_name + '/' + str(i)
        try:
            os.makedirs (work_dir)
        except WindowsError, e:
            mylog.error (e)
        except OSError, e:
            mylog.error (e)
        else:
            mylog.info ('MKDIR ' + work_dir + ' SUCCESS')

        # write
        write_cmd = 'python python_write_file.py ' + work_dir + ' ' + str(write_count) 
        mylog.info (write_cmd)
        try:
            os.system (write_cmd)
        except OSError, e:
            mylog.error (e)
        except WindowsError, e:
            mylog.error (e)
        else:
            mylog.info ('EXEC ' + write_cmd + ' SUCCESS')

        # delete
        delete_cmd = 'start /b python python_remove_file.py ' + work_dir + ' ' + str(write_count)
        mylog.info (delete_cmd)
        try:
            os.system (delete_cmd)
        except OSError, e:
            mylog.error (e)
        except WindowsError, e:
            mylog.error (e)
        else:
            mylog.info ('EXEC ' + delete_cmd + ' SUCCESS')

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage: python ' + __file__ + ' --node-count <COUNT> --all-size <GB> --day-count <COUNT>'
        exit(0)
    
    flags = {'--node-count':False, '--all-size':False, '--day-count':False}
    flags_tmp = flags.copy()

    node_count = 0
    all_size = 0
    day_count = 0
    args = sys.argv[1:]
    for arg in args:
        if arg == '--node-count':
            flags_tmp = flags.copy()
            flags_tmp['--node-count'] = True
        elif arg == '--all-size':
            flags_tmp = flags.copy()
            flags_tmp['--all-size'] = True
        elif arg == '--day-count':
            flags_tmp = flags.copy()
            flags_tmp['--day-count'] = True
        else:
            if flags_tmp['--node-count']:
                node_count = int(arg)
            elif flags_tmp['--all-size']:
                all_size = int(arg)
            elif flags_tmp['--day-count']:
                day_count = int(arg)
            else:
                print 'Error: invalid args!'
                exit(-1)
    mylog = log ('INFO')

    write_count = all_size*0.8/10/node_count
    mylog.info ('all_size=' + str(all_size) + 'GB, '
                + 'node_count=' + str(node_count) +
                ', ' + 'write_count=' + str(write_count)
                + ', ' + 'every_write=10GB')

    write_day(int(write_count), day_count)

