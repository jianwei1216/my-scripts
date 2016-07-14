#!/usr/bin/env python
#-*- coding: utf-8 -*-
import os
import sys
import re
import copy
import subprocess
import traceback
import ConfigParser
from decimal import *

digimanager = '/usr/local/digioceanfs_manager/manager/digi_manager.pyc'
extendconfigfile = '/root/serviceextend.conf'
maxn = 16 #n最大
maxnm = 2 #最多允许几组组合
#------------------------
# deal with unit
#------------------------ 
def getUnit(unit, value, isB = 'false'):
    if isB == 'true':
        if unit == 'KB':
           value = round(float(value) / 1024, 2)
        elif unit == 'MB':
           value = round(float(value) / 1024/ 1024, 2)
        elif unit == 'GB':
           value = round(float(value) / 1024/ 1024/ 1024, 2)
        elif unit == 'TB':
           value = round(float(value) / 1024/ 1024/ 1024/ 1024, 2)
        return value
    else:
        if unit == 'KB':
           value = round(float(value) / 1, 2)
        elif unit == 'MB':
           value = round(float(value) / 1024, 2)
        elif unit == 'GB':
           value = round(float(value) / 1024/ 1024, 2)
        elif unit == 'TB':
           value = round(float(value) / 1024/ 1024/ 1024, 2)
        return value
#------------------------
# sort string with int
#------------------------ 
def embedded_numbers(s):
    re_digits = re.compile(r'(\d+)')
    pieces = re_digits.split(s)
    pieces[1::2] = map(int, pieces[1::2])
    return pieces
#########################################################################################################
#
#   Function: func_node_disk_info
#   Variable: node(if this is default, list all the disk in cluster),
#             unit(this variable is to decide unit of return value),
#             clusternoderaidinfo
#   Purpose: to list disks info in a node or a cluster
#   Data structure: type: list,
#                   ex: [{'status':status, 'devname':devname, 'type':type, 'vandor':vandor, 'sn_number':sn_number, 'size':size, 'port':port,'servicename':raidname, 'istart':istart, 'pid':pid},
#                        {...},
#                        ...]
#
#########################################################################################################
def func_node_disk_info(node='',unit=''):
    clusternodediskinfo = {}
    key_nodisk = ['position','nodisk', 'devname']
    key_raid = ['position','status','devname','type','vandor','sn_number','size','type_in_raid','raidname','istart','pid']
    key_service = ['position','status','devname','type','vandor','sn_number','size','port','servicename','istart','pid']
    try:
        if node != '':
            clusternodediskinfo[node] = []
            cmd = 'python %s node list_disk %s' % (digimanager, node)
            proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
            result = proc.stdout.readlines()
            proc.wait()
            for res in result[1:-1]:
                if len(res.split()) <= 3:
                    diskinfo = res.split()
                    disk = dict()
                    disk['position'] = '-'
                    disk['status'] = diskinfo[1].strip()
                    disk['devname'] = diskinfo[2].strip()
                    disk['type'] = '-'
                    disk['vandor'] = '-'
                    disk['sn_number'] = '-'
                    disk['size'] = '-'
                    disk['port'] = '-'
                    disk['nodename'] = '-'
                    disk['size'] = '-'
                    clusternodediskinfo.append(disk)
                    continue
                diskinfo = res.split('\t')
                if diskinfo[1] == "in_raid":
                    d = dict(zip(key_raid,diskinfo))
                else:
                    d = dict(zip(key_service,diskinfo))
                d['nodename'] = node
                d['size'] = getUnit(unit, d['size'])
                clusternodediskinfo[node].append(d)
        else:
            cmd = 'python %s node list_disk %s' % (digimanager, node)
            proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
            result = proc.stdout.readlines()
            proc.wait()
            disklist = []
            flag = -1
            retcode = result[0]
            if retcode == '0\n':
                for res in result[1:-1]:
                    disk = dict()
                    diskinfo = res.split('\t')
                    if len(diskinfo) < 2:
                        disklist.append(diskinfo[0].strip())
                        flag += 1
                    elif len(diskinfo) == 3:
                        disk['position'] = diskinfo[0].strip()
                        disk['status'] = diskinfo[1].strip()
                        disk['devname'] = diskinfo[2].strip()
                        disk['type'] = '-'
                        disk['vandor'] = '-'
                        disk['sn_number'] = '-'
                        disk['size'] = '-'
                        disk['port'] = '-'
                        disk['nodename'] = '-'
                        disk['size'] = '-'
                    elif len(diskinfo) <= 8:
                        disk['position'] = diskinfo[0].strip()
                        disk['status'] = diskinfo[1].strip()
                        disk['devname'] = diskinfo[2].strip()
                        disk['type'] = diskinfo[3].strip()
                        disk['vandor'] = diskinfo[4].strip()
                        disk['sn_number'] = diskinfo[5].strip()
                        disk['size'] = diskinfo[6].strip()
                        disk['port'] = diskinfo[7].strip()
                        disk['nodename'] = disklist[flag]
                        disk['size'] = getUnit(unit, disk['size'])
                    else:
                        if diskinfo[1].strip()=='no_disk':
                            disk['position'] = '-'
                            disk['status'] = diskinfo[1].strip()
                            disk['devname'] = diskinfo[2].strip()
                            disk['type'] = '-'
                            disk['vandor'] = '-'
                            disk['sn_number'] = '-'
                            disk['size'] = '-'
                            disk['port'] = '-'
                            disk['nodename'] = '-'
                            disk['size'] = '-'
                        else:
                            disk['position'] = diskinfo[0].strip()
                            disk['status'] = diskinfo[1].strip()
                            disk['devname'] = diskinfo[2].strip()
                            disk['type'] = diskinfo[3].strip()
                            disk['vandor'] = diskinfo[4].strip()
                            disk['sn_number'] = diskinfo[5].strip()
                            disk['size'] = diskinfo[6].strip()
                            disk['port'] = diskinfo[7].strip()
                            if disk['status'] == 'in_raid':
                                disk['raidname'] = diskinfo[8].strip()
                            else:
                                disk['servicename'] = diskinfo[8].strip()
                            disk['istart'] = diskinfo[9].strip()
                            disk['nodename'] = disklist[flag]
                            disk['size'] = getUnit(unit, disk['size'])
                    if disk and disk['devname'] and disk['nodename']:
                        if disk['nodename'] not in clusternodediskinfo:
                            clusternodediskinfo[disk['nodename']] = []
                        clusternodediskinfo[disk['nodename']].append(disk)
    except Exception,e:
        print >> sys.stderr, traceback.print_exc(e)
    return clusternodediskinfo

