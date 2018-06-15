#include <linux/module.h>
#include <linux/kernel.h>

// MRC for reads
// MCR for writes

static void disable(void* data) {

}

static void enable(void* data) {

}

int init_module() {
    on_each_cpu(disable, NULL, 1);
    return 0;
}

void cleanup_module() {
    on_each_cpu(enable, NULL, 1);
}

MODULE_DESCRIPTION("Module to disable L1 cache coherency in a Cortex A9 processor");
MODULE_LICENSE("GPL");