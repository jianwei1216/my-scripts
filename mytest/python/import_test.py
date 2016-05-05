#!/usr/bin/python


# 1
import func

func.printme ("Zara")


# 2
from func import printinfo
printinfo ('Jaaj')


# 3
from func import *
print func.sum(1, 2)

# 4
Money = 200

def AddMoney():
    global Money
    Money = Money + 1

print Money
AddMoney ()
print Money

# 5
import math
print dir(math)
print math.log(3.14)
print math.__name__
print math.__file__

# 6
print '#' * 50
print reload(func)


