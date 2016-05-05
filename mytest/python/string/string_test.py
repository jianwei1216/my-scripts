#!/usr/bin/python

#1
def string_test1():
    url = "git clone 10.10.88.88:/root/git/linshiyue/test/digioceanfs/"
    tmp = url[-2]
    print tmp
    print tmp.isalpha()
    tmpstr = url.split("/")
    print tmpstr[-2]
    print url

    tup1 = (1, 2, 3)
    print len(tup1)

string_test1()

