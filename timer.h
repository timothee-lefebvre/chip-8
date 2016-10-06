#ifndef __TIMER_H__
#define __TIMER_H__

#define TIMER_DELAY_FREQUENCY 60
#define TIMER_DELAY_CYCLES (CPU_FREQUENCY / 60)

void update_timers(struct cpu_t *cpu);

#endif
