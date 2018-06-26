# Shared Global Mutex

A mutex that can be used between different independent processes.

## Compilation

```bash
make
```

By default make uses the compiler named `arm-buildroot-linux-uclibcgnueabihf-gcc`. For a different compiler:

```bash
make CC=<compiler>
```

## Creation and destruction of a Global Shared Mutex

The global shared mutex is created with the command `gmutex`. It creates a global shared mutex with a name. Any process
can access the global shared mutex providing the name.

Syntax:

```bash
gmutex [-i] [-d] [-c] [-n name]

    -i
        Initializes (creates) a new global shared mutex with name 'name'. If 'name' is nor provided with [-n name], a
        default name is used.

    -d
        Destroys global shared mutex 'name'. If 'name' is nor provided with [-n name], a default name is used.

    -c
        Check whether global shared mutex 'name' exists. If 'name' is nor provided with [-n name], a default name is 
        used.

    -n name
        Name of the global shared mutex targetted by the command 
```

## Using a Global Shared Mutex

To use a global shared mutex, it must have been created previosuly with command `gmutex` (see previous section).

The API to retrieve the global shared mutex is defined in file `global_mutex.h`. 

```c
pthread_mutex_t *get_global_mutex(char *name);
```

If `name` is `NULL`, the default global shared mutex is used.

An example of usage can be seen in `first.c` and `second.c`. 