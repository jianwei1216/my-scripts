#!/usr/bin/python
# -*- coding: UTF-8 -*-

import time
import os
import sys
import logging
import logging.handlers
import socket
import commands
import glob

LOG_FILE = "/var/log/digioceanfs/create_24_hours_and_del_24_hours.log"
mylog = ""

#mylog
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

#writefiles
def write_files(write_cnt, workdir):
    begin_write_time = time.time()
    for i in range(0, write_cnt):
        now = time.time()
        diff = now - begin_write_time
        if diff >= 86400:
            mylog.info ('time=' str(diff) + ', write ' + workdir + \
                        ' more than 24 hours, exit')
            break
        # one write 10 files and all is 11GB
        cmds = 'fio --directory=' + workdir +' \
                    --thread --iodepth=1 \
                    --rw=randwrite \
                    --ioengine=sync \
                    --bs=1m --size=1m \
                    --numjobs=10 \
                    --group_reporting \
                    --direct=1 \
                    --name=file' + str(i)
        exit_status, results = commands.getstatusoutput(cmds)
        mylog.info('exit_status=' + \
                   str(exit_status) + '\n' + \
                   str(results))

#delete files
def del_files(write_cnt, workdir):
    begin_del_time = time.time()
    for i in range(0, write_cnt):
        del_now = time.time()
        del_diff = del_now - begin_del_time
        if del_diff > 86400:
            mylog.info ('time=' str(diff) + \
                        ', delete ' + workdir + \
                        ' more than 24 hours, exit')
            break
        # one delete workdir/file*
        wildcard_path = workdir + '/file' + str(i) + '*'
        for path in glob.glob(wildcard_path):
            try:
                os.remove(path)
            except OSError, error_data:
                mylog.error(error_data)

#control write_days and delete days
def write_and_delete(days, write_cnt, workdir):
    # delete
    hostname = socket.gethostname()
    for i in range(0, days):
        #mkdir days dir
        #write
        #delete

if __name__ == '__main__':
    print 'Hello world!'
    log_level = 'INFO'
    mylog = log(log_level)
    write_files(1, 'test')
    del_files(1, 'test')
