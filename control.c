#include <stdint.h>
#include <stdio.h>

#include "control.h"
#include "SDL.h"

static uint8_t check_event(SDL_Event *event) {
    switch (event->type) {
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_KP_0:
                    return 0;
                    break;
                case SDLK_KP_1:
                    return 1;
                    break;
                case SDLK_KP_2:
                    return 2;
                    break;
                case SDLK_KP_3:
                    return 3;
                    break;
                case SDLK_KP_4:
                    return 4;
                    break;
                case SDLK_KP_5:
                    return 5;
                    break;
                case SDLK_KP_6:
                    return 6;
                    break;
                case SDLK_KP_7:
                    return 7;
                    break;
                case SDLK_KP_8:
                    return 8;
                    break;
                case SDLK_KP_9:
                    return 9;
                    break;
                case SDLK_a:
                    return 10;
                    break;
                case SDLK_b:
                    return 11;
                    break;
                case SDLK_c:
                    return 12;
                    break;
                case SDLK_d:
                    return 13;
                    break;
                case SDLK_e:
                    return 14;
                    break;
                case SDLK_f:
                    return 15;
                    break;
                default:
                    return -1;
                    break;
            }
        default:
            return -1;
            break;
    }
}

// Wait a keyboard event and return the pressed key
// TODO The -1 code may be problematic
uint8_t waitKeyboard(void) {
    SDL_Event event;
    uint8_t pressed_key = -1;

    do {
        SDL_WaitEvent(&event);
        pressed_key = check_event(&event);
    } while (pressed_key == -1);
    fprintf(stderr, "wait pressed key : %x\n", pressed_key);

    return pressed_key;
}

uint8_t checkKeyboard(void) {
    SDL_Event event;
    uint8_t pressed_key = -1;

    if (SDL_PollEvent(&event)) {
        pressed_key = check_event(&event);
        fprintf(stderr, "check pressed key : %x\n", pressed_key);
    }

    return pressed_key;
}
