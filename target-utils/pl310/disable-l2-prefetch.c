#include <linux/module.h>
#include <linux/io.h>                       /* Needed for ioremap */
#include <asm/hardware/cache-l2x0.h>        /* PL310 register offsets */

// MRC for reads
// MCR for writes

#define IMX6_PL310_BASE 0x00A02000

static void __iomem *l2_base;

/*
 * Module initialization
 */
static int __init disable_prefetch_init(void)
{
    u32 aux_l2;

    l2_base = ioremap(IMX6_PL310_BASE, SZ_4K);
    
    /* Initial status of L2 prefetch */
    aux_l2 = readl(l2_base + L2X0_AUX_CTRL);
    printk(KERN_INFO "(before) L2 I Prefetch=%d, L2 D Prefetch=%d\n", (aux_l2>>29)&1, (aux_l2>>28)&1);

    /* Disable L2 is needed before auxiliary register can be modified to disable prefetch */
    writel_relaxed(0, l2_base + L2X0_CTRL);

    /* Disable prefetch bits in L2 configuration*/
    aux_l2 &= ~(1<<29);   // I prefetch
    aux_l2 &= ~(1<<28);   // D prefetch
    writel_relaxed(aux_l2, l2_base + L2X0_AUX_CTRL);

    /* Re-enable L2 cache controller */
    writel_relaxed(1, l2_base + L2X0_CTRL);

    /* Check prefetch status*/
    aux_l2 = readl(l2_base + L2X0_AUX_CTRL);
    printk(KERN_INFO "(after) L2 I Prefetch=%d, L2 D Prefetch=%d\n", (aux_l2>>29)&1, (aux_l2>>28)&1);

    return 0;
}

/*
 * Module exit
 */ 
static void __exit disable_prefetch_exit(void)
{  
    u32 aux_l2;

    printk(KERN_INFO "Exiting Disable Prefetch module\n");

    /* Disable L2 is needed before auxiliary register can be modified to disable prefetch */
    writel_relaxed(0, l2_base + L2X0_CTRL);

    /* Re-enable L2 prefetch */
    aux_l2 = readl(l2_base + L2X0_AUX_CTRL);
    aux_l2 |= (1<<29);
    aux_l2 |= (1<<28);
    writel_relaxed(aux_l2, l2_base + L2X0_AUX_CTRL);

    /* Re-enable L2 cache controller */
    writel_relaxed(1, l2_base + L2X0_CTRL);

    /* Check prefetch status*/
    aux_l2 = readl(l2_base + L2X0_AUX_CTRL);
    printk(KERN_INFO "(after) L2 I Prefetch=%d, L2 D Prefetch=%d\n", (aux_l2>>29)&1, (aux_l2>>28)&1);
}

module_init(disable_prefetch_init);
module_exit(disable_prefetch_exit);

MODULE_DESCRIPTION("Module to disable automatic prefetch in a Cortex-A9 (imx6q)");
MODULE_LICENSE("GPL");