## Disable L1 prefetch

# Process

The imx6 is a Cortex A9 processor. According to the reference manual  (http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0388i/CIHCHFCG.html), the coherency can be enabled/disabled with register ACTLR

- Bit [2]: L1 prefetch enable
- Bit [1]: L2 prefetch hint enable. According to Cortex A9 reference manual, "When enabled, this feature enables the Cortex-A9 processor to automatically issue L2 prefetch hint requests when it detects regular fetch patterns on a coherent memory. This feature is only triggered in a Cortex-A9 MPCore processor, and not in a uniprocessor."

According to the Cortex A9 reference manual, L2 prefetch hints are also automatically generated by the Preload Engine (not present in the imx6)

The register ACTLR can "only accessible in privileged modes"  

In a multi-core configuration, it is important to modify the registers of each core. For this, the `on_each_cpu` function can be used.