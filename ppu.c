#include <stdint.h>

#include "ppu.h"

int draw_sprite(screen_t screen, uint8_t x, uint8_t y, uint8_t h, const uint8_t *pixels) {
    int l, pix, unset = 0;

    for (l = 0; l < h && l + y < PPU_SCREEN_H; ++l) {
        for (pix = 0; pix < 8 && x + pix < PPU_SCREEN_W; ++pix) {
            if (screen[y + l][x + pix] && pixels[l] & (0x80 >> pix)) {
                unset = 1;
            }
            screen[y + l][x + pix] ^= (pixels[l] & (0x80 >> pix)) >> (7 - pix);
        }
        /*screen[y + l][x + 0] ^= (pixels[l] & 0x80) ? PPU_WHITE : PPU_BLACK;
        screen[y + l][x + 1] ^= (pixels[l] & 0x40) ? PPU_WHITE : PPU_BLACK;
        screen[y + l][x + 2] ^= (pixels[l] & 0x20) ? PPU_WHITE : PPU_BLACK;
        screen[y + l][x + 3] ^= (pixels[l] & 0x10) ? PPU_WHITE : PPU_BLACK;*/

    }

    return unset;
}

void clear_screen(screen_t screen) {
    int x, y;

    for (y = 0; y < PPU_SCREEN_H; ++y) {
        for (x = 0; x < PPU_SCREEN_W; ++x) {
            screen[y][x] = PPU_BLACK;
        }
    }
}
