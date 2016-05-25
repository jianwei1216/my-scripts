#!/usr/bin/python

import os
import time
import sys

def date_to_seconds(filename):
    is_one = False
    is_two = False
    one = 0.0
    two = 0.0
    # open file
    fp = open(filename, 'r')

    # read file line
    while True:
        fr = fp.readline().strip('\n')
        if fr == '':
            break;
        # date_to_seconds
        seconds = time.mktime(time.strptime(fr, "%Y-%m-%d %H:%M:%S"))
        #print seconds
        if is_one == False:
            is_one = True
            one = seconds
        if is_two == False:
            is_two = True
            two = seconds
        if is_one == True and is_two == True:
            #print one, two
            print two - one
            one = two
            is_two = False

    # close file
    fp.close()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage: python ' + __file__ + ' filename'
        exit(-1)

    filename = sys.argv[1]
    date_to_seconds(filename)
