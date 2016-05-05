#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define SOFT (1024*1024*1024ULL)
#define HARD (SOFT*2)

int main(void)
{
    off_t num = (1024*1024*1024*2ULL);

    if (HARD == num) {
        printf ("%ld==%ld\n", num, HARD);
    }

    printf ("%ld\n", SOFT);
    printf ("%ld\n", HARD);
    printf ("sizeof(SOFT):%d\n", sizeof(SOFT));
    printf ("sizeof(int):%d\n", sizeof(int));
    printf ("sizeof(off_t):%d\n", sizeof(off_t));
    
    return 0;
}
