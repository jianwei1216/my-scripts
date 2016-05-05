#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char* argv[]) 
{
        int fd = -1;

        if (argc != 2) {
                printf ("Usage: %s /cluster2/test/FileName\n",
                        argv[0]);
                exit(1);
        }

        if (strncmp("/cluster2/test", argv[1], 14)) {
                printf ("Please input FilePath: /cluster2/test\n");
                exit(1);
        }

        fd = open (argv[1], O_RDWR, 0644);
        if (fd == -1) {
                perror ("open()");
                exit(-1);
        }

        if (lseek (fd, 0, SEEK_SET) < 0) {
                perror ("lseek()");
        }
        if (write (fd, "hello", strlen("hello")) != strlen("hello"))
                perror("write()");

        close (fd);

        return 0;
}
