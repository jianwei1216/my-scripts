#!/usr/bin/python

COLORPRE = '\033'
RED     = COLORPRE + '[31m'
GREEN   = COLORPRE + '[32m'
YELLOW  = COLORPRE + '[33m'
BLUE    = COLORPRE + '[34m'
PURPLE  = COLORPRE + '[35m'
ENDC    = COLORPRE + '[0m'

if __name__ == '__main__':
        print RED + 'RED' + ENDC
        print YELLOW + 'YELLOW' + ENDC
        print GREEN + 'GREEN' + ENDC
        print BLUE + 'BLUE' + ENDC
