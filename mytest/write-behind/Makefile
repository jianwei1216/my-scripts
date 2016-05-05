CC = gcc
CFLAGS = -g -O0

.PHONY:all
all: write_behind_write 

write_behind_write: write_behind_write.c
	$(CC) $(CFLAGS) $< -o $@

write_behind_read: write_behind_read.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -f write_behind_write write_behind_read

