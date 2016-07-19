CC = gcc
C_FLAGS = -lpthread -g

threads_pool: threads_pool.c threads_pool.h
	$(CC) $< $(C_FLAGS) -o $@

clean:
	rm -f threads_pool
