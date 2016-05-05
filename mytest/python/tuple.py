#!/usr/bin/python

tuple = ( 'abcd', 786, 2.23, 'john', 70.2 )
tinytuple = ( 123, 'john' )

print tuple
print tuple[0]
print tuple[1:3]
print tuple[2:]
print tinytuple * 2
print tuple + tinytuple

#tuple[2] = 1000

#1
tup1 = ('physics', 'chemistry', 1997, 2000); print tup1
tup2 = (1, 2, 3, 4, 5); print tup2
tup3 = ("a", "b", "c", "d"); print tup3

#2
tup1 = (); print tup1
tup2 = (50,); print tup2

#3
tup1 = ('physics', 'chemistry', 1997, 2000)
tup2 = (1, 2, 3, 4, 5, 6, 7)
print tup1[0]
print tup2[1:5]

#4
tup1 = (12, 34, 56)
tup2 = ("abc", "xyz")
#tup1[0] = 20 ===> error
tup3 = tup1 + tup2; print tup3

#5
tup = ('physics', 'chemistry', 1997, 2000)
#print tup; del tup; print tup

#6
print len((1,2,3))
print (1,2,3) + (4,5,6)
print ('Hi!') * 4
print 3 in (1, 2, 3)
for x in (1, 2, 3): print x

#7
L = ('spam', 'Spam', 'SPAM!')
print L[2]
print L[-2]
print L[1:]

#8
print 'abc', -4.24e93, 18+6.6j, 'xyz'
x,y = 1,2; print x, y

#9
tup1 = (1, 2, 3, 1)
print cmp(tup1, (2,))
print len(tup1)
print max(tup1)
print min(tup1)
print list(tup1)