def func_client_node_list_all(service_name):
    clusterclientnodes=[]
    try:
        cmd = 'python %s service client_list %s' % (digimanager, service_name)
        proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
        result = proc.stdout.readlines()
        proc.wait()
        if len(result)>1:
            for i in result:
                r=i.replace('\n','')
                if r!='0' and r!='':
                    s=r.split('\t')
                    node=dict()
                    node['service_name']=service_name
                    node['node_name']=s[0]
                    if s[1]=='started':
				        node['status']='1'
                    else:
                        node['status']='0'
                    clusterclientnodes.append(node)
    except Exception,e:
        print >> sys.stderr, traceback.print_exc(e)
    return clusterclientnodes
#########################################################################################################
#
#   Function: func_service_list_all
#   Variable: unit(not-essential)
#   Purpose: to list all services info
#   Data structure: type: list
#                   ex: [{'servicename': servicename, 'totalsize': toatlsize, 'usesize': usesize, 'raidlv': raidlv, 'status': status, 'usage': usage, 'abmormal_status': abnormal_status}#                       ,{...},...]
#
#########################################################################################################    
def func_service_list_all(unit=''):
    clusterservices = {}
    try:
        p_status = 'name: (\S+)\s+service total size: (\S+)\s+service used size: (\S*)\s+service raid level: (\S+)\s+service status: (\S+)\s+'
        cmd = "python %s service list" % (digimanager)
        proc_status = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
        result_status = proc_status.stdout.readlines()
        proc_status.wait()
        if len(result_status)>1:
            for i in range(len(result_status)):
                arr=result_status[i].replace('\n','')
                index=arr.split(': ')
                if index[0]=='Volume Name':
                    x=0
                    end=''
                    while end!='Bricks':
                        end=result_status[i+x].replace('\n','')
                        end=end.split(':')
                        end=end[0]
                        x+=1
                    service=dict()						
                    service['servicename']=index[1]
                    for j in range(x-1):
                        r=result_status[i+j].replace('\n','')
                        s=r.split(': ')				
                        if s[0]=='Type':		
                            service['raidlv']=s[1]
                            continue
                        elif s[0]=='Replica Number':	
                            service['afr']=s[1]
                            continue
                        elif s[0]=='Stripe Number':	
                            service['strip']=s[1]
                            continue
                        elif s[0]=='Volume ID':		
                            service['vid']=s[1]
                            continue
                        elif s[0]=='Status':		
                            service['status']=s[1]
                            continue
                        elif s[0]=='Number of Bricks':		
                            service['disknum']=s[1]
                            continue
                        elif s[0]=='Transport-type':		
                            service['type']=s[1]
                            continue
                        elif s[0]=='Free Size':		
                            service['freesize']=s[1]
                            continue
                        elif s[0]=='Total Size':		
                            service['totalsize']=s[1]
                            continue
                        elif s[0]=='Brick Status':		
                            service['brickstatus']=s[1]
                            continue
                        else:
                            pass
                    try:
                        service['afr']
                    except:    
                        service['afr']='NaN'
                    try:
                        service['strip']
                    except:    
                        service['strip']='NaN'
                    if 	service['freesize']=='N/A':
                        service['usage']=0
                        service['freesize']=0.0
                        service['totalsize']=0.0
                        service['usedsize']=0.0
                    else:
                        if float(service['totalsize'])==0:
                            service['usedsize']=0						
                            service['usage']=0
                        else:	
                            service['usedsize']=float(service['totalsize'])-float(service['freesize'])
                            service['usage']=round(Decimal(str(service['usedsize']))/Decimal(str(service['totalsize'])), 2)	
                        service['freesize']=getUnit(unit, service['freesize'])
                        service['totalsize']=getUnit(unit, service['totalsize'])
                        service['usedsize']=getUnit(unit, service['usedsize'])
                    service['client']=len(func_client_node_list_all(service['servicename']))
                    service['afr_info']='0'
                    if service['raidlv'].find('Disperse') >= 0:
                        re_result = re.match('(.*)\((.*)\)(.*)',service['disknum'])
                        if re_result:
                            disperse_detail = re_result.groups()[1]
                            service['afr'] = disperse_detail.split(" + ")[1]
                            service['strip'] = str(int(disperse_detail.split(" + ")[0]))
                    clusterservices[service['servicename']] = service
    except Exception,e:
        print >> sys.stderr,traceback.print_exc()
    return clusterservices
