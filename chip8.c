#include "chip8.h"
#include "timer.h"

void init_chip8(struct chip8_t *chip) {
    cpu_init(&chip->cpu);
    init_memory(chip->memory);
    lcd_init(&chip->lcd);
    clear_screen(chip->screen);
}

void update_chip8(struct chip8_t *chip) {
    cpu_update(&chip->cpu, chip->memory, chip->screen);
    lcd_refresh(&chip->lcd, chip->screen);
}
