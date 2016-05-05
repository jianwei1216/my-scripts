#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

int main(void)
{
    int fd = -1;
    int ret = -1
    struct flock tmplk = {0};
    struct old_linux_dirent tmpdir;
    DIR *dirp = NULL;

    dirp = opendir ("/root/tmp/");
    if (dirp == NULL) {
        perror("opendir()"); 
        goto out;
    }

    fd = dirfd (dirp);
    if (fd == -1) {
        perror("dirfd()");
        goto out;
    }
    printf ("%d\n", fd);

    while (1) {
        ret = readdir (fd, &tmpdir, 0);
        if (ret == -1) {
            perror ("readdir()");
            break;
        } else if (ret == 0) {
            printf ("end-of-dir!\n");
            break;
        }
        printf ("name=%s, inode=%d, off=%ld, len=%d\n",
                tmpdir.d_name, tmpdir.d_ino, tmpdir.d_off,
                tmpdir.d_reclen);
    }

    tmplk.l_type = F_WRLCK;
    tmplk.l_whence = SEEK_SET;
    tmplk.l_start = 0;
    tmplk.l_len = 10;
    tmplk.l_pid = 0;

#if 0
    ret = fcntl(fd, F_SETLK, &tmplk);
    if (ret == -1) {
        perror ("fcntl(F_WRLCK)");
        goto out;
    }
#endif
 
    /*sleep(30);*/

    tmplk.l_type = F_UNLCK;
    tmplk.l_whence = SEEK_SET;
    tmplk.l_start = 0;
    tmplk.l_len = 10;
    tmplk.l_pid = 0;

#if 0
    ret = fcntl(fd, F_SETLK, &tmplk);
    if (ret == -1) {
        perror ("fcntl(F_UNLCK)");
        goto out;
    }
#endif

out:
    if (dirp)
        closedir(dirp);

    if (fd != -1)
        close (fd);

    return 0;
}
