#!/usr/bin/python
# -*- coding:UTF-8 -*-

import sys
import argparse
import textwrap
import math

def args_get_sum_or_max_number():
        # 1. 创建一个parser对象
        parser = argparse.ArgumentParser (description='Process some integers.')
        # 2. 向对象parser中添加参数
        parser.add_argument ('integers', metavar='N', type=int, nargs='+',
                              help='an integer for the accumulator')
        parser.add_argument ('--sum', dest='accumulate', action='store_const',
                             const=sum, default=max,
                             help='sum the integers (default: find the max)')
        # 3. 
        args = parser.parse_args()
        print args.accumulate (args.integers)

def args_prog():
        parser = argparse.ArgumentParser(prog='myprogram')
        parser.add_argument ('--foo', help='foo help of the %(prog)s program') 
        args = parser.parse_args()
        parser.print_help()

def args_usage():
        parser = argparse.ArgumentParser(prog='PROG', usage='%(prog)s [options]')
        parser.add_argument ('--foo', nargs='?', help='foo help')
        parser.add_argument ('bar', nargs='+', help='bar help')
        parser.print_help()

def args_descrition():
        parser = argparse.ArgumentParser(description='A foo that bars')
        parser.print_help()

def args_epilog():
        parser = argparse.ArgumentParser(description='A foo tha bars',
                                         epilog='And that\'s how you\'d foo a bar')
        parser.print_help()

def args_parents():
        # parent
        parent_parser = argparse.ArgumentParser(add_help=False)
        parent_parser.add_argument('--parent', type=int)

        # child
        foo_parser = argparse.ArgumentParser(parents=[parent_parser])
        foo_parser.add_argument('foo')
        print foo_parser.parse_args(['--parent', '2', 'XXX'])
        foo_parser.print_help()

        # child
        bar_parser = argparse.ArgumentParser(parents=[parent_parser])
        bar_parser.add_argument('--bar')
        print bar_parser.parse_args(['--bar', 'YYY'])
        bar_parser.print_help()

def args_formatter_class():
        #parser = argparse.ArgumentParser(prog='PROG',
        #                                 description='''this description 
        #                                 was indented weird but that is okay''',
        #                                 epilog='''likewise for this epilog whose 
        #                                 whitespace will be cleaned up and whose 
        #                                 words will be wrapped across a couple lines''')
        #parser.print_help()

        # 1. ====>RawDescriptionHelpFormatter<====
        #parser = argparse.ArgumentParser(prog='PROG',
        #                                 formatter_class=argparse.RawDescriptionHelpFormatter,
        #                                 description=textwrap.dedent('''
        #                                        Please do not mess up this text!
        #                                        --------------------------------
        #                                                I have indented it
        #                                                exactly the way
        #                                                I want it'''))
        #parser.print_help()

        # 2. ====>RawTextHelpFormatter<====
        pass

        # 3. ====>ArgumentDefaultsHelpFormatter<====
        parser = argparse.ArgumentParser(prog='PROG',
                                         formatter_class=argparse.ArgumentDefaultsHelpFormatter)        
        parser.add_argument('--foo', type=int, default=42, help='FOO!')
        parser.add_argument('bar', nargs='*', default=[1,2,3], help='BAR!')
        parser.print_help()

def args_prefix_chars():
        parser = argparse.ArgumentParser(prog='PROG', prefix_chars='-+')
        parser.add_argument('+f')
        parser.add_argument('++bar')
        print parser.parse_args('+f X ++bar Y'.split())

def args_fromfile_prefix_chars():
        with open('args.txt', 'w') as fp:
                fp.write('-f\nbar')
        parser = argparse.ArgumentParser(fromfile_prefix_chars='@')
        parser.add_argument('-f')
        print parser.parse_args(['-f', 'foo', '@args.txt'])

def args_argument_default():
        parser = argparse.ArgumentParser()#argument_default=argparse.SUPPERSS)
        parser.add_argument('--foo')
        parser.add_argument('bar', nargs='?')
        print parser.parse_args(['--foo', '1', 'BAR'])
        print parser.parse_args([])

def args_conflict_handler():
        parser = argparse.ArgumentParser(prog='PROG', conflict_handler='resolve')
        parser.add_argument('-f', '--foo', help='old foo help')
        parser.add_argument('--foo', help='new foo help')
        args = parser.parse_args()
        print args
        print args.foo

