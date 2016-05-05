#!/usr/bin/python

list = [ 'abcd', 786, 2.23, 'john', 70.2 ]
tinylist = [ 123, 'john' ]
print list #abcd 786 2.23 john 70.2
print list[0] #abcd
print list[1:3] #786 2.23
print list[2:] # 2.23 john 70.2
print tinylist * 2 #123 john 123 john
print list + tinylist # abcd 786 2.23 john 70.2 123 john


print list[2]
list[2] = 1000
print list[2]

# 1
list1 = ['physics', 'chemistry', 1997, 2000]
list2 = [1, 2, 3, 4, 5]
list3 = ["a", "b", "c", "d"]
print list1
print list2
print list3

#2
list1 = ['physics', 'chemistry', 1997, 2000]
list2 = [1, 2, 3, 4, 5, 6, 7]
print "list1[0]: ", list1[0]
print "list2[1:5]: ", list2[1:5]

#3
list1 = ['physics', 'chemistry', 1997, 2000]
print "Value available at index 2 : "
print list1[2]
list[2] = 2001
print 'New value available at index 2 : '
print list[2]

#4
list1 = ['physics', 'chemistry', 1997, 2000]
print list1
del list1[2]
print "After del value at index 2 : "
print list1

#5
print len(list1)
print [1, 2, 3] + [4, 5, 6]
print ['Hi'] * 4
print 3 in [1, 2, 3]
for x in [1,2,3]: print x

#6
L = ['spam', 'Spam', 'SPAM!']
print L[2]
print L[-2]
print L[1:]
print L[-1]

#7
list1 = ['a', 'b', 'c']
list2 = ['b', 'b', 'c']
tuple1 = (1,2,3)
print cmp(list1, list2)
print len(list1)
print max(list1)
print min(list1)
print tuple(list1)

#8
list1 = ['a', 'a', 'b', 'c']; print list1
list1.append('d'); print list1
print list1.count('a')
list1.extend(['e', 'f']); print list1
print list1.index('e')
list1.insert(4, 'z'); print list1
list1.pop(4); print list1
list1.remove('a'); print list1
list1.reverse(); print list1
list1.sort();print list1



