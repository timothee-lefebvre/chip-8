#ifndef __LCD_H__
#define __LCD_H__

#include <stdint.h>

#include "ppu.h"
#include "SDL.h"

#define LCD_W 640
#define LCD_H 320
#define LCD_WHITE 0x00FFFFFF
#define LCD_BLACK 0x00000000

struct lcd_t {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect rect;
};

void lcd_init(struct lcd_t *lcd);
void lcd_refresh(struct lcd_t *lcd, screen_t screen);

#endif
