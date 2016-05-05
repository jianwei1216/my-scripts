TARGET=shark
CONF=`pkg-config --cflags --libs glib-2.0`
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
CC=gcc
CFLAGS=-Wall -g -lpthread   -lcrypto -lssl 
FLAGS=-lcrypto -lssl 

%.o:%.c
	$(CC) $(CFLAGS) $(CONF) -c $< -o $@

all:$(OBJS)
	$(CC) $(OBJS) $(CONF) -o $(TARGET)  $(FLAGS)

.PHONE:clean

clean:
	rm -rf *.o $(TARGET)
