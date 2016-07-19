#!/usr/bin/python


try:
    import cPickle as pickle
except ImportError:
    import pickle
    

def pickle_test():
    info_dict = {'name':'yeho', 'age':'100', 'Lang':'Python'}
    f = open('info.pkl', 'wb')
    pickle.dump (info_dict, f)
    f.close()

def cpickle_test():
    d = {'name':'Bob', 'age':20, 'score':88}
    print pickle.dumps(d)
    f = open('dump.pkl', 'wb')
    pickle.dump(d, f)
    f.close()
    
    f = open('dump.pkl', 'rb')
    d = pickle.load(f)
    f.close()
    print d


if __name__ == '__main__':
    cpickle_test()
