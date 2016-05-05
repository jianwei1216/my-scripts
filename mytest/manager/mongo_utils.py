# -*- coding: utf-8 -*-

import os
import sys
import time
import pymongo

import settings
from manager_utils import *

currpath = os.path.join(os.getcwd(),os.path.dirname(__file__))
if not currpath in sys.path:
    sys.path.append(currpath)

manager_path = os.path.join(currpath[:currpath.rfind('utils')])
if not manager_path in sys.path:
    sys.path.append(manager_path)

ACOLID = None

#####################################################
    # write collection data to mongo database
    # such as cpu/memory/fan/battery/process...
#####################################################
def write_event_to_db(data,aflag=False):
    ### 多事件同时操作
    # data: dict;
    #       要写入监控集合或报警集合的数据信息
    # aflag: True or False
    #       标识该数据信息为监控信息或报警信息。如果为报警信息，直接写入报警集合，不写入监控集合。
    threshold = {}
    if not aflag:
        ### 采集信息中monitor_type即为event_type
        if 'event_group' in data:
            #表示需要阈值判断
            threshcol = Digi_Collection('thresholdcol')
            ret = threshcol.connect()
            if ret == -1:      # connect failed
                exit()
            #connect success
            threshold = probe_threshold_key(threshcol,data['event_group'])

        #不需要阈值判断
        alarm_data = filter_and_probe_alarm_data(data,threshold)
        if alarm_data:
            if 'id' not in alarm_data:
                alarm_data['id'] = get_alarm_collection_id()

            alarm_handle_process(alarm_data)    # handle or unhandle
            alarm_event_coalesced(alarm_data)   # event coalesced

            almcol = Digi_Collection('alarmcol')
            ret = almcol.connect()
            if ret == -1:
                exit()
            almcol.write(alarm_data)
            
        mncol = Digi_Collection('monitorcol')
        ret = mncol.connect()
        if ret == -1:
            exit()
        mncol.write(data)
    else:                             # data为报警信息,直接写报警集合
        data['id'] = get_alarm_collection_id()

        alarm_handle_process(data)    # handle or unhandle
        alarm_event_coalesced(data)   # event coalesced
 
        almcol = Digi_Collection('alarmcol')
        ret = almcol.connect()
        if ret == -1:
            exit()
        almcol.write(data)

def get_alarm_collection_id():
    global ACOLID
    if not ACOLID:
        # 从数据库查询最新ID
        almcol = Digi_Collection('alarmcol')
        ret = almcol.connect()
        if ret == -1:
            exit()
        almcol.sort([('id',pymongo.DESCENDING)])
        almcol.limit(1)
        result = almcol.read()
        if result:
            ACOLID = result[0]['id'] + 1
        else:
            ACOLID = 1                # 集合中第一个数据的ID 
    else:
        ACOLID = ACOLID + 1
    return ACOLID

def probe_threshold_key(ocol,flag):
    # ocol: object;
    #       集合类对象
    # flag: string;
    #       监控事件类型标识
    threshold = {}
    if flag == 'CPUSAGE':
        result = ocol.read({'$or':[{'type':600010001},{'type':700010001}]})
    elif flag == 'CPUTEMP':
        result = ocol.read({'$or':[{'type':600010002},{'type':700010002}]})
    elif flag == 'MEM':
        result = ocol.read({'$or':[{'type':600020001},{'type':700020001}]})
    elif flag == 'FAN':
        result = ocol.read({'$or':[{'type':600030001},{'type':700030001}]})
    elif flag == 'UPS':
        result = ocol.read({'$or':[{'type':600050001},{'type':700050001}]})
    elif flag == 'BAT':
        result = ocol.read({'$or':[{'type':600060001},{'type':700060001}]})
    elif flag == 'SDISK':
        result = ocol.read({'$or':[{'type':600070004},{'type':700070004}]})
    elif flag == 'DISK':
        result = ocol.read({'$or':[{'type':600070006},{'type':700070005}]})
    elif flag == 'SDINODE':
        result = ocol.read({'$or':[{'type':600070007},{'type':700070006}]})
    elif flag == 'DINODE':
        result = ocol.read({'$or':[{'type':600070008},{'type':700070007}]})
    elif flag == 'SERVICE':

        result = ocol.read({'$or':[{'type':600160001},{'type':700160001}]})

    for ret in result:
        threshold[ret['type']] = ret['threshold']
    # threshold = {600020001:90,700020001:75}
    ###  阈值是否需要更新

    return threshold