def args_add_help():
        #parser = argparse.ArgumentParser(prog='PROG', add_help=False)
        #parser.add_argument('--foo', help='foo help')
        #args = parser.parse_args()
        #parser.print_help()
        #print args

        parser = argparse.ArgumentParser(prog='PROG', prefix_chars='+/')
        parser.print_help()

def args_name_or_flags():
        parser = argparse.ArgumentParser(prog='PROG')
        parser.add_argument('-f', '--foo')
        parser.add_argument('bar')
        #print parser.parse_args(['BAR'])
        #print parser.parse_args(['BAR', '--foo', 'FOO'])
        args = parser.parse_args(['10.10.21.111', '--foo', 'FOO'])
        print 'args.bar=', args.bar
        print 'args.foo=', args.foo

def args_action():
        #default store
        parser = argparse.ArgumentParser()
        parser.add_argument('--foo')
        args = parser.parse_args('--foo 1'.split())
        #print 'args.foo=', args.foo

        # store_const
        parser1 = argparse.ArgumentParser()
        parser1.add_argument('--foo', action='store_const', const=42)
        args = parser1.parse_args(['--foo'])
        #print 'args.foo=', args.foo

        # store_true/false
        parser2 = argparse.ArgumentParser()
        parser2.add_argument('--foo', action='store_true')
        parser2.add_argument('--bar', action='store_false')
        parser2.add_argument('--baz', action='store_true')
        args = parser2.parse_args('--foo --bar'.split())
        #print args

        # append
        parser3 = argparse.ArgumentParser()
        parser3.add_argument('--foo', action='append')
        args = parser3.parse_args('--foo 1 --foo 2'.split())
        #print args

        # append_const
        parser4 = argparse.ArgumentParser()
        parser4.add_argument('--str', dest='types', action='append_const', const=str)
        parser4.add_argument('--int', dest='types', action='append_const', const=int)
        args = parser4.parse_args('--str --int'.split())
        #print args

        # count
        parser5 = argparse.ArgumentParser()
        parser5.add_argument('--verbose', '-v', action='count')
        args = parser5.parse_args(['-vvvvvvvvvvv'])
        #print args
        
        # help
        # version
        parser6 = argparse.ArgumentParser()
        parser6.add_argument('--version', action='version', version='%(prog)s 2.0')
        #args = parser6.parse_args(['--version'])
        #print args

        class FooAction(argparse.Action):
                def __init__(self, option_strings, dest, nargs=None, **kwargs):
                        if nargs is not None:
                                raise ValueError('nargs not allowed')
                        super(FooAction, self).__init__(option_strings, dest, **kwargs)
                def __call__(self, parser, namespace, values, option_strings=None):
                        print '%r %r %r' % (namespace, values, option_strings)
                        setattr(namespace, self.dest, values)

        parser7 = argparse.ArgumentParser()
        parser7.add_argument('--foo', action=FooAction)
        parser7.add_argument('bar', action=FooAction)
        args = parser7.parse_args('1 --foo 2'.split())
        #print args

def args_nargs():
        # N
        #parser = argparse.ArgumentParser()
        #parser.add_argument('--foo', nargs=2)
        #parser.add_argument('bar', nargs=1)
        #args = parser.parse_args('c --foo a b'.split())
        ##print args
        #
        ## ?
        #parser1 = argparse.ArgumentParser()
        #parser1.add_argument('--foo', nargs='?', const='c', default='d')
        #parser1.add_argument('bar', nargs='?', default='d')
        #args = parser1.parse_args(['XX', '--foo', 'YY'])
        ##print args
        #args = parser1.parse_args(['XX', '--foo'])
        ##print args
        #args = parser1.parse_args([])
        ##print args

        #parser2 = argparse.ArgumentParser()
        #parser2.add_argument('infile', nargs='?', type=argparse.FileType('r'),
        #                     default=sys.stdin)
        #parser2.add_argument('outfile', nargs='?', type=argparse.FileType('w'),
        #                     default=sys.stdout)
        #args = parser2.parse_args(['input.txt', 'output.txt'])
        ##print args 
        #args = parser2.parse_args([])
        ##print args

        ## *
        #parser3 = argparse.ArgumentParser()
        #parser3.add_argument('--foo', nargs='*')
        #parser3.add_argument('--bar', nargs='*')
        #parser3.add_argument('barz', nargs='*')
        #args = parser3.parse_args('a b --foo x y --bar 1 2 3'.split())
        ##print args

        ## +
        #parser4 = argparse.ArgumentParser()
        #parser4.add_argument('foo', nargs='+')
        #args = parser4.parse_args(['a', 'b'])
        ##print args
        ##args = parser4.parse_args([])
        ##print args

        # REMAINDER
        parser5 = argparse.ArgumentParser(prog='PROG')
        parser5.add_argument('--foo')
        parser5.add_argument('command')
        parser5.add_argument('args', nargs=argparse.REMAINDER)
        #args = parser5.parse_args('--foo B cmd --arg1 XX ZZ'.split())
        args = parser5.parse_args()
        print args

