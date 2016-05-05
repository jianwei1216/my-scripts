#include <sys/epoll.h>
#include <stdio.h>


int main (void)
{
        printf ("EPOLLIN=%d\n", EPOLLIN);
        printf ("EPOLLOUT=%d\n", EPOLLOUT);
        printf ("EPOLLPRI=%d\n", EPOLLPRI);
        printf ("EPOLLERR=%d\n", EPOLLERR);
        printf ("EPOLLHUP=%d\n", EPOLLHUP);

        return 0;
}
