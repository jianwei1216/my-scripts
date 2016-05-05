#!/usr/bin/python

# 1
def try_test():
    try:
        print __name__
        print __file__
        fo = open ("testfile", "r")
        #fo.write ("This is my test file for exception handing!\n")
    except IOError:
        print "Error: can\'t find file or read data!"
    else:
        print "Written content in the file successfully!"
        fo.close()

#try_test()

# 2
def try_test2():
    try:
        fh = open ("testfile", "w")
        fh.write ("This is my test!\n")
    finally:
        print "Error: can\'t find file or read data", "\nhello"
        fh.close()

#try_test2()

# 3
def try_test3():
    try:
        fh = open ("testfile", "w")
        try:
            fh.write("Hello Python!\n")
        finally:
            print 'Going to close file'
            fh.close()
    except IOError:
        print "Error: can\'t find file or read data"

#try_test3()

# 4
def try_test4(var):
    try:
        return int(var)
    except ValueError, Argument:
        print 'The argument does not contain numbers: ', Argument

#print try_test4("xyz")

# 5
def try_test5(level):
    if level < 1:
        raise 'Invalid level: ', level

#try_test5(0)

# 6
class Networkerror(RuntimeError):
    def __init__(self, arg):
        self.args = arg

def try_test6():
    try:
        raise Networkerror("Bad hostname")
    except Networkerror, e:
        print e.args

try_test6()
