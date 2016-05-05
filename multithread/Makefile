CC = gcc
CFLAGS = -g -O0
LDFALGS = -lpthread -lgfapi
# 只读不写  即读客户端用该宏
#MACRO = -D __JUST_READ_NOT_WRITE_
# 只写不读  即写客户端用该宏
MACRO = -D __JUST_WRITE_NOT_READ_

all: posix gluster

posix:posix.c
	$(CC) $(CFLAGS) $(LDFALGS) $(MACRO) $< -o $@

gluster:gluster.c
	$(CC) $(CFLAGS) $(LDFALGS) $(MACRO) $< -o $@

clean:
	rm -rf posix gluster
