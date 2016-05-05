#!/usr/bin/python

# 1
class Employee:
    'all employees'
    empCount = 0

    def __init__(self, name, salary):
        self.name = name
        self.salary = salary
        Employee.empCount += 1

    def displayCount(self):
        print "Total Employee %d" % Employee.empCount

    def displayEmployee(self):
        print "Name : ", self.name, " Salary : ", self.salary

def class_test1 ():
    emp1 = Employee ("Zara", 2000)
    emp2 = Employee ("Manni", 5000)

    emp1.displayCount()
    emp2.displayEmployee()
    print 'Total Employee %d' % Employee.empCount

    emp1.age = 7
    emp2.age = 8

    print 'Emp1 age : %d' % emp1.age
    #del emp1.age
    print hasattr(emp1, 'age')
    print getattr(emp1, 'age')
    setattr (emp1, 'age', 8)
    delattr (emp1, 'age')
    #print 'Emp1 age : %d' % emp1.age

    print '__doc__:\t', Employee.__doc__
    print '__name__:\t', Employee.__name__
    print '__module__:\t', Employee.__module__
    print '__bases__:\t', Employee.__bases__
    print '__dict__:\t', Employee.__dict__

#class_test1()

# 2
class Point:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y

    def __del__(self):
        class_name = self.__class__.__name__
        print class_name, 'destroy!'

def class_test2():
    pt1 = Point()
    pt2 = pt1
    pt3 = pt1

    print id(pt1), id(pt2), id(pt3)

    del pt1
    del pt2
    del pt3

#class_test2()

# 3
class Parent:
    parentAttr = 100
    
    def __init__(self):
        print 'call parent __init__'

    def parentMethod(self):
        print 'call parent method'

    def setAttr(self, attr):
        Parent.parentAttr = attr

    def getAttr(self):
        print 'Parent attr: ', Parent.parentAttr 

class Child(Parent):
    def __init__(self):
        print 'call child __init__'

    def childMethod(self):
        print 'call child method'

def class_test3():
    c = Child()
    c.childMethod()
    c.parentMethod()
    c.setAttr(200)
    c.getAttr()

    print issubclass(Child, Parent)
    print isinstance(c, Child)

#class_test3()

# 4
class Parent:
    def myMethod(self):
        print 'call parent method'

class Child:
    def myMethod(self):
        print 'call child method'

def class_test4():
    c = Child()
    c.myMethod()

#class_test4()


# 5
class Vector:
    def __init__(self, a, b):
        self.a = a
        self.b = b

    def __str__(self):
        return 'Vector (%d, %d)' % (self.a, self.b)

    def __add__(self, other):
        return Vector(self.a + other.a, self.b + other.b)

def class_test5():
    v1 = Vector(2, 10)
    v2 = Vector(5, -2)
    print v1 + v2

#class_test5()


# 6
class JustCounter:
    __secretCount = 0
    publicCount = 0

    def count(self):
        self.__secretCount += 1
        self.publicCount += 1
        #JustCounter.publicCount += 1
        print self.__secretCount

def class_test6():
    counter = JustCounter()
    counter.count()
    counter.count()
    print counter.publicCount
    #print counter.__secretCount

    counter1 = JustCounter()
    counter1.count()
    counter1.count()
    print counter1.publicCount

    print JustCounter.publicCount

#class_test6()

