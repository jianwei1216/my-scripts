#!/usr/bin/python
# -*- coding: UTF-8 -*-

# 1
def print_test():
    print "Python 是一个非常棒的语言,不是吗？"

#print_test()

# 2
def raw_input_test():
    str = raw_input ("请输入:")
    print "你输入的内容是:", str

#raw_input_test()

# 3
def input_test():
    str = input ("请输入：")
    print "你输入的内容是：", str

#input_test()

# 4
def open_close_test():
    fo = open ("list.py", "rb")
    print fo.name
    print fo.closed
    print fo.mode
    print fo.softspace
    print fo.close

#open_close_test()

# 5
def write_test():
    fo = open("foo.txt", "wb")
    print fo.write ("Hello Python!\n")
    print fo.close

#write_test()

# 6
def read_test():
    fo = open ("foo.txt", "rb")
    str = fo.read()
    print str
    fo.close()

#read_test()

# 7
def tell_seek_test():
    fo = open ("foo.txt", "r+")
    str = fo.read()
    print str

    position = fo.tell()
    print 'current position: ', position

    position = fo.seek(0,0)
    str =fo.read(10)
    print str

    fo.close()

#tell_seek_test()

# 8
import os

def rename_test():
    os.rename ("foo.txt", "foo1.txt")    

#rename_test()

# 9
def remove_test():
    os.remove("foo1.txt")

#remove_test()

# 10
def mkdir_test():
    os.mkdir("newdir")

#mkdir_test()

# 11
def chdir_test():
    print os.getcwd()
    os.chdir("newdir")
    print os.getcwd()

#chdir_test()

# 12
def rmdir_test():
    os.rmdir ("newdir")

rmdir_test()

