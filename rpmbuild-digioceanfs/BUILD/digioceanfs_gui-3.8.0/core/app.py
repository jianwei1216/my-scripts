#!/usr/bin/env python
#-*- coding: utf-8 -*-

import os
import sys

currdir = os.path.join(os.getcwd(),os.path.dirname(__file__))
if currdir not in sys.path:
    sys.path.append(currdir)
from base import *
import index

web.wsgi.runwsgi = lambda func, addr=None: web.wsgi.runfcgi(func, addr)

if __name__ == '__main__':
	app.run()
