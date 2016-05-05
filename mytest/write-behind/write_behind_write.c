/* *
 * Used for testing only writint 1KB data to the glusterfs-dht 
 * and learning the write-behind how to work!
 * 2015-11-06 17:32:00
 * */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    int fd = -1;
    int write_ret = -1;
    int page_size = 131072; /* write-behind 128KB to wind */
    char c = '1';

    if (argc != 2){
        printf ("\nUsage: %s filepath\n", argv[0]);
        exit(-1);
    }

    fd = open (argv[1], O_RDWR|O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        exit(errno);
    }

    while(page_size) {
        write_ret = write(fd, &c, 1);
        if (write_ret != 1) {
            perror("write");
            break;
        }
        page_size --;
    }

    while(1);

    return 0;
}
