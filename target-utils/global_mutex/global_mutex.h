#include <pthread.h>

#define DEFAULT_GLOBAL_MUTEX_NAME "/global_mutex"

pthread_mutex_t *get_global_mutex(char *name);