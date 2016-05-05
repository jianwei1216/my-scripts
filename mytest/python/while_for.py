#!/usr/bin/python

# while 
count = 0
while (count < 9):
    print 'The count is:', count 
    count = count + 1

print 'Good Bye!'

i = 0
while i < 10:
    i += 1
    if i % 2 > 0:
        continue
    print i

i = 1
while 1:
    print i
    i += 1
    if i > 10:
        break

#var = 1
#while var == 1:
    #num = raw_input ("Enter a number: ")
    #print 'You entered: ', num

count = 0
while count < 5:
    print count, " is less than 5."
    count += 1
else:
    print count, " is not less than 5."

#flag = 1
#while (flag): print 'Given flag is really true!'

for letter in 'Python':
    print 'Current character: ', letter

fruits = ['banana', 'apple', 'mango']
print 'for 1'
for fruit in fruits:
    print 'Current fruit is ', fruit

print 'for 2'
for index in range(len(fruits)):
    print 'Current fruit is ', fruits[index]

print 'for 3'
for num in range (10,20):
    for i in range (2, num):
        if num % i == 0:
            j = num / i
            print '%d == %d * %d' %(num, i, j)
            break
    else:
        print num, 'is a prime.'

print 'while 1'
i = 2
while (i < 100):
    j = 2
    while (j <= (i/j)):
        if not (i%j): break
        j = j + 1
    if (j > i/j): print i, " is a prime."
    i = i + 1

print 'break 1'
for letter in 'Python':
    if letter == 'h':
        continue
        #break
    print 'Current letter is ', letter

print 'break 2'
var = 10
while var > 0:
    var = var - 1
    if var == 5:
        continue
        #break
    print 'Current variable value is ',var

print 'pass 1'
for letter in 'Python':
    if letter == 'h':
        pass
        print 'This is pass'
    print 'Current letter is ', letter

print 'Good bye'
