#!/usr/bin/python

a = 20
b = 20

if (a is b):
    print '1 - a is b'

if (id(a) == id(b)):
    print '2 - a is b'
else:
    print '2 - a is not b'

print 'id(a) = ', id(a)
print 'id(b) = ', id(b)