def args_default():
        parser = argparse.ArgumentParser()
        parser.add_argument('--foo', default=42)
        args = parser.parse_args(['--foo', '2'])
        #print args
        args = parser.parse_args([])
        #print args

        parser1 = argparse.ArgumentParser()
        parser1.add_argument('--length', default='10', type=int)
        parser1.add_argument('--width', default=10.5, type=int)
        args = parser1.parse_args()
        #print args

        parser2 = argparse.ArgumentParser()
        parser2.add_argument('foo', nargs='?', default=42)
        args = parser2.parse_args(['a'])
        #print args
        args = parser2.parse_args([])
        #print args

        parser3 = argparse.ArgumentParser()
        parser3.add_argument('--foo', default=argparse.SUPPRESS)
        args = parser3.parse_args([])
        #print args
        args = parser3.parse_args(['--foo', '1'])
        print args

def perfect_square(string):
        value = int(string)
        sqrt = math.sqrt(value)
        if sqrt != int(sqrt):
                msg = '%r is not a perfect square' % (string)
                raise argparse.ArgumentTypeError(msg)
        return value

def args_type():
        parser = argparse.ArgumentParser()
        parser.add_argument('foo', type=int)
        parser.add_argument('bar', type=file)
        args = parser.parse_args('2 temp.txt'.split())
        #print args

        parser1 = argparse.ArgumentParser()
        parser1.add_argument('foo', type=argparse.FileType('r'))
        args = parser1.parse_args(['out.txt'])
        #print args
        #print args.foo.readline()

        parser2 = argparse.ArgumentParser(prog='PROG')
        parser2.add_argument('foo', type=perfect_square)
        #args = parser2.parse_args(['2'])
        #print args

        parser3 = argparse.ArgumentParser(prog='PROG')
        parser3.add_argument('foo', type=int, choices=xrange(5, 10))
        args = parser3.parse_args(['7'])
        print args

def args_choices():
        parser = argparse.ArgumentParser(prog='GAME')
        parser.add_argument('move', choices=['rock', 'paper', 'scissors'])
        args = parser.parse_args(['rock'])
        #print args

        parser1 = argparse.ArgumentParser()
        parser1.add_argument('door', type=int, choices=range(1, 4))
        args = parser1.parse_args(['2'])
        print args
        
def args_required():
        parser = argparse.ArgumentParser()
        parser.add_argument('--foo', required=True)
        args = parser.parse_args(['--foo', 'BAR'])
        print args
        args = parser.parse_args([])
        print args

def args_help():
        parser = argparse.ArgumentParser(prog='frobble')
        parser.add_argument('--foo', action='store_true',
                            help='foo the bars before frobbling')
        parser.add_argument('bar', nargs='+',
                            help='one of the bars to be frobbled')
        args = parser.parse_args()
        print args

def args_metavar():
        parser = argparse.ArgumentParser()
        parser.add_argument('--foo')
        parser.add_argument('bar')
        args = parser.parse_args('X --foo Y'.split())
        #parser.print_help()
        #print args

        parser1 = argparse.ArgumentParser()
        parser1.add_argument('--foo', metavar='YYY')
        parser1.add_argument('bar', metavar='XXX')
        #parser1.print_help()

        parser2 = argparse.ArgumentParser()
        parser2.add_argument('-x', nargs=2)
        parser2.add_argument('--foo', nargs=2, metavar=('bar', 'baz'))
        parser2.print_help()