#----------------------------------------------------
# ecnodeinfos {3:[['node-1','node-2','node-3']],4:[['node-4','node-5','node-6','node-7']]} #磁盘数：每组对应的节点列表
# eccountinfos {3: 1, 4: 1} #磁盘数：一共几组
# usednodes set(['node-1','node-2']) #已使用的节点
'''
     n m num
    [[2,1,1]] #nmlist
     /_/   \  
    {3: [['0','1','2']]} #ecgroupinfos
       ___/    \_____
      /              \
    {'0': [0, 1, 2], '1': [3, 4, 5], '2': [6, 7, 8]} #ecbrickinfos
           |   \  \        \   \  \__________________________________________________________________________________________
           |    \  \        \   \____________________________________________________________________                        \
           |     \  \        \_______________________________________________                        \                        \
           |      \  \_______________________________                        \                        \                        \
       ____|       \_________                        \                        \                        \                        \
      /                      \                        \                        \                        \                        \
    {0: ('node-1', '/disk1'), 1: ('node-2', '/disk1'), 2: ('node-3', '/disk1'), 3: ('node-1', '/disk2'), 4: ('node-2', '/disk2'), 5: ('node-3', '/disk2'), 6: ('node-1', '/disk3'), 7: ('node-2', '/disk3'), 8: ('node-3', '/disk3')} #brickinfos
'''
#----------------------------------------------------  
def func_service_brick_info(clusterservicename):
    m = 1
    nmlist = []
    eccountinfos = {}
    brickinfos = {}
    ecbrickinfos = {}
    ecnodeinfos = {}
    usednodes = set()
    
    cmd = "python %s service list" % (digimanager)
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    result = proc.stdout.readlines()
    proc.wait()
    
    for line in result:
        rdm = re.match('^Number\s+of\s+Bricks\s*:.*\(\s*\d+\s*\+\s*(\d+)\s*\).*$',line)
        ecm = re.match('^ec-arrays\s*:\s*(.*)$',line)
        ndm = re.match('^Brick(\d+)\s*:\s*(.*):(.*)$',line)
        if rdm: #获取m
            m = int(rdm.group(1).strip())
        elif ecm: #获取array
            ecstr = ecm.group(1).strip()
            teclist = re.split('<([^<]*)>',ecstr)
            for tecstr in teclist: #'0,0,3','1,3,3','2,6,3','3,9,3' (arrayid,startid,counts)
                if tecstr:
                    tec = tecstr.split(',')
                    brickids = range(int(tec[1]),int(tec[1])+int(tec[2]))
                    ecbrickinfos[tec[0]] = brickids
        elif ndm: #获取brick
            brickid = int(ndm.group(1)) - 1
            node = ndm.group(2).strip()
            mountpoint = ndm.group(3).strip().replace('-','/')
            brickinfos[brickid] = (node,mountpoint)
            usednodes.add(node)
    
    for arrayid,brickids in ecbrickinfos.iteritems():
        counts = len(brickids)
        if counts not in ecnodeinfos:
            ecnodeinfos[counts] = []
            eccountinfos[counts] = 0
        nodelist = []
        for brickid in brickids:
            if brickid in brickinfos:
                brickinfo = brickinfos[brickid] #(node,mountpoint)
                nodelist.append(brickinfo[0])
        nodelist.sort()
        if nodelist:
            if nodelist not in ecnodeinfos[counts]:
                ecnodeinfos[counts].append(nodelist)
            eccountinfos[counts] += 1
    for nm,counts in eccountinfos.iteritems():
        n = nm - m
        nmlist.append([n,m,counts])

    return m, nmlist, ecbrickinfos, brickinfos, usednodes, ecnodeinfos
    
