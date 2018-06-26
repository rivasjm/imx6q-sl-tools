
#include "global_mutex.h"

#include <stdio.h>

int main(int argc, char const *argv[])
{
    int ret;
    global_mutex_t global_mutex;
    pthread_mutex_t *mutex;

    ret = init_global_mutex(&global_mutex);

    print_global_mutex(&global_mutex);

    mutex = get_pthread_mutex_t(&global_mutex);

    ret = destroy_global_mutex(&global_mutex);


    return 0;
}