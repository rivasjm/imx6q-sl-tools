#include "global_mutex.h"

#include <stdio.h>
#include <fcntl.h>              /* For O_* constants */
#include <sys/mman.h>           /* shm_open */
#include <unistd.h>             /* close */
#include <sys/stat.h>           /* For mode constants */

pthread_mutex_t *get_global_mutex(char *name) {
    
    char *current_name = DEFAULT_GLOBAL_MUTEX_NAME;
    int fd, ret;
    pthread_mutex_t *mutex = NULL;

    if (name != NULL) {
        current_name = name;
    }

    printf("Retrieving global mutex %s\n", current_name);

    /* Open shared memory object that stores the mutex */

    fd = shm_open(current_name, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (fd < 0) {
        perror("shm_open");     
        return NULL;
    }

    /* Map pthread to shared memory object, where a pthread_mutex is already stored */

    mutex = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (mutex == MAP_FAILED) {
        perror("mmap");
        mutex = NULL;
    }

    /* File descriptor is no longer needed, it can be closed */

    ret = close(fd);
    if (ret < 0) {
        perror("close");
        mutex = NULL;
    }

    return mutex;

}