def alarm_handle_process(data):
    # data: dict;
    #       报警数据信息
    atype = data['type'] // 100000000
    # atype = 6       表示异常事件
    # atype = 7       表示异常恢复或普通事件
    if atype == 7:
        almcol = Digi_Collection('alarmcol')
        ret = almcol.connect()
        if ret == -1:
            exit()
        htype = int(str(data['type']).replace('7','6',1))
        matches = {'type':htype,'node':data['node'],'servicename':data['servicename'],'device':data['device'],'handled':False}
        up_data = {'$set':{'handled':True}}
        almcol.update(matches,up_data,True)

def alarm_event_coalesced(data):

    # data: dict;
    #       报警数据信息
    atype = data['type']
    pass

####################################################
####             测试
####################################################
#def filter_and_probe_alarm_data(data,threshold={}):
#    #test_data = {'fan_rev':2400,'fan_status':'ok','low_rev':800,'upper_rev':25300,'device_name':'cpu1_fan'}
#    #test_data = {'low_temp':0.0,'cpu_status':'ok','cpu_temp':32.0,'device_name':'CPU','upper_temp':95}
#    ret = {}
#    ret['date'] = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
#    ret['type'] = 600010001
#    ret['sevirity'] = 'WARNING'
#    ret['servicename'] = '--'
#    ret['node'] = 'node-1.cluster-1'
#    ret['message'] = 'error message'
#    ret['device'] = data['device_name']
#    return ret

