#define _GNU_SOURCE

#include "global_mutex.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>             /* sleep */

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{
    int ret;

    pthread_mutex_t *mutex;
    pthread_t thread;
    cpu_set_t cpuset;

    /* DEBUG: get number of cores available */

    // int numberOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);
    // printf("Number of processors: %d\n", numberOfProcessors);

    /* Set affinity of this task */
    
    thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);

    ret = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (ret != 0) {
        handle_error_en(ret, "pthread_setaffinity_np");
    }

    /* Retrieve global mutex */ 

    mutex = get_global_mutex(NULL);

    if (mutex == NULL) {
        printf("Process Second: global mutex could not be retrieved\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(mutex);

    printf("Process First: Inside Critical Section [core=%d]\n", \
        sched_getcpu());

    pthread_mutex_unlock(mutex);

    printf("Process First: Unlocked\n");

    return 0;
}
