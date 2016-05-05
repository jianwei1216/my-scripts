/* *
 * Used for testing only read 1KB data to the glusterfs-dht 
 * and learning the write-behind how to work!
 *
 *  wb_readv
 *
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
    int read_ret= -1;
    char c = '0';

    if (argc != 2){
        printf ("\nUsage: %s filepath\n", argv[0]);
        exit(-1);
    }

    fd = open (argv[1], O_RDONLY, 0644);
    if (fd < 0) {
        perror("open");
        exit(errno);
    }

    while(read_ret == 0) {
        read_ret = read (fd, &c, 1);
        if (read_ret == -1) {
            perror("read"); 
            break;
        }
    }

    while(1);

    return 0;
}
