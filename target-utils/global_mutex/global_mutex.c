#include "global_mutex.h"

#include <stdio.h>
#include <sys/mman.h>           /* shm_open */
#include <fcntl.h>              /* For O_* constants */
#include <sys/stat.h>           /* For mode constants */
#include <errno.h>              /* errno returned by shm_open */
#include <string.h>             /* strerror strcpy */
#include <unistd.h>             /* close */
#include <stdlib.h>             /* malloc */

int init_global_mutex(global_mutex_t *gmutex, char *name) {

    int ret;
    pthread_mutex_t *pmutex;

    /* set global mutex name */

    gmutex->name = (char *) malloc(strlen(name));
    strcpy(gmutex->name, name);
    
    /* 
     * Try to access shared memory object
     * With O_CREAT|O_EXCL, shm_open returns error if shared object already exists
     *      return -1 and errno=EEXIST 
     */
    gmutex->fd = shm_open(name, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (gmutex->fd < 0) {
        if (errno == EEXIST) {
            printf("Shared memory object already exists\n");
        }
    }

    /* size shared memory object appropiately to store a pthread_mutex */

    // printf("gmutex->fd=%d\n", gmutex->fd);
    // printf("size of pthread_mutex_t=%d\n", sizeof(pthread_mutex_t));

    ret = ftruncate(gmutex->fd, sizeof(pthread_mutex_t)); 
    if (ret < 0) {
        perror("ftruncate");
        return -1;
    }

    /* map pthread to shared memory object */

    pmutex = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED, gmutex->fd, 0);
    if (pmutex == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    /* init pthread mutex with PTHREAD_PROCESS_SHARED attribute*/

    pthread_mutexattr_t attr;
    ret = pthread_mutexattr_init(&attr);
    if (ret < 0) {
        perror("pthread_mutexattr_init");
        return -1;
    }

    ret = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    if (ret < 0) {
        perror("pthread_mutexattr_getpshared");
        return -1;
    }

    ret = pthread_mutex_init(pmutex, &attr);
    if (ret < 0) {
        perror("pthread_mutex_init");
        return -1;
    }
    gmutex->pthread_mutex = pmutex;

    ret = pthread_mutexattr_destroy(&attr);
    if (ret < 0) {
        perror("pthread_attr_destroy");
        return -1;
    }

    return 0;

}

int destroy_global_mutex(global_mutex_t *gmutex) {

    int ret, err;

    err = 0;

    /* Destroy pthread mutex */

    ret = pthread_mutex_destroy(gmutex->pthread_mutex);
    if (ret < 0) {
        perror("pthread_mutex_destroy");
        err = -1;
    }

    /* Unmap pthread_mutex  */
    
    ret = munmap((void *) gmutex->pthread_mutex, sizeof(pthread_mutex_t));
    if (ret < 0) {
        perror("munmap");
        err = -1;
    }

    /* Close shm_open file */
    
    close(gmutex->fd);

    /* Unlink shared memory object */
    
    ret = shm_unlink(gmutex->name);
    if (ret < 0) {
        perror("shm_unlink");
        err = -1;
    }

    /* Clean gmutex contents  */

    gmutex->fd = 0;
    gmutex->pthread_mutex = NULL;
    free(gmutex->name);

    return err;
}

void print_global_mutex(global_mutex_t *gmutex) {
    printf("Global mutex [name=%s ptr=%p fd=%d]\n", gmutex->name, gmutex->pthread_mutex, gmutex->fd);
}

pthread_mutex_t *get_pthread_mutex_t(global_mutex_t *gmutex) {
    return gmutex->pthread_mutex;
}