def combnm(a,b,nn,c='',flag=1):
    global schemelist
    global minnm
    if len(a) > 0:
        rangelist = range(b[0],-1,-1)
        if flag == 2:
            rangelist = range(b[0]+1)
        for i in rangelist:
            c1 = '%s*%s' % (a[0],i)
            if c:
                c1 = '%s+%s*%s' % (c,a[0],i)
            #if c1 and eval(c1) > 0 and eval(c1) == nn: #全部磁盘都被处理掉
            if c1 and eval(c1) > 0 and nn - eval(c1) < minnm: #剩余磁盘不足以构建已有的组合
                schemelist.append((0,c1))
                return
            combnm(a[1:],b[1:],nn,c1,flag)
            
'''
a agaplist 任意两个组合直接n的差值
b aglist 每个组合有几组
d tnmlist 组合列表
e gaplist 差值对应的两个组合，索引对应tnmlist
'''
def combnmgap(a,b,d,e,nn,c=''):
    global schemelist
    nmlen = len(d)
    if len(a) > 0:
        for i in range(b[0],-1,-1):
            if c and eval(c) == nn:
                schemelist.append((1,c))
                return
            c1 = '%s*%s' % (a[0],i)
            if c:
                c1 = '%s+%s*%s' % (c,a[0],i)
            #递归，减组记录（组内扩容，比如3+1变为了4+1，则原来的组合的组数减一），加组不记录
            tlist = c1.split('+')
            tindex = len(tlist)
            aindex = e[tindex-1][0]
            bindex = e[tindex-1][1]
            b1 = copy.deepcopy(b)
            if i > 0:
                gindex = 0
                for o in range(nmlen):
                    for p in range(o+1,nmlen):
                        if gindex >= tindex:
                            gap = d[o][0]-d[p][0]
                            if gap > 0:
                                if p == aindex:
                                    b1[gindex-tindex+1] = b1[gindex-tindex+1]-i
                            else:
                                if o == aindex:
                                    b1[gindex-tindex+1] = b1[gindex-tindex+1]-i
                        gindex += 1
            combnmgap(a[1:],b1[1:],d,e,nn,c1)
            
