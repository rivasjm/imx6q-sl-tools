
#include <pthread.h>

#define GLOBAL_MUTEX_NAME "/gmutex"

typedef struct global_mutex_t {
    pthread_mutex_t *pthread_mutex;     /* Normal pthread mutex, that will be stored in the shared memory object */
    int fd;                             /* File descriptor of shared memory object */
    char *name;                         /* Name of shared memory object. Needed for shm_unlink*/
} global_mutex_t;

int init_global_mutex(global_mutex_t *gmutex, char *name);

int destroy_global_mutex(global_mutex_t *gmutex);

void print_global_mutex(global_mutex_t *gmutex);

pthread_mutex_t *get_pthread_mutex_t(global_mutex_t *gmutex);