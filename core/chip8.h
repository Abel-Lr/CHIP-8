#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>
#include <stdbool.h>

#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_NB_REG 16
#define CHIP8_STACK_SIZE 16
#define CHIP8_SCREEN_WIDTH 64
#define CHIP8_SCREEN_HEIGHT 32
#define CHIP8_NB_KEYPAD 16
#define CHIP8_PROGRAM_START 0x200
#define CHIP8_MAX_ROM_SIZE (CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_START)
#define CHIP8_SPRITE_WIDTH 8
#define CHIP8_ADDR_MASK 0x0FFF

typedef struct
{
    uint8_t memory[CHIP8_MEMORY_SIZE]; // 4 KB RAM, programs start at 0x200
    uint8_t V[CHIP8_NB_REG];           // 16 8-bit registers, VF is used as flag
    uint16_t I;                        // address register
    uint16_t PC;                       // address of the next instruction
    uint16_t stack[CHIP8_STACK_SIZE];  // return addresses for subroutines
    uint8_t SP;                        // index of the next free stack slot
    uint8_t delay_timer;
    uint8_t sound_timer;
    bool screen[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH]; // screen indexed as screen[y][x]
    bool keyboard[CHIP8_NB_KEYPAD];                       // 16 keys, true if pressed
    int fx0a_key;                                         // Key pressed during FX0A, -1 if none
} Chip8;

void chip8_init(Chip8 *chip8);
int chip8_load_rom(Chip8 *chip8, const char *rom_path);
uint16_t chip8_fetch(Chip8 *chip8);
void chip8_cycle(Chip8 *chip8);
void chip8_update_timers(Chip8 *chip8);

#endif
