CC = gcc
CFLAGS = -g -O0
ISA_INCLUDE = -I/usr/include/isa-l
LDFLAGS = -L/usr/lib64 -lisal

test:decode_list.c
	$(CC) $(CFLAGS) $(ISA_INCLUDE) $(LDFLAGS) $< -o $@

clean:
	rm -f test
