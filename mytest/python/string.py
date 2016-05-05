#!/usr/bin/python
# -*- coding: UTF-8 -*-
import string

s = "a1a2an"
print s[0:]

s = "ilovepython"
print s[1:5]

str = 'Hello World!'
print str #Hello World!
print str[0] #H
print str[2:5] #llo
print str[2:] #llo World!
print str * 2 #Hello World!Hello World!
print str + "TEST" #Hello World!TESET

print 'str 2'
var1 = 'Hello World!'
var2 = 'Python Programming'
print 'var1[0] is ', var1[0]
print 'var2[1:5] is ', var2[1:5]

print 'str 3'
print 'Updating String :-', var1[:6] + 'Python'

print '转义字符'
print 'Hello \
        World'
print '\''
print '\"'
print '\a'
print '\b'
print '\e'
print '\000'
print '1\n2'
print '1\v2'
print '1\t2'
print '1\r2'
print '1\f2'
print '1\0122'
print '1\x0a2'
print '\other'


print '字符串运算符'
print "a" + " b"
print "a"*100
str1 = '12345'
print str1[0]
print str1[:]
print '1' in str1
print '0' in str1
print R'\n'

i = 10
print 'My name is %s and weight is %d kg.' % ('Zara', 21)

print '%c' % ('c')
print '%s' % ("hello")
print '%d' % (11)
print '%u' % (22)
print '%o' % (33)
print '%X' % (44)
print '%f' % (55)
print '%e' % (66)
print '%d' % (id(77))
print '%x' % (77)
print id(77)
#print '%c, %s, %d, %u %o %x %f %e %p' % ('c', "hello", 1, 2, 3, 4, 5, 6, i) 

#print "%*2d" % (88)

hi = '''hi
there'''
print hi


str2 = u'hello world2!'
print str2

print 'str3'
str3 = 'hello\tworld3'
print str3.capitalize()
print str3.center(100)
print str3.count(' ', 0, len(str3))
#print str3.decode(encoding='UTF-8', errors='strict')
print str3.decode('UTF-8', 'strict')
print str3.encode('UTF-8', 'strict')
print str3.endswith('3', 0, len(str3))
print str3.expandtabs(12)
print str3.find('world', 0, len(str3))
print str3.index('world', 0, len(str3))
print str3.isalnum()
print str3.isalpha()
#print str3.isdecimal()
print str3.isdigit()
print str3.islower()
#print str3.isnumeric()
print str3.istitle()
print str3.isupper()
print str3.join("hello world")
print str3.ljust(100)
print str3.rjust(100)
print str3.lower()
print str3.upper()
#str3 = "     hello                "
print str3.lstrip()
print str3.rstrip()
#print str3.maketrans('l', 'Z')
print max(str3)
str3 = "hello"
print min(str3)
print str3.partition('l')
print str3.replace('l', 'w')
print str3.rfind('l', 0, len(str3))
print str3.rindex('l', 0, len(str3))







