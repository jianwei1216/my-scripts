#!/usr/bin/python

import re

# 1
def re_test():
    print (re.match('www', 'www.baidu.com').span())
    print (re.match('com', 'www.baidu.com'))

#re_test()

# 2
def re_test2():
    line = 'Cats are smarter than dogs'
    searchObj = re.search(r'(.*) are (.*?) .*', line, re.M|re.I)

    if searchObj:
        print 'group():', searchObj.group()
        print 'group(1):', searchObj.group(1)
        print 'group(2):', searchObj.group(2)
    else:
        print "No match!"

#re_test2()

# 3
def re_test3():
    print (re.search('www', 'www.baidu.com').span())
    print (re.search('com', 'www.baidu.com').span())

#re_test3()

# 4
def re_test4():
    line = 'Cats are smarter than dogs'
    matchObj = re.match(r'dogs',  line, re.M|re.I)
    if matchObj:
        print 'match --> matchObj.group():', matchObj.group()
    else:
        print 'No match!!!'

    searchObj = re.search(r'dogs',  line, re.M|re.I)
    if searchObj:
        print 'search --> matchObj.group():', searchObj.group()
    else:
        print 'No match!!!'


#re_test4()

# 5
def re_test5():
    phone = '2016-04-12'
    num = re.sub(r'#.*$', "", phone)
    print 'Phone number1: ', num

    num =re.sub(r'\D', "", phone)
    print 'Phone number2: ', num

re_test5()




