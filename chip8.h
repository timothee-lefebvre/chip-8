#ifndef __CHIP8__
#define __CHIP8__

#include "cpu.h"
#include "lcd.h"
#include "mmu.h"
#include "ppu.h"

struct chip8_t {
    struct cpu_t cpu;
    screen_t screen;
    mmu_t memory;
    struct lcd_t lcd;
};

void init_chip8(struct chip8_t *chip);
void update_chip8(struct chip8_t *chip);

#endif