def args_dest():
        parser = argparse.ArgumentParser()
        parser.add_argument('bar')
        args = parser.parse_args(['XXX'])
        #print args

        parser1 = argparse.ArgumentParser()
        parser1.add_argument('-f', '--foo-bar', '--foo')
        parser1.add_argument('-x', '-y')
        args = parser1.parse_args('-f 1 -x 2'.split())
        #print args
        args = parser1.parse_args('--foo 1 -x 2'.split())
        #print args

        parser2 = argparse.ArgumentParser()
        parser2.add_argument('--foo', dest='bar')
        args = parser2.parse_args('--foo XXX'.split())
        print args

def args_option_value_syntax():
        parser = argparse.ArgumentParser()
        parser.add_argument('-x')
        parser.add_argument('--foo')
        args = parser.parse_args(['-x', 'X'])
        print args
        args = parser.parse_args(['--foo', 'FOO'])
        print args
        args = parser.parse_args(['--foo=FOO'])
        print args
        args = parser.parse_args(['-xX'])
        print args

        parser1 = argparse.ArgumentParser()
        parser1.add_argument('-x', action='store_true')
        parser1.add_argument('-y', action='store_true')
        parser1.add_argument('-z')
        args = parser1.parse_args(['-xyzZ'])
        print args

def args_invalid_arguments():
        parser = argparse.ArgumentParser()
        parser.add_argument('--foo', type=int)
        parser.add_argument('bar', nargs='?')
        #args = parser.parse_args(['--foo', 'spam'])
        #print args
        #args = parser.parse_args(['--bar'])
        #print args
        args = parser.parse_args(['spam', 'badger'])
        print args

def args_argument_containing():
        # args argument containing -
        parser = argparse.ArgumentParser()
        parser.add_argument('-x')
        parser.add_argument('foo', nargs='?')
        #args = parser.parse_args(['-x', '-1'])
        #print args
        #args = parser.parse_args(['-x', '-1', '-5'])
        #print args

        parser1 = argparse.ArgumentParser()
        parser1.add_argument('-1', dest='one')
        parser1.add_argument('foo', nargs='?')
        args = parser1.parse_args(['-1', 'X'])
        #args = parser1.parse_args(['-2'])
        #args = parser1.parse_args(['-1', '-1'])
        args = parser1.parse_args(['--', '-f'])
        print args

def args_argument_abbreviations():
        parser = argparse.ArgumentParser()
        parser.add_argument('-bacon')
        parser.add_argument('-badger')
        args = parser.parse_args('-bad MMM'.split())
        print args
        args = parser.parse_args('-bac WOOD'.split())
        print args
        args = parser.parse_args('-ba BA'.split())
        print args

def args_beyond_sys_argv():
        parser = argparse.ArgumentParser()
        parser.add_argument('integers', metavar=int, type=int, choices=xrange(10),
                            nargs='+', help='an integer in the range 0..9')
        parser.add_argument('--sum', dest='accumulate', action='store_const', const=sum,
                            default=max, help='sum the integers(default: find the max)')
        args = parser.parse_args(['1', '2', '3', '4'])
        print args
        args = parser.parse_args(['--sum', '1', '2', '3', '4'])
        print args

def args_namespace_object():
        parser = argparse.ArgumentParser()
        parser.add_argument('--foo')
        args = parser.parse_args(['--foo', 'BAR'])
        print args
        print vars(args)

        class C(object):
                pass

        c = C()
        parser = argparse.ArgumentParser()
        parser.add_argument('--foo')
        parser.parse_args(args=['--foo', 'BAR'], namespace=c)
        print args.foo
        print c.foo

if __name__ == '__main__':
        #args_get_sum_or_max_number()
        #args_prog()
        #args_usage()
        #args_descrition()
        #args_epilog()
        #args_parents()
        #args_formatter_class()
        #args_prefix_chars()
        #args_fromfile_prefix_chars()
        #args_argument_default()
        #args_conflict_handler()
        #args_add_help()
        #args_name_or_flags()
        #args_action()
        args_nargs()
        #args_default()
        #args_type()
        #args_choices()
        #args_required()
        #args_help()
        #args_metavar()
        #args_dest()
        #args_option_value_syntax()
        #args_invalid_arguments()
        #args_argument_containing()
        #args_argument_abbreviations()
        #args_beyond_sys_argv()
        #args_namespace_object()
