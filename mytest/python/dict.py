#!/usr/bin/python

dict = {}
dict['one'] = "This is one."
dict[2] = 'This is two.'

tinydict = { 'name':'john', 'code':786, 'dept':'sales' }
print dict['one']
print dict[2]
print tinydict
print tinydict.keys()
print tinydict.values()

#1
d1 = {'key1':'value1', 'key2':'value2'}; print d1
d2 = {'Alice':'2341', 'Beth':'9102', 'Cecil':'3258'}; print d2

#2
d1 = {'Name':'Zara', 'Age':7, 'Class':'First'};
print d1['Name']
print d1['Age']
#print d1['Alice']

#3
d1 = {'Name':'Zara', 'Age':7, 'Class':'First'}
d1['Age'] = 8
print d1['Age']
d1['School'] = 'DPS School'
print d1

#4
d1 = {'Name':'Zara', 'Age':7, 'Class':'First'}
del d1['Name']; print d1
d1.clear(); print d1
del d1;

#5
d1 = {'Name':'Zara', 'Name':'Zjw','Age':7, 'Class':'First'}; print d1

#6
#d1 = {['Name']:'Zara', 'Hello':'World'}; print d1

#7
d1 = {'Name':'Zara','Age':7, 'Class':'First'}; print d1
d2 = {'Name':'Zjw', 'Age':7, 'Class':'First'}
print cmp(d1, d2)
print len(d1)
print str(d1)
print type(d1)

#8
d1 = {'Name':'Zara','Age':7, 'Class':'First'}; print d1, "8"
print d1.copy()
list1 = ["a", "b", "c"]
d2 = {}.fromkeys(('x', 'y', 'z', 'u', 'v', 'w'), -1); print d2
#d3 = dict(['x',1], ['y', 2]); print d3
print d1.get('Name')
print d1.has_key('Name')
print d1.items()
print d1.keys()
print d1.values()
d1.setdefault('Zjw', 'Hi')
print d1.get('Zjw')
