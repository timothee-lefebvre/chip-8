#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "control.h"
#include "cpu.h"
#include "mmu.h"
#include "ppu.h"
#include "SDL.h"
#include "timer.h"

static void cpu_push_stack(struct cpu_t *cpu, uint16_t value) {
    cpu->stack[cpu->sp++] = value;
}

static uint16_t cpu_pop_stack(struct cpu_t *cpu) {
    return cpu->stack[--cpu->sp];
}

static void hex_to_bcd(uint8_t hex, uint8_t bcd[3]) {
    bcd[0] = hex / 100;
    hex %= 100;
    bcd[1] = hex / 10;
    bcd[2] = hex % 10;
}

void cpu_init(struct cpu_t *cpu) {
    memset(cpu->g_regs, 0, CPU_NB_GEN_REGS * sizeof(reg8_t));
    cpu->I = 0;
    cpu->sound_tim = 0;
    cpu->delay_tim = 0;
    cpu->pc = 0x200;
    cpu->sp = 0;

    cpu->cycles = 0;
    srand(time(NULL));
}

void cpu_update(struct cpu_t *cpu, mmu_t memory, screen_t screen) {
    static uint16_t start = 0, end;

    cpu_execute(cpu, memory, screen);
    if (++cpu->cycles >= CPU_FREQUENCY / 60) {
        uint16_t ms_diff = 1000 - (end - start);
        cpu->cycles -= CPU_FREQUENCY / 60;
        end = SDL_GetTicks();
        SDL_Delay(ms_diff >= 0 ? ms_diff / 60 : 0);
        start = end;
    }
}

