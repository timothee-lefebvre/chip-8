#include "chip8.h"
#include "rom.h"

int main(int argc, char* argv[]) {
    struct chip8_t chip;

    init_chip8(&chip);
    load_rom(argv[1], chip.memory);
    for (;;) {
        update_chip8(&chip);
    }

    return 0;
}
