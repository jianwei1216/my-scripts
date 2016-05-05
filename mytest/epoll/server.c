#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>

int main(int argc, char * argv[])
{
        int epoll_fd = -1; 
        int server_fd = -1;
        struct sockaddr_in server_addr;
        struct sockaddr_in client_addr;
       
        server_fd = socket (AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
                perror ("socket()");
                goto out;
        }

        memset (&server_addr, 0, sizeof(struct sockaddr_in));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(1234);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(server_fd, (const struct sockaddr*)&server_addr,
                 sizeof(server_addr)) < 0) {
                perror ("bind()");
                goto out;
        }

        if (listen(server_fd, 10) < 0) {
                perror ("listen()");
                goto out;
        }

        epoll_fd = epoll_create (11); 
        if (epoll_fd < 0) {
                perror ("epoll_create()");
                goto out;
        }

        

out:
        if (server_fd > 0)
                close (server_fd);
        if (epoll_fd > 0)
                close (epoll_fd);

        return 0;
}
