#include "global_mutex.h"

#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/mman.h>           /* shm_open */
#include <fcntl.h>              /* For O_* constants */
#include <sys/stat.h>           /* For mode constants */
#include <errno.h>              /* errno returned by shm_open */
#include <string.h>             /* strerror strcpy */
#include <unistd.h>             /* close */
#include <stdlib.h>             /* malloc */

int init_global_mutex(char *name);
int destroy_global_mutex(char *name);
int check_global_mutex(char *name);

/*
 * Global Mutex
 */
int main(int argc, char *argv[])
{
    int iflag = 0;
    int dflag = 0;
    int cflag = 0;
    char *nvalue = DEFAULT_GLOBAL_MUTEX_NAME;
    int c;

    if (argc <= 1) {
        fprintf(stderr, "Usage: %s [-i] [-d] [-c] [-n name]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((c = getopt (argc, argv, "idcn:")) != -1) {
        switch (c) {
            case 'i':
                iflag = 1;
                break;
            case 'd':
                dflag = 1;
                break;
            case 'c':
                cflag = 1;
                break;
            case 'n':
                nvalue = optarg;
                if (nvalue[0] != '/') {
                    fprintf(stderr, "Global mutex name must start with '/'\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-i] [-d] [-c] [-n name]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    /* Handle global mutex initialization */
    
    if (iflag) {
        return init_global_mutex(nvalue); 
    }

    /* Handle global mutex destruction */

    if (dflag) {
        return destroy_global_mutex(nvalue);
    }

    /* Check whether a global mutex exists */

    if (cflag) {
        if (check_global_mutex(nvalue) == 0) {
            printf("Global mutex %s exists\n", nvalue);
        } else {
            printf("Global mutex %s does not exist\n", nvalue);
        }
    }

    return 0;

}

/* 
 * Global Mutex Initialization
 */
int init_global_mutex(char *name) {

    int fd, ret;
    pthread_mutex_t *mutex = NULL;
    pthread_mutexattr_t attr;

    printf("Creating global mutex %s\n", name);

    /* Initialize shared memory object that stores the mutex */

    fd = shm_open(name, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (fd < 0) {
        perror("shm_open");         // It fails if object with the same name already exists
        return EXIT_FAILURE;
    }

    /* Size shared memory object appropiately to store a pthread_mutex */

    ret = ftruncate(fd, sizeof(pthread_mutex_t)); 
    if (ret < 0) {
        perror("ftruncate");
        return EXIT_FAILURE;
    }

    /* Map pthread to shared memory object, so it can be initialized in the shared memory area */

    mutex = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (mutex == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    /* File descriptor is no longer needed, it can be closed */

    ret = close(fd);
    if (ret < 0) {
        perror("close");
        return EXIT_FAILURE;
    }

    /* init pthread mutex with PTHREAD_PROCESS_SHARED attribute*/

    ret = pthread_mutexattr_init(&attr);
    if (ret < 0) {
        perror("pthread_mutexattr_init");
        return EXIT_FAILURE;
    }

    ret = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    if (ret < 0) {
        perror("pthread_mutexattr_getpshared");
        return EXIT_FAILURE;
    }

    ret = pthread_mutex_init(mutex, &attr);
    if (ret < 0) {
        perror("pthread_mutex_init");
        return EXIT_FAILURE;
    }

    ret = pthread_mutexattr_destroy(&attr);
    if (ret < 0) {
        perror("pthread_attr_destroy");
        return EXIT_FAILURE;
    }

    /* Once the mutex has been initialized, this process no longer needs the mapping */

    ret = munmap((void *) mutex, sizeof(pthread_mutex_t));
    if (ret < 0) {
        perror("munmap");
        return EXIT_FAILURE;
    }

    return 0;

}

/*
 * Global Mutex Destruction
 */
int destroy_global_mutex(char *name) {

    int fd, ret;
    int err = 0;
    pthread_mutex_t *mutex = NULL;

    printf("Destroying global mutex %s\n", name);

    /* Open shared memory object that stores the mutex as read only*/

    fd = shm_open(name, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (fd < 0) {
        perror("shm_open");     
        return EXIT_FAILURE;
    }

    /* Map pthread to shared memory object, where a pthread_mutex is already stored */

    mutex = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (mutex == MAP_FAILED) {
        perror("mmap");
        err = EXIT_FAILURE;
    }

    /* File descriptor is no longer needed, it can be closed */

    ret = close(fd);
    if (ret < 0) {
        perror("close");
        err = EXIT_FAILURE;
    }

    /* Destroy pthread mutex */

    ret = pthread_mutex_destroy(mutex);
    if (ret < 0) {
        perror("pthread_mutex_destroy");
        err = EXIT_FAILURE;
    }

    /* Unmap pthread_mutex  */
    
    ret = munmap((void *) mutex, sizeof(pthread_mutex_t));
    if (ret < 0) {
        perror("munmap");
        err = EXIT_FAILURE;
    }

    /* Unlink shared memory object */

    ret = shm_unlink(name);
    if (ret < 0) {
        perror("shm_unlink");
        err = EXIT_FAILURE;
    }

    return err;

}

/* 
 * Check whether a Global Mutex exists
 */
int check_global_mutex(char *name) {
    int fd, ret;

    /* Open shared memory object that stores the mutex as read only*/

    fd = shm_open(name, O_RDONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (fd < 0) {
        perror("shm_open");     
        return EXIT_FAILURE;
    }

    ret = close(fd);
    if (ret < 0) {
        perror("close");
        return EXIT_FAILURE;
    }

    return 0;

}
