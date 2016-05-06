#!/usr/bin/python

import py_compile
import sys
import os

def mkpyc(filepath):
    py_compile.compile(filepath)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print "Usage: python " + __file__ + " filepath"
        exit(0)

    filepath = sys.argv[1]
    mkpyc(filepath)
