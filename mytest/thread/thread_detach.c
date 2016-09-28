#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

void *say_hello (void *data)
{
        int *i = data;
        while (1) {
                printf ("pthread_id=%p, i=%d\n", pthread_self(), *i);
                sleep (1);
        }
out:
        return NULL;
}

int main(void)
{
        int ret = 0;
        int i = 0;
        pthread_t thread;
        int arr[10] = {0,};

        for (i = 0; i < 10; i++) {
                arr[i] = i;
                ret = pthread_create (&thread, NULL, say_hello, (void*)(&arr[i]));
                if (ret != 0) {
                        printf ("pthread_create() %d failed\n", i);
                        break;
                } else {
                        ret = pthread_detach (thread);
                        if (ret != 0) {
                                printf ("pthread_detach() %d failed\n", i);
                                break;
                        }
                }
        }

        sleep (1000000);
out:
        return ret;
}
