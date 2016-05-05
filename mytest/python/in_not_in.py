#!/usr/bin/python

a = 10
b = 20
list = [1, 2, 3, 4, 5]

if (a in list):
    print '1 - a in ', list
else:
    print '1 - a not in ', list

a = 2
if (a not in list):
    print '2 - a not in ', list
else:
    print '2 - a in ', list

