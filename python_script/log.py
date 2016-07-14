#!/usr/bin/python

import os
import logging
import logging.config

#log_conf = "./log.conf"
log_conf = "/root/git/my-scripts/python_script/log.conf"

try:
    os.mkdir('/var/log/my-scripts')
    os.system('touch /var/log/my-scripts/fast.log')
    os.system('touch /var/log/my-scripts/misc.log')
except:
    pass

logging.config.fileConfig(log_conf)
fastlog = logging.getLogger('fastCommandLoger')
misclog = logging.getLogger('miscLoger')

def test():
    fastlog.info('This is info message')
    fastlog.debug('This is DEBUG message')
    fastlog.warn('This is Warning message')
    fastlog.error('This is Error message')
    fastlog.critical('This is critical message')

    misclog.info('This is info message')
    misclog.debug('This is DEBUG message')
    misclog.warn('This is Warning message')
    misclog.error('This is Error message')
    misclog.critical('This is critical message')

if __name__ == "__main__":
    test()
