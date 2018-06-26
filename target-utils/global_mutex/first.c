#include "global_mutex.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>             /* sleep */


int main(int argc, char *argv[])
{
    pthread_mutex_t *mutex;

    mutex = get_global_mutex(NULL);

    if (mutex == NULL) {
        printf("Process First: global mutex could not be retrieved\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(mutex);

    printf("Process First: Inside Critical Section\n");
    sleep(10);

    pthread_mutex_unlock(mutex);

    printf("Process First: Unlocked\n");

    return 0;
}