'''
a tlist 每个组合的总节点数列表
b tglist 新增的节点数针对每个组合最多能创建几组
d tnmlist 现有的组合列表
'''
def combnmnmexit(nn,schemestr):
    if schemestr:
        total = 0
        dm = re.search('\+?(\d+)\*(\d+)\*(\d+)\*(\d+)',schemestr)
        if dm:
            tschemestr = re.sub('\+?(\d+)\*(\d+)\*(\d+)\*(\d+)','',schemestr)
            if tschemestr:
                total += eval(tschemestr) 
            tj = int(dm.group(3))
            tg = int(dm.group(4))
            total += tj * tg
            if total == nn:
                return True
    return False

def combnmnm(a,b,d,nn,c='',flag=1):
    global schemelist
    global itemchange
    if len(a) > 0:
        rangelist = range(b[0]+1)
        if flag == 2:
            rangelist = range(b[0],-1,-1)
        for i in rangelist:
            c1 = ''
            c1 = '%s*%s' % ((a[0][0]+a[0][1]),i)
            if c:
                c1 = '%s+%s*%s' % (c,(a[0][0]+a[0][1]),i)
            tnn = nn - (c1 and eval(c1) or 0)
            tindex = d.index(a[0])
            if i > 0:
                itemchange.add(tindex)
            ilen = len(d) #现有组合的数量
            if tnn > 0 and len(itemchange) < len(d):
                for item in d:
                    if d.index(item) not in itemchange:
                        for j in range(item[2],0,-1):
                            cg = tnn%j #剩余新增节点是否能整除某个组的组合，使其整体扩容至一个新的组合
                            tg = tnn/j #组合要新增几个节点
                            if cg == 0:
                                if tg+item[0] <= maxn:
                                    if j == item[2]:
                                        c1 = '%s+%s*%s*%s*%s' % (c1,item[0],item[1],j,tg)
                                        if combnmnmexit(nn,c1):
                                            schemelist.append((5,c1))
                                            return
                                    else:
                                        if ilen < maxnm:
                                            c1 = '%s+%s*%s*%s*%s' % (c1,item[0],item[1],j,tg)
                                            if combnmnmexit(nn,c1):
                                                schemelist.append((5,c1))
                                                return
            combnmnm(a[1:],b[1:],d,nn,c1,flag)
    
