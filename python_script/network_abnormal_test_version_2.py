#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import time
import logging
import logging.handlers
import commands

LOG_FILE = '/var/log/digioceanfs/' + __file__.split('/')[-1].split('.')[0] + '.log'
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


# 创建2+1的纠删码卷，ec(16 * (2+1))

def create_5_level_directory(no):
    global workdir
    dirs = ''
    dirs += workdir
    dirs += '/five-' + str(int(time.time())) + '-' + str(no) + '/'
    try:
        os.mkdir(dirs)
    except OSError, e:
        mylog.info('Five-level-dir Failed:' + dirs)
        mylog.error(e)
        return -1
    mylog.info('Five-level-dir:' + dirs)
    return dirs

def mkdir_create_some_directories_and_files(now_dir):
    for i in range(0, 1000):
        try:
            dirname = now_dir + 'newdir-' + str(int(time.time())) + '-' + str(i)
            os.mkdir(dirname)
            mylog.debug('Create dir ' + dirname + ' success!')
            filename = now_dir + 'newfile-' + str(int(time.time())) + '-' + str(i)
            os.mknod(filename)
            mylog.debug('Create file ' + filename + ' success!')
        except OSError, e:
            mylog.error(e)
        except IOError, e:
            mylog.error(e)

def close_network_card_software(host, ethno, times=1):
    # ssh host "ifconfig ethno down"
    cmd = 'ssh ' + host + ' \" ifconfig ' + ethno + ' down\"'
    mylog.info(cmd)
    for i in range(0, times):
        out_status, result = commands.getstatusoutput(cmd)

def restore_network_card_software(host, ethno, times=1):
    cmd = 'ssh ' + host + ' \" ifconfig ' + ethno + ' up\"'
    mylog.info(cmd)
    for i in range(0, times):
        out_status, result = commands.getstatusoutput(cmd)

def lookup_fifth_level_directory(now_dir):
    cmd = 'ls ' + now_dir
    mylog.info(cmd)
    out_status, result = commands.getstatusoutput(cmd)

def get_volume_status():
    out_status, result = commands.getstatusoutput('digiocean volume status')
    mylog.info(result)
    out_status, result = commands.getstatusoutput('digiocean peer status')
    mylog.info(result)

def bug6284_test():
    global workdir
    global dir_count
    global hours
    global hosts_list
    global network_card_name
    now_time = 0
    count = 0
    now_dir = ''

    begin_time = time.time()
    mylog.info('BEGIN_TIME:' + str(begin_time))

    while True:
        if count >= dir_count and dir_count != 1:
            mylog.info('Create 5 level dirs count more than ' + str(dir_count) + ' and exit!')
            break

        now_time = time.time()
        if (now_time - begin_time) > (hours * 3600):
            mylog.info('Process already run more than ' + str(hours) + ' and exit!')
            break
        # 1.在挂载点创建创建五级目录，
        #get_volume_status()
        if dir_count == 1 and count == 0:
            now_dir = create_5_level_directory(count)
            if now_dir == -1:
                continue
                #mylog.error('Create 5 level dirs failed')
                #exit(-1)

        if dir_count != 1:
            now_dir = create_5_level_directory(count)
            if now_dir == -1:
                continue
                #mylog.error('Create 5 level dirs failed')
                #exit(-1)

        count += 1

        # 2.在第五级目录下创建文件删除文件等等一些操作使第五级目录的version值（扩展属性）发生改变，
        #get_volume_status()
        mkdir_create_some_directories_and_files(now_dir)

        # 3.然后拔掉一个机器上的网线，
        close_network_card_software(hosts_list[0], network_card_name[0], 3)

        # 4.再次在第五级目录下执行一些操作，使第五级目录version发生改变，
        #get_volume_status()
        mkdir_create_some_directories_and_files(now_dir)

        # 5.然后将拔掉网线的机器恢复正常没有执行修复操作
        restore_network_card_software(hosts_list[0], network_card_name[0], 3)
        time.sleep(5)

        # 6.再次在第五级目录下执行一些操作，使第五级目录version发生改变，
        #get_volume_status()
        mkdir_create_some_directories_and_files(now_dir)

        # 8.然后ls查看第五级目录下的信息，发现卡主现象，但是不是死锁，其上四级目录都是正常的，没有问题，log信息显示在附件中。
        #get_volume_status()
        lookup_fifth_level_directory(now_dir)
        time.sleep(300)

    end_time = time.time()
    mylog.info('END_TIME:' + str(end_time))
    mylog.info('COST_TIME:' + str((end_time - begin_time)/3600) + ' hours')

