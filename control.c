#include <stdint.h>
#include <stdio.h>

#include "control.h"
#include "SDL.h"

static uint8_t *keys;

static uint8_t check_event(void) {
    if      (keys[SDL_SCANCODE_KP_0]) { return 0; }
    else if (keys[SDL_SCANCODE_KP_1]) { return 1; }
    else if (keys[SDL_SCANCODE_KP_2]) { return 2; }
    else if (keys[SDL_SCANCODE_KP_3]) { return 3; }
    else if (keys[SDL_SCANCODE_KP_4]) { return 4; }
    else if (keys[SDL_SCANCODE_KP_5]) { return 5; }
    else if (keys[SDL_SCANCODE_KP_6]) { return 6; }
    else if (keys[SDL_SCANCODE_KP_7]) { return 7; }
    else if (keys[SDL_SCANCODE_KP_8]) { return 8; }
    else if (keys[SDL_SCANCODE_KP_9]) { return 9; }
    else if (keys[SDL_SCANCODE_A])    { return 10; }
    else if (keys[SDL_SCANCODE_B])    { return 11; }
    else if (keys[SDL_SCANCODE_C])    { return 12; }
    else if (keys[SDL_SCANCODE_D])    { return 13; }
    else if (keys[SDL_SCANCODE_E])    { return 14; }
    else if (keys[SDL_SCANCODE_F])    { return 15; }
    else                              { return -1; }
}

// Check new inputs
void process_input(void) {
    int i;
    if (keys == NULL) {
        keys = SDL_GetKeyboardState(NULL);
    }
    SDL_PumpEvents();
}

// Wait a keyboard event and return the pressed key
// TODO The -1 code may be problematic
uint8_t waitKeyboard(void) {
    uint8_t pressed_key = -1;

    while (pressed_key == -1) {
        pressed_key = check_event();
    }
    return pressed_key;
}

uint8_t isKeyPressed(uint8_t) {
    return check_event();
}
