#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define BUF_SIZE    10240
#define FILE_SIZE   20000

int main(int argc, char* argv[])
{
        int fd = -1;
        int file_size = FILE_SIZE;
        char *buf = NULL;

        if (argc != 2) {
                printf ("Usage: %s /cluster2/test/FileName\n",
                        argv[0]);
                exit(1);
        }

        if (strncmp("/cluster2/test", argv[1], 14)) {
                printf ("Please input FilePath: /cluster2/test\n");
                exit(1);
        }

        buf = calloc(BUF_SIZE, sizeof(char));
        if (!buf) {
                perror("calloc()");
                exit(-1);
        }
        memset (buf, '*', BUF_SIZE);
        buf[BUF_SIZE-1] = '\n';

        fd = open (argv[1], O_RDWR|O_CREAT, 0644);
        if (fd == -1) {
                perror ("open()");
                exit(-1);
        }

        while (--file_size) {
                if (write(fd, buf, BUF_SIZE) != BUF_SIZE) {
                        perror ("write()");
                        break;
                }
        }

        close (fd);

        if (buf) 
            free (buf);

        return 0;
}
