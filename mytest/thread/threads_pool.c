#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>

typedef struct {
    pthread_mutex_t     mutex;
    pthread_cond_t      cond;
    int                 max_count;
    int                 min_count;
    int                 curr_count;
} global_thread_t;

global_thread_t *conf;

void *thread_worker (void *data)
{
    if (data == NULL) 
        goto out;

out:
    return NULL;
}

int __threads_scale (global_thread_t *conf)
{
    int ret = -1;
    int scale = 0;
    int diff = 0;

    if (conf == NULL) {
        printf ("conf is NULL!")
        goto out;
    }

    scale = conf->curr_count;
    if (scale < conf->min_count)
        scale = conf->min_count;

    if (scale > conf->max_count)
        scale = conf->max_count;

    if (conf->curr_count < scale)
        diff = scale - conf->curr_count; 

    while (diff) {
        diff --;
        ret = pthread_create ()
    }

out:
    return ret;
}

int threads_scale (global_thread_t *conf)
{
    int ret = -1;

    if (conf == NULL) {
        printf ("conf is NULL!")
        goto out;
    }

    pthread_mutex_lock (&conf->mutex);
    ret = __threads_scale (conf); 
    pthread_mutex_unlock (&conf->mutex);

out:
    return ret;
}

#define THREADS_POOL_MIN_COUNT 1
#define THREADS_POOL_MAX_COUNT 10

int main (int argc, char *argv[])
{
    int ret = -1;
    
    conf = (global_thread_t *)calloc (1, sizeof(*conf));
    if (conf == NULL) {
        printf ("%s", strerror(errno));
        goto out;
    }

    conf->min_count = THREADS_POOL_MIN_COUNT;
    conf->max_count = THREADS_POOL_MAX_COUNT;

    ret = pthread_mutex_init (&conf->mutex, NULL);
    if (ret == -1) {
        printf ("pthread_mutex_init(): %s", strerror(errno));
        goto out;
    }

    ret = pthread_cond_init(&conf->cond, NULL);
    if (ret == -1) {
        printf ("pthread_cond_init(): %s", strerror(errno));
        goto out;
    }

    ret = threads_scale (conf);
    if (ret == -1) {
        printf ("Failed to call threads_scale()")
        goto out;
    }

    /* TODO: pthread_join */
out:
    free (conf);
    return 0;
}
