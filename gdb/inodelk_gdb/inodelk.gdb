#!/bin/sh
###############################################
#  1. writing file1
#  2. print pl_inodelk --> inode
#  3. rename file2 file1 file2
#  4. print pl_inodelk --> inode
#  5. this case: will be successful because of
#     pl_inodelk --> inode is not release !!!
###############################################

set pagination off
set print pretty
set print repeats 0
set logging on ./inodelk.log
















