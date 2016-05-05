#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_BUF_SIZE 512

int main(void)
{
    char *cmd = "python /usr/local/digioceanfs_manager/manager/digi_manager.pyc node list_disk";
    FILE *fp = NULL;
    char buf[MAX_BUF_SIZE] = {0,};
    char *s = NULL;

    fp = popen(cmd, "r");
    if (!fp) {
        perror("allocate fp failed!");
        exit (-1);
    }

    while (fgets(buf, sizeof(buf), fp))
        printf ("%s", buf);

    pclose(fp);

    return 0;
}
