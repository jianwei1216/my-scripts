#!/usr/bin/python
# -*- coding:UTF-8 -*-

# python function test

# 1
def printme ( str ):
    '打印传入的字符串到标准显示设备上'
    print str
    return

printme("hello")
#printme() #--> error, not income arg
print 1

# 2
def changeme (mylist):
    print "mylist1: ", mylist
    mylist.append ([1, 2, 3, 4])
    return

mylist = [10, 20, 30]
changeme (mylist)
print "mylist2: ", mylist

# 3
printme (str="MY GOD")

# 4
def printinfo (name, age):
    print 'Name is', name
    print 'Age is', age
    return

printinfo (age=18, name='Jack')


# 5
def printinfo (name, age=35):
    print 'Name is', name
    print 'Age is', age
    return

printinfo (name='Jack')

# 6
def printargs (arg1, *vartuple):
    print arg1
    for i in vartuple:
        print i
    return

printargs (10)
printargs (10, 20, 30)

# 7
sum = lambda arg1, arg2: arg1 + arg2

print sum(10, 20)

# 8
def sum (arg1, arg2):
    total = arg1 + arg2
    return total

print sum(100,200)

# 9
total = 0
def sum (arg1, arg2):
    total = arg1 + arg2
    print "n: ", total
    return total

print sum(100,200)
print "w: ", total
































