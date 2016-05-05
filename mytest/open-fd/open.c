#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    int fd1 = -1;
    int fd2 = -1;

    if (argc != 2) {
        printf ("Usage: %s /cluster2/test/FileName\n", argv[0]);
        exit(1);
    }

    fd1 = open (argv[1], O_RDWR);
    printf ("fd1 = %d\n", fd1);

    fd2 = open (argv[1], O_RDWR);
    printf ("fd2 = %d\n"), fd2;

    close(fd1);
    close(fd2);

    return 0;
}
