/*
 * =====================================================================================
 *
 *       Filename:  write.c
 *
 *    Description:  write
 *
 *        Version:  1.0
 *        Created:  01/18/2016 02:42:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hustpeior (), hustpuyuegang@163.com
 *        Company:  casic
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


void writev_test(const char* filename)
{
    int fd = -1;
    int writev_count = -1;
    struct flock lock = {0,};

    if (!filename) {
        printf ("arg is NULL!\n");        
        goto out;
    }
    
    fd = open (filename, O_RDWR|O_CREAT, 0644);
    if (fd == -1) {
        perror("open()");
        goto out;
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror ("fcntl()");
        goto out;
    }

    writev_count = write (fd, filename, strlen(filename));
    if (writev_count != strlen(filename)) {
        perror ("write()");
        goto out;
    }

    close (fd);

out:
    return;
}

int main(void)
{
    writev_test("/cluster2/test/newfile"); 

    return 0;
}
