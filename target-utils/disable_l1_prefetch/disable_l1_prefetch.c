#include <linux/module.h>
#include <linux/kernel.h>

// MRC for reads
// MCR for writes

static void disable(void* data) {

    u32 val;

    printk(KERN_INFO "Disabling L1 cache prefetch\n");
    
    /* Read ACTLR register */
    asm volatile("mrc p15, 0, %0, c1, c0, 1\t\n" : "=r"(val));
    printk(KERN_INFO "ACTLR = 0x%x (before)\n", val);

    /* 
    By default, ACTLR=0x4f for each core
    0x4f =  0b1001111
              |  ||||
              |  |||cache and TLB maintenance
              |  ||L2 prefetch hint enable  
              |  |L1 prefetch enable
              |  write full lines of zero mode
              SMP : coherency
    */  

    /* Write 00 in L1 prefetch and L2 prefetch hint */
    val &= ~(0b11<<1);
    asm volatile("mcr p15, 0, %0, c1, c0, 1\t\n" :: "r"(val));

    /* Read ACTLR register */
    asm volatile("mrc p15, 0, %0, c1, c0, 1\t\n" : "=r"(val));
    printk(KERN_INFO "ACTLR = 0x%x (after)\n", val);

}

static void enable(void* data) {
    
    u32 val;

    printk(KERN_INFO "Re-enabling L1 cache prefetch\n");
    
    /* Read ACTLR register */
    asm volatile("mrc p15, 0, %0, c1, c0, 1\t\n" : "=r"(val));
    printk(KERN_INFO "ACTLR = 0x%x (before)\n", val);

    /* 
    By default, ACTLR=0x4f for each core
    0x4f =  0b1001111
              |  ||||
              |  |||cache and TLB maintenance
              |  ||L2 prefetch hint enable  
              |  |L1 prefetch enable
              |  write full lines of zero mode
              SMP : coherency
    */  

    /* Write 11 in L1 prefetch and L2 prefetch hint */
    val &= (0b11<<1);
    asm volatile("mcr p15, 0, %0, c1, c0, 1\t\n" :: "r"(val));

    /* Read ACTLR register */
    asm volatile("mrc p15, 0, %0, c1, c0, 1\t\n" : "=r"(val));
    printk(KERN_INFO "ACTLR = 0x%x (after)\n", val);

}

int init_module() {
    on_each_cpu(disable, NULL, 1);
    return 0;
}

void cleanup_module() {
    on_each_cpu(enable, NULL, 1);
}

MODULE_DESCRIPTION("Module to disable L1 cache prefetch in a Cortex A9 processor");
MODULE_LICENSE("GPL");