CC 			= gcc
CFLAGS 		= -g -Wall -o0
LDFLAGS 	= -L/usr/lib64 -lisal
INCLUDES 	= -I/usr/include/isa-l

test:test.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f test
