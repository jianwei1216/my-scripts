#!/usr/bin/python
# -*- coding: UTF-8 -*-

# 位运算符
print " & | ^ ~ << >>"

# 逻辑运算符
a = 10
b = 20
if (a and b):
    print 'a and b = ', a and b

if (a or b):
    print 'a or b = ', a or b

a = 0
if (not a):
    print 'not a = ', not a


a = 21
b = 10
c = 0

# 赋值运算符
print "赋值运算符"
c = a + b
c += a
print "1 - c = ", c

c *=  a
print "2 - c = ", c

c /= a
print "3 - c = ", c

c = 2
c %= c
print "4 - c = ", c

c **= a
print "5 - c = ", c

c //= a
print "6 - c = ", c

# 比较运算符
print "比较运算符"
if (a == b):
    print "1 - a == b"
else:
    print "1 - a != b"

if (a != b):
    print "2 - a != b"
else:
    print "2 - a == b" 

if (a <> b):
    print "3 - a != b"
else:
    print "3 - a == b"

if (a < b):
    print "4 - a < b"
else:
    print "4 - a >= b"

if (a > b):
    print "5 - a > b"
else:
    print "5 - a <= b"

if (a >= b):
    print "6 - a >= b"
else:
    print "6 - a < b"

if (a <= b):
    print "7 - a <= b"
else:
    print "7 - a > b"

# + - * ** / // 运算符
print "+ - * / ** //"
c = a + b
print "1 - c = ", c # 31

c = a - b
print "2 - c = ", c # 11

c = a * b
print "3 - c = ", c # 210

c = a / b
print "4 - c = ", c # 2

a = 2
b = 3
c = a ** b
print "6 - c = ", c # 8

a = 10
b = 5
c = a // b
print "7 - c = ", c # 2