if __name__ == "__main__":
    global schemelist
    global itemchange
    global minnm
    servicename = None
    m = 1
    cecbrickinfo = {}
    args = sys.argv[1:]
    if len(args) > 0:
        servicename = args[0]
    if servicename:
        m, nmlist, ecbrickinfos, brickinfos, usednodes, ecnodeinfos = func_service_brick_info(servicename)
        clusterservices = func_service_list_all()
        if servicename in clusterservices and ecbrickinfos:#服务存在并且服务是纠删码模式
            nodedisks = func_node_disk_info()
            nodediskinfos = {}
            usednodediskinfos = {} #已创建服务的节点磁盘信息
            nodediskcountinfos = {} #未创建服务的节点上的磁盘数量统计
            for nodename,diskinfos in nodedisks.iteritems():
                for diskinfo in diskinfos:
                    if diskinfo['status'] == 'unused':
                        if diskinfo['nodename'] in usednodes:
                            if diskinfo['nodename'] not in usednodediskinfos:
                                usednodediskinfos[diskinfo['nodename']] = []
                            usednodediskinfos[diskinfo['nodename']].append(diskinfo)
                        else:
                            if diskinfo['nodename'] not in nodediskinfos:
                                nodediskinfos[diskinfo['nodename']] = []
                                nodediskcountinfos[diskinfo['nodename']] = 0
                            nodediskinfos[diskinfo['nodename']].append(diskinfo)
                            nodediskcountinfos[diskinfo['nodename']] += 1
            #处理已创建服务节点上的磁盘
            minnm = min([nm[0]+nm[1] for nm in nmlist] or [0])
            while usednodediskinfos:
                if len(usednodediskinfos.keys()) < minnm:
                    break
                for eccount,ecnodeinfo in ecnodeinfos.iteritems():
                    for nodelist in ecnodeinfo:
                        if len(usednodediskinfos.keys()) < len(nodelist):
                            continue
                        for node in nodelist:
                            if node not in usednodediskinfos:
                                break
                        else:
                            newec = []
                            for node in nodelist:
                                disk = usednodediskinfos[node].pop(0)
                                if not usednodediskinfos[node]:
                                    del usednodediskinfos[node]
                                newec.append((disk['nodename'],disk['devname']))
                            if eccount not in cecbrickinfo:
                                cecbrickinfo[eccount] = []
                            cecbrickinfo[eccount].append(newec)
            #处理未创建服务节点
            nn = len(nodediskinfos.keys()) #新增节点数
            disknum = min(nodediskcountinfos.values() or [0]) #节点磁盘数，按最少节点磁盘数计算
            
            tnmlist = nmlist[::]
            tlist = [nm[0]+nm[1] for nm in tnmlist] #每个组合的总节点数
            tglist = [nn*disknum/at for at in tlist] #每个n+m组合最多能创建几组
            maxnn = max(tlist) - m
            minnm = min(tlist)
            #组差距
            gaplist = []
            aglist = [] #每个n+m组合有几组
            nmlen = len(tnmlist)
            for i in range(nmlen):
                for j in range(i+1,nmlen):
                    gap = tnmlist[i][0]-tnmlist[j][0]
                    if gap > 0:
                        gaplist.append([j,i,gap])
                        aglist.append(tnmlist[j][2])
                    else:
                        gaplist.append((i,j,abs(gap)))
                        aglist.append(tnmlist[i][2])
            agaplist = [gap[2] for gap in gaplist]
            
            schemelist = []
            itemchange = []
            if nn >= min(tlist):
                #任意n+m组合
                combnm(tlist,tglist,nn,'')
            if len(nmlist) > 1:
                #任意差距组合
                combnmgap(agaplist,aglist,tnmlist,gaplist,nn,c='')
            #先创建现有组合，将某些组向maxn移动,可能产生新组
            combnmnm(tnmlist,tglist,tnmlist,nn,'')
            for schemestr in schemelist:
                if schemestr[0] == 0:
                    scheme = []
                    if schemestr[1]:
                        tschemelist = schemestr[1].split('+')
                        i = 0
                        for gscheme in tschemelist:
                            tscheme = gscheme.split('*')
                            if int(tscheme[1]) > 0:
                                nm = tnmlist[i]
                                scheme.append([nm[0],nm[1],int(tscheme[1])])
                            i += 1
                    for item in scheme:
                        tt = item[0]+item[1]
                        for loop in item[2]:
                            newec = []
                            nodes = nodediskinfos.keys()
                            nodes = sorted(nodes,key=embedded_numbers,reverse=True)
                            for i in range(tt-1,-1,-1):
                                disk = nodediskinfos[nodes[i]].pop(0)
                                if not nodediskinfos[nodes[i]]:
                                    del nodediskinfos[nodes[i]]
                                    nodes.remove(nodes[i])
                                newec.append((disk['nodename'],disk['devname']))
                            if eccount not in cecbrickinfo:
                                cecbrickinfo[eccount] = []
                            cecbrickinfo[eccount].append(newec)
                elif schemestr[0] == 1:
                    scheme = []
                    tschemelist = schemestr[1].split('+')
                    i = 0
                    for gscheme in tschemelist:
                        tscheme = gscheme.split('*')
                        if int(tscheme[1]) > 0:
                            gap = gaplist[i]
                            anm = tnmlist[gap[0]]
                            bnm = tnmlist[gap[1]]
                            scheme.append([anm[0],anm[1],bnm[0],bnm[1],int(tscheme[1])])
                        i += 1
                    for item in scheme:
                        ft = item[0] + item[1]
                        tt = item[2] + item[3]
                        gap = tt - ft
                        #每个array添加gap块磁盘，获取ecbrickinfos，逐个处理
                        i = 0
                        eckey = ','.join([str(it) for it in item])
                        ecnums = ecbrickinfos.keys()
                        ecnums.sort()
                        for ecnum in ecnums:
                            ecbrickids = ecbrickinfos[ecnum]
                            if i == item[4]:
                                break
                            if len(ecbrickids) == ft:
                                nodes = nodediskinfos.keys()
                                nodes = sorted(nodes,key=embedded_numbers,reverse=True)
                                newec = []
                                for k in range(gap-1,-1,-1):
                                    disk = nodediskinfos[nodes[k]].pop(0)
                                    if not nodediskinfos[nodes[k]]:
                                        del nodediskinfos[nodes[k]]
                                        nodes.remove(nodes[k])
                                    newec.append((disk['nodename'],disk['devname']))
                                if eckey not in cecbrickinfo:
                                    cecbrickinfo[eckey] = {}
                                if ecnum not in cecbrickinfo[eckey]:
                                    cecbrickinfo[eckey][ecnum] = []
                                cecbrickinfo[eckey][ecnum] = newec
                                i += 1
                elif schemestr[0] == 5:
                    scheme = []
                    if schemestr[1]:
                        tschemelist = schemestr[1].split('+')
                        i = 0
                        for gscheme in tschemelist[:-1]:
                            tscheme = gscheme.split('*')
                            if int(tscheme[1]) > 0:
                                nm = tnmlist[i]
                                scheme.append([nm[0],nm[1],int(tscheme[1])])
                            i += 1
                        tscheme = tschemelist[-1].split('*')
                        scheme.append([int(tscheme[0]),int(tscheme[1]),int(tscheme[0])+int(tscheme[3]),int(tscheme[1]),int(tscheme[2])])
                    for item in scheme[:-1]:
                        tt = item[0]+item[1]
                        for loop in range(item[2]):
                            newec = []
                            nodes = nodediskinfos.keys()
                            nodes = sorted(nodes,key=embedded_numbers,reverse=True)
                            for i in range(tt-1,-1,-1):
                                disk = nodediskinfos[nodes[i]].pop(0)
                                if not nodediskinfos[nodes[i]]:
                                    del nodediskinfos[nodes[i]]
                                    nodes.remove(nodes[i])
                                newec.append((disk['nodename'],disk['devname']))
                            if eccount not in cecbrickinfo:
                                cecbrickinfo[eccount] = []
                            cecbrickinfo[eccount].append(newec)
                    #移动
                    if scheme:
                        item = scheme[-1]
                        ft = item[0] + item[1]
                        tt = item[2] + item[3]
                        gap = tt - ft
                        #每个array添加gap块磁盘，获取ecbrickinfos，逐个处理
                        i = 0
                        eckey = ','.join([str(it) for it in item])
                        ecnums = ecbrickinfos.keys()
                        ecnums.sort()
                        for ecnum in ecnums:
                            ecbrickids = ecbrickinfos[ecnum]
                            if i == item[4]:
                                break
                            if len(ecbrickids) == ft:
                                nodes = nodediskinfos.keys()
                                nodes = sorted(nodes,key=embedded_numbers,reverse=True)
                                newec = []
                                for k in range(gap-1,-1,-1):
                                    disk = nodediskinfos[nodes[k]].pop(0)
                                    if not nodediskinfos[nodes[k]]:
                                        del nodediskinfos[nodes[k]]
                                        nodes.remove(nodes[k])
                                    newec.append((disk['nodename'],disk['devname']))
                                if eckey not in cecbrickinfo:
                                    cecbrickinfo[eckey] = {}
                                if ecnum not in cecbrickinfo[eckey]:
                                    cecbrickinfo[eckey][ecnum] = []
                                cecbrickinfo[eckey][ecnum] = newec
                                i += 1
    #生成配置文件
    if cecbrickinfo:
        config = ConfigParser.ConfigParser()
        for section,sctioninfo in cecbrickinfo.iteritems():
            if type(section) == int:
                count = len(sctioninfo)
                n = section - m
                section = '%d,%d,%d' % (n,m,count)
                config.add_section(section)
                config.set(section,'service',servicename)
                i = 0
                for diskinfo in sctioninfo:
                    optionvalue = '\n    '.join(['%s:%s' % (item[0],item[1]) for item in diskinfo])
                    config.set(section,str(i),optionvalue)
                    i += 1
            elif section.find(',') >= 0:
                config.add_section(section)
                config.set(section,'service',servicename)
                for ecnum,diskinfo in sctioninfo.iteritems():
                    optionvalue = '\n    '.join(['%s:%s' % (item[0],item[1]) for item in diskinfo])
                    config.set(section,str(ecnum),optionvalue)
        f = open(extendconfigfile,'w')
        config.write(f)
        f.close()

