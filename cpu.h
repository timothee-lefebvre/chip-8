#ifndef __CPU_H__
#define __CPU_H__

#include "mmu.h"
#include "ppu.h"

#define CPU_FLAGS_REG_IDX 15
#define CPU_FREQUENCY 840
#define CPU_NB_GEN_REGS 16
#define CPU_STACK_SIZE 16

typedef uint8_t reg8_t;
typedef uint16_t reg16_t;
typedef uint16_t stack_t[CPU_STACK_SIZE];

struct cpu_t {
    // regs[15] is reserved for flags
    reg8_t g_regs[CPU_NB_GEN_REGS];
    reg16_t I;
    reg8_t sound_tim, delay_tim;
    reg16_t pc, sp;
    stack_t stack;

    uint16_t cycles;
};

void cpu_init(struct cpu_t *cpu);
void cpu_update(struct cpu_t *cpu, mmu_t memory, screen_t screen);
void cpu_execute(struct cpu_t *cpu, mmu_t memory, screen_t screen);

#endif