void cpu_execute(struct cpu_t *cpu, mmu_t memory, screen_t screen) {
    uint16_t opcode = (read_memory(memory, cpu->pc) << 8) | read_memory(memory, (cpu->pc + 1));
    cpu->pc += 2;
    fprintf(stdout, "opcode : %4x\npc : %4x\n", opcode, cpu->pc);

    switch (opcode & 0xF000) {
        // Ignore the 0NNN opcode
        case 0x0000:
            if (opcode == 0x00EE) {
                cpu->pc = cpu_pop_stack(cpu);
            }
            else if (opcode == 0x00E0) {
                clear_screen(screen);
            }
            else fprintf(stderr, "UNKNOWN : %4x\n", opcode);
            break;
        // Jump
        case 0x1000:
        {
            uint16_t addr = opcode & 0x0FFF;
            cpu->pc = addr;
            break;
        }
        // Call
        case 0x2000:
        {
            uint16_t addr = opcode & 0x0FFF;
            cpu_push_stack(cpu, cpu->pc);
            cpu->pc = addr;
            break;
        }
        case 0x3000:
        {
            uint8_t reg_idx = (opcode & 0x0F00) >> 8;
            uint8_t value = opcode & 0x00FF;
            if (cpu->g_regs[reg_idx] == value) { cpu->pc += 2; }
            break;
        }
        case 0x4000:
        {
            uint8_t reg_idx = (opcode & 0x0F00) >> 8;
            uint8_t value = opcode & 0x00FF;
            if (cpu->g_regs[reg_idx] != value) { cpu->pc += 2; }
            break;
        }
        case 0x5000:
        {
            uint8_t reg1_idx = (opcode & 0x0F00) >> 8;
            uint8_t reg2_idx = (opcode & 0x00F0) >> 4;
            if (cpu->g_regs[reg1_idx] == cpu->g_regs[reg2_idx]) { cpu->pc += 2; }
            break;
        }
        // Store value in reg
        case 0x6000:
        {
            uint8_t value = opcode & 0x00FF;
            uint8_t reg_idx = (opcode & 0x0F00) >> 8;
            cpu->g_regs[reg_idx] = value;
            break;
        }
        // Add value to reg
        case 0x7000:
        {
            uint8_t value = opcode & 0x00FF;
            uint8_t reg_idx = (opcode & 0x0F00) >> 8;
            cpu->g_regs[reg_idx] += value;
            break;
        }
        case 0x8000:
            switch (opcode & 0x000F) {
                // Store reg in another reg
                case 0x0000:
                {
                    uint8_t reg_src_idx = (opcode & 0x00F0) >> 4;
                    uint8_t reg_dst_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[reg_dst_idx] = cpu->g_regs[reg_src_idx];
                    break;
                }
                // Or
                case 0x0001:
                {
                    uint8_t reg_src_idx = (opcode & 0x00F0) >> 4;
                    uint8_t reg_dst_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[reg_dst_idx] |= cpu->g_regs[reg_src_idx];
                    break;
                }
                // And
                case 0x0002:
                {
                    uint8_t reg_src_idx = (opcode & 0x00F0) >> 4;
                    uint8_t reg_dst_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[reg_dst_idx] &= cpu->g_regs[reg_src_idx];
                    break;
                }
                // Xor
                case 0x0003:
                {
                    uint8_t reg_src_idx = (opcode & 0x00F0) >> 4;
                    uint8_t reg_dst_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[reg_dst_idx] ^= cpu->g_regs[reg_src_idx];
                    break;
                }
                // Add reg in another reg
                case 0x0004:
                {
                    uint8_t reg_src_idx = (opcode & 0x00F0) >> 4;
                    uint8_t reg_dst_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[CPU_FLAGS_REG_IDX] = (cpu->g_regs[reg_dst_idx] + cpu->g_regs[reg_src_idx] > 0xFF) ? 1 : 0;
                    cpu->g_regs[reg_dst_idx] += cpu->g_regs[reg_src_idx];
                    break;
                }
                // Sub reg from another reg
                case 0x0005:
                {
                    uint8_t reg_src_idx = (opcode & 0x00F0) >> 4;
                    uint8_t reg_dst_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[CPU_FLAGS_REG_IDX] = (cpu->g_regs[reg_src_idx] > cpu->g_regs[reg_dst_idx]) ? 0 : 1;
                    cpu->g_regs[reg_dst_idx] -= cpu->g_regs[reg_src_idx];
                    break;
                }
                // Shift a reg to the right
                case 0x0006:
                {
                    uint8_t reg_src_idx = (opcode & 0x00F0) >> 4;
                    uint8_t reg_dst_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[CPU_FLAGS_REG_IDX] = cpu->g_regs[reg_src_idx] & 0x01;
                    cpu->g_regs[reg_dst_idx] = cpu->g_regs[reg_src_idx] >> 1;
                    //cpu->g_regs[reg_dst_idx] >>= 1;
                    break;
                }
                case 0x0007:
                {
                    uint8_t reg_src_idx = (opcode & 0x00F0) >> 4;
                    uint8_t reg_dst_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[CPU_FLAGS_REG_IDX] = (cpu->g_regs[reg_dst_idx] > cpu->g_regs[reg_src_idx]) ? 0 : 1;
                    cpu->g_regs[reg_dst_idx] = cpu->g_regs[reg_src_idx] - cpu->g_regs[reg_dst_idx];
                    break;
                }
                // Shift a reg to the left
                case 0x000E:
                {
                    uint8_t reg_src_idx = (opcode & 0x00F0) >> 4;
                    uint8_t reg_dst_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[CPU_FLAGS_REG_IDX] = cpu->g_regs[reg_src_idx] >> 7;
                    cpu->g_regs[reg_dst_idx] = cpu->g_regs[reg_src_idx] << 1;
                    //cpu->g_regs[reg_dst_idx] <<= 1;
                    break;
                }
                default:
                    fprintf(stderr, "UNKNOWN : %4x\n", opcode);
                    break;
            }
            break;
        case 0x9000:
        {
            uint8_t reg1_idx = (opcode & 0x0F00) >> 8;
            uint8_t reg2_idx = (opcode & 0x00F0) >> 4;
            if (cpu->g_regs[reg1_idx] != cpu->g_regs[reg2_idx]) { cpu->pc += 2; }
            break;
        }
        case 0xA000:
        {
            uint16_t addr = opcode & 0x0FFF;
            cpu->I = addr;
            break;
        }
        case 0xB000:
        {
            uint16_t addr_offset = opcode & 0x0FFF;
            cpu->pc = cpu->g_regs[0] + addr_offset;
            break;
        }
        // Set random value to a reg
        case 0xC000:
        {
            uint8_t reg_idx = (opcode & 0x0F00) >> 8;
            uint8_t mask = opcode & 0x00FF;
            cpu->g_regs[reg_idx] = rand() & mask;
            break;
        }
        // Draw sprite
        case 0xD000:
        {
            uint8_t x = cpu->g_regs[(opcode & 0x0F00) >> 8];
            uint8_t y = cpu->g_regs[(opcode & 0x00F0) >> 4];
            uint8_t n = opcode & 0x000F;
            uint8_t flag = draw_sprite(screen, x, y, n, &memory[cpu->I]);
            cpu->g_regs[CPU_FLAGS_REG_IDX] = flag;
            fprintf(stderr, "draw in %d, %d, height %d, from %4x\n",
                    x, y, n, cpu->I);
            break;
        }
        case 0xE000:
            if ((opcode & 0x00FF) == 0x009E) {
                uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                if (cpu->g_regs[reg_idx] == checkKeyboard()) { cpu->pc += 2; }
            }
            else if ((opcode & 0x00FF) == 0x00A1) {
                uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                if (cpu->g_regs[reg_idx] != checkKeyboard()) { cpu->pc += 2; }
            }
            else fprintf(stderr, "UNKNOWN : %4x\n", opcode);
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                // Store delay timer to reg
                case 0x0007:
                {
                    uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[reg_idx] = cpu->delay_tim;
                    break;
                }
                case 0x000A:
                {
                    uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                    cpu->g_regs[reg_idx] = waitKeyboard();
                    break;
                }
                case 0x0015:
                // Load delay timer to reg
                {
                    uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                    cpu->delay_tim = cpu->g_regs[reg_idx];
                    break;
                }
                // Store sound timer to reg
                case 0x0018:
                {
                    uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                    cpu->sound_tim = cpu->g_regs[reg_idx];
                    break;
                }
                // Add reg to I
                case 0x001E:
                {
                    uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                    cpu->I += cpu->g_regs[reg_idx];
                    break;
                }
                // I points to a digit sprite
                case 0x0029:
                {
                    uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                    uint8_t digit = cpu->g_regs[reg_idx] & 0x0F;
                    cpu->I = digit * 5;
                    break;
                }
                // Hex to BCD
                case 0x0033:
                {
                    uint8_t digits[3];
                    uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                    hex_to_bcd(cpu->g_regs[reg_idx], digits);
                    write_memory(memory, digits[0], cpu->I);
                    write_memory(memory, digits[1], cpu->I + 1);
                    write_memory(memory, digits[2], cpu->I + 2);
                    break;
                }
                case 0x0055:
                {
                    uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                    int i;
                    for (i = 0; i <= reg_idx; ++i) {
                        write_memory(memory, cpu->g_regs[i], cpu->I + i);
                    }
                    break;
                }
                case 0x0065:
                {
                    uint8_t reg_idx = (opcode & 0x0F00) >> 8;
                    int i;
                    for (i = 0; i <= reg_idx; ++i) {
                        cpu->g_regs[i] = read_memory(memory, cpu->I + i);
                    }
                    break;
                }
                default:
                    fprintf(stderr, "UNKNOWN : %4x\n", opcode);
                    break;
            }
            break;
        default:
            fprintf(stderr, "UNKNOWN : %4x\n", opcode);
            break;
    }
    update_timers(cpu);
}
