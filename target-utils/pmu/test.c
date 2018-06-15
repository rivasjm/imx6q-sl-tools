#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>

// My system has 6 configurable counters and a separate Cycle Count register.
// This will contain a nice human-readable name for the configured counters.
const char* cpu_name[7] = { "", "", "", "", "", "", "CCNT" };

typedef struct {
  __u32 reg[7];       // 6 configurables and the cycle count
} cpu_perf;

void print_counters(cpu_perf data) {
    printf("Counters overview:\n");
    for (int i=0; i<7; i++) {
        if (cpu_name[i] != "") {
            printf("  %s: %d\n", cpu_name[i], data.reg[i]);
        }
    }
}

__u32 _read_cpu_counter(int r) {
  // Read PMXEVCNTR #r
  // This is done by first writing the counter number to PMSELR and then reading PMXEVCNTR
  __u32 ret;
  asm volatile ("MCR p15, 0, %0, c9, c12, 5\t\n" :: "r"(r));      // (write) Select event counter in PMSELR
  asm volatile ("MRC p15, 0, %0, c9, c13, 2\t\n" : "=r"(ret));    // (read)  Read from PMXEVCNTR
  return ret;
}

void _setup_cpu_counter(__u32 r, __u32 event, const char* name) {
  cpu_name[r] = name;

  // Write PMXEVTYPER #r
  // This is done by first writing the counter number to PMSELR and then writing PMXEVTYPER

  // c9 0 c12 5 -> PMSELR: Performance Monitors Event Counter Selection
  // Last 5 bits selects a performance counter (also called event counter)
  // This selection is next used by PMXEVTYPER or PMXEVCNTR  
  asm volatile ("MCR p15, 0, %0, c9, c12, 5\t\n" :: "r"(r));        // (write) Select event counter in PMSELR

  // c9 0 c13 1 -> PMXEVTYPER: Performance Monitors Event Type Select
  // Selects which event increments the event counter specified in PMSELR
  // Last 8 bits select the event
  // From 0x00 -> 0x3F : Common events
  //      0x40 -> 0xFF : implementation defined events
  asm volatile ("MCR p15, 0, %0, c9, c13, 1\t\n" :: "r"(event));    // (write) Set the event number in PMXEVTYPER
}

void init_cpu_perf() {
  // Disable all counters for configuration (PCMCNTENCLR)
  // c9 0 c12 2 -> PMCNTENCLR: Performance Monitors Count Enable Clear
  // 0b10000000000000000000000000111111
  //   |                         ||||||
  //   |                         write 1 disables those 6 performance counters
  //   write 1 here disables cycle counter
  asm volatile ("MCR p15, 0, %0, c9, c12, 2\t\n" :: "r"(0x8000003f));

  // disable counter overflow interrupts
  // asm volatile ("MCR p15, 0, %0, c9, c14, 2\n\t" :: "r"(0x8000003f));


  // Select which events to count in the 6 configurable counters
  // Note that both of these examples come from the list of required events.
  _setup_cpu_counter(0, 0x04, "L1DACC");
  _setup_cpu_counter(1, 0x03, "L1DFILL");
  
  _setup_cpu_counter(2, 0x06, "ARMV7_PERFCTR_MEM_READ");
  _setup_cpu_counter(3, 0x19, "ARMV7_PERFCTR_BUS_ACCESS");
  _setup_cpu_counter(4, 0x00, "ARMV7_PERFCTR_PMNC_SW_INCR");
  _setup_cpu_counter(5, 0x07, "ARMV7_PERFCTR_MEM_WRITE");


}


void reset_cpu_perf() {
  // Disable all counters (PMCNTENCLR):
  asm volatile ("MCR p15, 0, %0, c9, c12, 2\t\n" :: "r"(0x8000003f));

  __u32 pmcr  = 0x1    // enable counters
            | 0x2    // reset all other counters
            | 0x4    // reset cycle counter
            | 0x8    // enable "by 64" divider for CCNT.
            | 0x10;  // Export events to external monitoring

  // program the performance-counter control-register (PMCR):
  asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(pmcr));

  // clear overflows (PMOVSR):
  asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000003f));

  // Enable all counters (PMCNTENSET):
  asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000003f));

}

cpu_perf get_cpu_perf() {
  cpu_perf ret;
  int r;

  // Read the configurable counters
  for (r=0; r<6; ++r) {
    ret.reg[r] = _read_cpu_counter(r);
  }

  // Read CPU cycle count from the CCNT Register
  asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(ret.reg[6]));

  return ret;
}

int main() {
  init_cpu_perf();

  // Here's what a test looks like:
  reset_cpu_perf();
  /*
   * ... Perform your operations
   */
  cpu_perf results_1 = get_cpu_perf();

    print_counters(results_1);

    int data[1500000] = {0};
    for (int i=0; i<1500000; i++) {
        data[i] += rand();
    }

    cpu_perf results_2 = get_cpu_perf();
    print_counters(results_2);

}