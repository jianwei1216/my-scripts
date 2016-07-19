#!/usr/bin/python

import virtkey
import time

def virtkey_test():
    #v = virtkey.virtkey()
    v = virtkey.virtkey()
    v.press_keysym(65507)
    v.press_unicode(ord('v'))
    v.release_unicode(ord('v'))

if __name__ == '__main__':
    virtkey_test()
