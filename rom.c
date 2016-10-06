#include <stdio.h>

#include "rom.h"

int load_rom(const char* rom, mmu_t memory) {
    FILE* file = NULL;

    if ((file = fopen(rom, "r")) == NULL) {
        perror("Error while loading rom");
        return -1;
    }
    fread(memory + 0x200, MMU_MEMORY_SIZE, 1, file);

    fclose(file);
}