def help_options():
    print 'Usage: ' + __file__ + ' --workdir --dir-count --operate-hosts --network-card-name [--hours] [--log-level] [--help]'

def help_info():
    help_options()

    print ''
    print 'Mandatory arguments.\n' \
          '  --workdir \n'\
          '  --dir-count          if dir_count == 1, only operate one dir, if dir_count > 1,\n'\
          '                       when operate dir_count over, then exit\n'\
          '  --hours              if hours set and dir_count > 1, then dir_count over and exit,\n'\
          '                       or hours over and exit!\n'\
          '  --operate-hosts      can multi hosts, must be second network card ip!\n'\
          '  --network-card-name  e.g eth0/eth1/eno1/eno2\n'\
          '  --help               display this help and exit\n' \
          '  --log-level          DEBUG/INFO(default)/ERROR\n'\

if __name__ == '__main__':
    cmds = ''
    global workdir
    global hours
    global dir_count
    global hosts_list
    global network_card_name
    workdir = ''
    log_level = ''
    hours = -1
    dir_count = -1
    hosts_list = []
    network_card_name = []
    flags = {'--workdir':False, '--log-level':False, '--hours':False,\
             '--dir-count':False, '--operate-hosts':False, '--network-card-name':False}
    flags_tmp = flags.copy()

    if len(sys.argv) < 2:
        help_options()
        exit(-1)

    if sys.argv[1] == '--help':
        help_info()
        exit(0)

    cmds += 'python ' + sys.argv[0] + ' '
    args = sys.argv[1:]
    for arg in args:
        if arg == '--workdir':
            flags_tmp = flags.copy()
            flags_tmp['--workdir'] = True
        elif arg == '--log-level':
            flags_tmp = flags.copy()
            flags_tmp['--log-level'] = True
        elif arg == '--hours':
            flags_tmp = flags.copy()
            flags_tmp['--hours'] = True
        elif arg == '--dir-count':
            flags_tmp = flags.copy()
            flags_tmp['--dir-count'] = True
        elif arg == '--operate-hosts':
            flags_tmp = flags.copy()
            flags_tmp['--operate-hosts'] = True
            cmds += '--operate-hosts '
        elif arg == '--network-card-name':
            flags_tmp = flags.copy()
            flags_tmp['--network-card-name'] = True
            cmds += '--networkd-card-name '
        else:
            if flags_tmp['--workdir']:
                workdir = arg
                cmds += '--workdir ' + workdir + ' '
            elif flags_tmp['--log-level']:
                log_level = arg
                cmds += '--log-level ' + log_level + ' '
            elif flags_tmp['--hours']:
                hours = int(arg)
                cmds += '--hours ' + arg + ' '
            elif flags_tmp['--dir-count']:
                dir_count = int(arg)
                cmds += '--dir-count ' + arg + ' '
            elif flags_tmp['--operate-hosts']:
                hosts_list.append(arg)
                cmds += arg + ' '
            elif flags_tmp['--network-card-name']:
                network_card_name.append(arg) 
                cmds += arg
            else:
                print '[line:%d] Error: args are invalid!'\
                      % (sys._getframe().f_lineno)
                exit(22)

    if workdir == '' or dir_count == -1:
        print '[line:%d] Error: args are invalid!'\
              % (sys._getframe().f_lineno)
        exit(22)

    if log_level == '':
        log_level = 'INFO' 
    mylog = log(log_level)

    workdir += '/one-' + str(int(time.time()))
    workdir += '/two-' + str(int(time.time()))
    workdir += '/three-' + str(int(time.time()))
    workdir += '/four-' + str(int(time.time()))
    mylog.info('Four-level-dir:' + workdir)
    os.makedirs(workdir)

    mylog.info(cmds)
    print 'workdir:' , workdir
    print 'hosts_list:',hosts_list
    print 'network_card_name:', network_card_name
    print 'hours:', str(hours)
    print 'dir_count:', str(dir_count)
    bug6284_test() 
