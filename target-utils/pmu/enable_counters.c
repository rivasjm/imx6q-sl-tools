#include <linux/module.h>
#include <linux/kernel.h>

// MRC for reads
// MCR for writes

static void pmu_user_enable(void* data)
{
    /* 
       Enable user-mode access to the performance counter 
       c9 0 c14 0 -> PMUSERENR: Performance Monitors User Enable Register
       Last bit: 0-> user mode disabled, 1->user mode enabled
       00000000000000000000000000000001
                                      |
                                      PMUSERENR.EN=1 -> write 1 enables user mode access to PMU's
    */
    asm ("mcr p15, 0, %0, C9, C14, 0\n\t" :: "r"(1));  

    /* 
       Disable counter overflow interrupts (just in case) 
       c9 0 c14 2 -> PMINTENCLR: Performance Monitors Interrupt Enable Clear register
       0x8000000f -> 10000000000000000000000000001111
                     |                           ||||
                     |                           disables overflow interrupts for these 4 performance counters
                     disable overflow interrupt for cycle counter                                             
    */
    asm ("mcr p15, 0, %0, C9, C14, 2\n\t" :: "r"(0x8000000f));
     
    /*
       Enable debug mode access. This is needed to avoid reading zero in the counters
       c1 0 c1 1 -> SDER: Secure Debug Enable
       0b11 -> 00000000000000000000000000000011
                                             ||
                                             | write 1 -> Invasive debug permitted in Secure User mode.
                                             |
                                             write 1 -> Non-invasive debug permitted in Secure User mode.
    */
	asm volatile("mcr p15, 0, %0, c1, c1, 1" : : "r" (0b11));

    printk(KERN_INFO "Performance counters user mode access enabled\n");
}

static void pmu_user_disable(void* data) 
{
    /* Disable debug mode access */

    asm volatile("mcr p15, 0, %0, c1, c1, 1" : : "r" (0b00));

    /* Write PMUSERENR.EN = 0  */

    asm ("mcr p15, 0, %0, C9, C14, 0\n\t" :: "r"(0)); 

    printk(KERN_INFO "Performance counters user mode access disabled\n");
}

int init_module() 
{
    on_each_cpu(pmu_user_enable, NULL, 1);
    return 0;
} 

void cleanup_module() 
{
    on_each_cpu(pmu_user_disable, NULL, 1);
} 

MODULE_DESCRIPTION("Module for enabling performance counter on ARMv7");
MODULE_LICENSE("GPL");