class Digi_Collection():
    # monitorcol: 监控集合
    # alarmcol：报警集合
    # thresholdcol：阈值集合

    def __init__(self,name):
        self.name = name
        self.wclient = None        # write
        self.rclient = None        # read
        self.db_sharding = False
        self.db_health = False

        self.sort_conds = []
        self.limit_conds = 0

        self.probe_database_process_health()

    def probe_database_process_health(self):
        mongos_cmd = "ps aux | grep mongos | grep 20000 | grep -v 'grep'"            #是否有mongos进程
        config_cmd = "ps aux | grep mongod | grep 21000 | grep -v 'grep'"            #是否有mongod配置服务器进程
        repl_cmd = "ps aux | grep mongod | grep 27017 | grep -v 'grep'"              #是否有mongod副本集进程
        mongos_ret = os.system(mongos_cmd)
        config_ret = os.system(config_cmd)
        repl_ret = os.system(repl_cmd)
        if mongos_ret == 0 and config_ret == 0 and repl_ret == 0:
            self.db_health = True
            self.db_sharding = True
        elif mongos_ret == 256 and config_ret == 256 and repl_ret == 0:
            self.db_health = True
            self.db_sharding = False
        else:
            self.db_health = False
            self.db_sharding = False

    def connect(self):
        # init write client and read client
        if self.db_health and self.db_sharding:          # sharding and replica set
            mpath = 'mongodb://127.0.0.1:20000'
            client = pymongo.MongoClient(mpath)
            self.rclient = self.wclient = client
        elif self.db_health:                             # only replica set
            mpath = 'mongodb://127.0.0.1:27017'
            self.rclient = pymongo.MongoClient(mpath)    # read from local        

            primary = self.rclient.clusterdb.command('isMaster')['primary']
            mpath = 'mongodb://' + primary                
            self.wclient = pymongo.MongoClient(mpath)    # write to primary
        else:
            digi_debug("MongoDB connect failed.")
            return -1

        return 0

    #def close(self):
        #self.client.close()

    def write(self,data):
        # data: dict;
        #       写入该集合的数据
        # 如果副本集模式，写数据需要连接到主节点
        try:
            col = self.wclient.clusterdb.get_collection(self.name)
            col.insert_one(data)
        except Exception,e:
            if self.name == 'alarmcol':
                ACOLID = ACOLID - 1
            digi_debug('clusterdb %s insert % failed' % (self.name,str(data)))

    def read(self,matches={}):
        # matches: dict;
        #          查询条件
        # 返回值为list
        ### matches 需要完善，字段筛选查询的情况
        ### col.find_one({'author':'Mike'},{'author':1})
        read_list = []
        try:
            col = self.rclient.clusterdb.get_collection(self.name)
            result = col.find(matches)
            if self.sort_conds:
                result = result.sort(self.sort_conds)
            if self.limit_conds:
                result = result.limit(self.limit_conds)
            for line in result:
                read_list.append(line)
        except Exception,e:
            digi_debug('clusterdb %s find/sort/limit failed' % self.name)
        return read_list

    def update(self,matches,data,many=False):
        # matches: dict;
        #          更新条件
        # data: dict;
        #       需要更新的字段及内容
        #       ex: data = {'$set':{'handled':True}
        #           表示更新指定字段
        #           data = {'date':'2016-03-09 17:00:00','type':'600010001','severity':'WARNING','servicename':'test','message':'High CPU usage','node':'node-1.cluster-1'}
        #           表示更新所有字段
        # 如果副本集模式，写数据需要连接到主节点
        try:
            col = self.wclient.clusterdb.get_collection(self.name)
            if many:
                ret = col.update_many(matches,data)
            else:
                ret = col.update(matches,data)
        except Exception,e:
            digi_debug('clusterdb %s update %s failed' % (self.name,str(data)))

    def sort(self,matches):
        # matches: list;
        #          排序键和排序方式组成的序列，可以为一个或多个元素
        #          ex：[('id',pymongo.DESCENDING)] 或 [("id",pymongo.DESCENDING),("node",pymongo.ASCENDING)]
        self.sort_conds = matches

    def limit(self,lmt):
        # lmt: int;
        #      筛选要获取的数量
        self.limit_conds = lmt

if __name__ == '__main__':
    #python mongo_utils.py write alarmcol type#600010002 node#node-1.cluster-1 message#'Disk Error' severity#ERROR True
    #python mongo_utils.py  read alarmcol type#600010002 node#node-1.cluster-1
    ### 磁盘挂载点到盘位信息的转换
    if len(sys.argv) >= 3:
        if sys.argv[1] == 'write':
            data = {}
            aflag = sys.argv[-1]
            colname = sys.argv[2]
            for line in sys.argv[3:-1]:
                if line.find('#') >= 0:
                    data[line.split('#')[0]] = line.split('#')[1]
                else:
                    #digi_debug('argv errors')
                    print 'argv errors'
                    exit()
            if data:
                ### 如果只写监控集合，aflag为False，给data添加monitor_type，write_event_to_db，然后在filter中处理该type，filter返回{}
                ### 如果只写报警集合，aflag为True，write_event_to_db
                if colname == 'alarmcol':
                    write_event_to_db(data,aflag)
                else:
                    data['monitor_type'] = '--'
                    write_event_to_db(data,aflag)
            else:
                print 'argv errors'
                exit()
        else:
            matches = {}
            for line in sys.argv[2:]:
                if line.find('#') >= 0:
                    matches[line.split('#')[0]] = line.split('#')[1]
                else:
                    print 'argv errors'
                    exit()
            almcol = Digi_Collection('alarmcol')
            ret = almcol.connect()
            if ret == -1:
                exit()
            result = almcol.read(matches)
            if result:
                print '1'      # exists
            else:
                print '0'      # not exists
    else:
        print 'argv errors'
        exit()
