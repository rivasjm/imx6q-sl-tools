## Disable L1 cache coherency

**IMPORTANT**

Installing this module freezes the board, requiring a reboot. Coherency should be disabled at board boot time, inside the kernel (maybe in arch/arm/mach-imx/mach-imx6q.c). I keep this module here for documentation purposes.

# Process

When tasks (threads) are statically allocated to a core, and migration is not allowed, L1 cache coherency is not
necessary. In this case, coherency can even have a negative impact, as additional noise is created between cores.

The imx6 is a Cortex A9 processor. According to the reference manual 
(http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0388i/CIHCHFCG.html), the coherency can be
emabled/disabled with ACTLR.SMP (bit 6 of register ACTLR)

The register ACTLR can "only accessible in privileged modes"  

In a multi-core configuration, it is important to modify the registers of each core. For this, the `on_each_cpu`
function can be used.