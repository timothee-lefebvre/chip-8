#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"
#include "ppu.h"
#include "SDL.h"

void lcd_init(struct lcd_t *lcd) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
        fprintf(stderr, "SDL init failed : %s\n", SDL_GetError());
        exit(-1);
    }

    if (SDL_CreateWindowAndRenderer(LCD_W, LCD_H, 0, &lcd->window, &lcd->renderer)) {
        fprintf(stderr, "SDL init failed : %s\n", SDL_GetError());
        exit(-1);
    }

    lcd->texture = SDL_CreateTexture(lcd->renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, LCD_W, LCD_H);
    if (lcd->texture == NULL) {
        fprintf(stderr, "SDL init failed : %s\n", SDL_GetError());
        exit(-1);
    }

    lcd->rect.x = 0;
    lcd->rect.y = 0;
    lcd->rect.w = LCD_W;
    lcd->rect.h = LCD_H;

    atexit(SDL_Quit);
}

void lcd_refresh(struct lcd_t *lcd, screen_t screen) {
    Uint32 *pixels;
    int pitch, i, j;

    if (SDL_LockTexture(lcd->texture, &lcd->rect, (void**) &pixels, &pitch)) {
        fprintf(stderr, "LCD refresh failed : %s\n", SDL_GetError());
        return;
    }

    for (i = 0; i < lcd->rect.h; ++i) {
        for (j = 0; j < pitch / sizeof(Uint32); ++j) {
            pixels[i * pitch / sizeof(Uint32) + j] = screen[i/(LCD_H/PPU_SCREEN_H)][j/(LCD_W/PPU_SCREEN_W)] ? LCD_WHITE : LCD_BLACK;
        }
    }
    
    SDL_UnlockTexture(lcd->texture);
    SDL_RenderCopy(lcd->renderer, lcd->texture, NULL, NULL);
    SDL_RenderPresent(lcd->renderer);
}
