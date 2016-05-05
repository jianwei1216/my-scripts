#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os 
import sys
import math
import random

var1 = 1
var2 = 10
del var1, var2

print '类型转换'
s = '123456'
print type(int(s)), int(s)
print type(complex(s)), complex(s)
print type(str(s)), str(s)
print type(float(s)), float(s)
print type(repr(s)), repr(s)
print type(eval(s)), eval(s)
print type(tuple(s)), tuple(s)
print type(list(s)), list(s)
print type(chr(90)), chr(90)
print type(unichr(90)), unichr(90)
print type(ord('A')), ord('A')
print type(hex(90)), hex(90)
print type(oct(90)), oct(90)

print '数学函数'
print abs(-10)
print cmp("hello", "world")
print max(1,2,3)
print min(1,2,3)
print math.ceil(4.1)
print math.exp(1)
print math.fabs(-1)
print math.floor(4.9)
print math.log(100, 10)
print math.log10(100)
print math.modf(-0)
print pow(2,3)
print round(3.123453, 5)
print math.sqrt(4)

print '随机数函数'
print random.choice(range(100))
print random.randrange(1,100,1)
print random.seed(100)
list1 = [1,3,4,5,6,2]
print list1
print random.shuffle(list1)
print list1
print random.uniform(1,2)

print '三角函数'
print math.acos(0.8)


print math.pi
print math.e













