#include <linux/module.h>
#include <linux/kernel.h>

// MRC for reads
// MCR for writes

/*
 * From arch/arm/mach-imx/hotplug.c
 * Freezes the board too, not used anymore
 */
static inline void cpu_enter_lowpower(void)
{
    #define CR_C	(1 << 2)	/* Dcache enable			*/

	unsigned int v;

	asm volatile(
		"mcr	p15, 0, %1, c7, c5, 0\n"
	"	mcr	p15, 0, %1, c7, c10, 4\n"
	/*
	 * Turn off coherency
	 */
	"	mrc	p15, 0, %0, c1, c0, 1\n"
	"	bic	%0, %0, %3\n"
	"	mcr	p15, 0, %0, c1, c0, 1\n"
	"	mrc	p15, 0, %0, c1, c0, 0\n"
	"	bic	%0, %0, %2\n"
	"	mcr	p15, 0, %0, c1, c0, 0\n"
	  : "=&r" (v)
	  : "r" (0), "Ir" (CR_C), "Ir" (0x40)
	  : "cc");
}

static void disable(void* data) {

    // cpu_enter_lowpower();

    u32 val;

    printk(KERN_INFO "Disabling L1 cache coherency\n");
    
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

    /* Write 0 in SMP bit (bit 6) */
    val &= ~(1<<6);
    asm volatile("mcr p15, 0, %0, c1, c0, 1\t\n" :: "r"(val));

    /* Read ACTLR register */
    asm volatile("mrc p15, 0, %0, c1, c0, 1\t\n" : "=r"(val));
    printk(KERN_INFO "ACTLR = 0x%x (after)\n", val);

}

static void enable(void* data) {
    
    u32 val;

    printk(KERN_INFO "Re-enabling L1 cache coherency\n");
    
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

    /* Write 1 in SMP bit (bit 6) */
    val &= (1<<6);
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

MODULE_DESCRIPTION("Module to disable L1 cache coherency in a Cortex A9 processor");
MODULE_LICENSE("GPL");