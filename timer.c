#include <stdio.h>

#include "cpu.h"

#define TIMER_DELAY_FREQUENCY 60
#define TIMER_DELAY_CYCLES (CPU_FREQUENCY / 60)

void update_timers(struct cpu_t *cpu) {
    static uint8_t delay_cycles_count = 0;
    
    if (cpu->delay_tim > 0) {
        /*++delay_cycles_count;
        if (delay_cycles_count >= TIMER_DELAY_CYCLES) {
            delay_cycles_count -= TIMER_DELAY_CYCLES;
            cpu->delay_tim--;
        }*/
        cpu->delay_tim--;
    }
}
