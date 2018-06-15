# Disable L2 cache auto-prefetch

## Process

The imx6 processor includes the PL310 (L2C-210) L2 cache controller. To disable the L2 cache auto-prefetch:

1. Disable L2 cache: write 0 in `reg1_control[0]`
2. Disable Instructions prefetch: write 0 in `reg1_aux_control[29]`
3. Disable Data prefetch: write 0 in `reg1_aux_control[28]`
4. Re-enable L2 cache: write 1 in `reg1_control[0]`

When writing to `reg1_aux_control` it is important that the reserved bits [9:1] are preserved