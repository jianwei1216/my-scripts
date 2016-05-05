#define _GNU_SOURCE
#include <dirent.h>     /*  Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE);  } while (0)

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];

};

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int fd, nread, fd2;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    int ret;
    char d_type;
    struct flock tmplk = {0};

#if 0
    fd2 = open(argv[1], O_RDWR);
    if (fd2 == -1)
        handle_error("open");
#endif

#if 1
    fd = open(argc > 1 ? argv[1] : ".", O_RDONLY | O_DIRECTORY);
    if (fd == -1)
        handle_error("open");
#endif

    tmplk.l_type = F_WRLCK;
    tmplk.l_whence = SEEK_SET;
    tmplk.l_start = 0;
    tmplk.l_len = 10;
    tmplk.l_pid = 0;

    ret = fcntl(fd, F_SETLK, &tmplk);
    if (ret == -1) {
        perror ("fcntl(F_WRLCK)");
        goto out;
    }

#if 0
    for ( ; ;  ) {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1)
            handle_error("getdents");

        if (nread == 0)
            break;

        printf("--------------- nread=%d ---------------\n", nread);
        printf("i-node#  file type  d_reclen  d_off   d_name\n");
        for (bpos = 0; bpos < nread;) {
            d = (struct linux_dirent *) (buf + bpos);
            printf("%8ld  ", d->d_ino);
            d_type = *(buf + bpos + d->d_reclen - 1);
            printf("%-10s ", (d_type == DT_REG) ?  "regular" :
                    (d_type == DT_DIR) ?  "directory" :
                    (d_type == DT_FIFO) ? "FIFO" :
                    (d_type == DT_SOCK) ? "socket" :
                    (d_type == DT_LNK) ?  "symlink" :
                    (d_type == DT_BLK) ?  "block dev" :
                    (d_type == DT_CHR) ?  "char dev" : "???");
            printf("%4d %10lld  %s\n", d->d_reclen,
                    (long long) d->d_off, (char *) d->d_name);
            bpos += d->d_reclen;

        }
    }
#endif
out:
    exit(EXIT_SUCCESS);
}
