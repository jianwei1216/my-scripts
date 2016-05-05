#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (void)
{
    char buf[] = "hello world ni hao";
    char *s = NULL;
    char *m = NULL;
    char buff[128] = {0};

    strncpy (buff, buf, strlen(buf));

    /*s = buf;*/
    asprintf (&m, "%s", buf);
    s = m;
    printf ("1:s: %s\n", m);

    for (s = strtok(s, " "); s; s = strtok(NULL, " ")) {
        printf ("s:%s\n", s);
    }

    printf ("2:s: %s\n", m);

    free (m);
    return 0;
}
