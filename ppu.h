#ifndef __PPU_H__
#define __PPU_H__

#include <stdint.h>

#define PPU_SCREEN_W 64
#define PPU_SCREEN_H 32
#define PPU_BLACK 0
#define PPU_WHITE 1

typedef uint8_t screen_t[PPU_SCREEN_H][PPU_SCREEN_W];

int draw_sprite(screen_t screen, uint8_t x, uint8_t y, uint8_t h, const uint8_t *pixels);
void clear_screen(screen_t screen);

#endif
