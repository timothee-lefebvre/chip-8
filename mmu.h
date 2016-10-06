#ifndef __MMU_H__
#define __MMU_H__

#include <stdint.h>

#define MMU_MEMORY_SIZE 0x1000

typedef uint8_t mmu_t[MMU_MEMORY_SIZE];

void init_memory(mmu_t memory);
uint8_t read_memory(mmu_t memory, uint16_t addr);
void write_memory(mmu_t memory, uint8_t value, uint16_t addr);

#endif
