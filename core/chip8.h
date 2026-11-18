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

typedef struct
{
    uint8_t memory[CHIP8_MEMORY_SIZE];
    uint8_t V[CHIP8_NB_REG];
    uint16_t I;
    uint16_t PC;
    uint16_t stack[CHIP8_STACK_SIZE];
    uint8_t SP;
    uint8_t delay_timer;
    uint8_t sound_timer;
    bool screen[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH];
    bool keyboard[CHIP8_NB_KEYPAD];
} Chip8;

void chip8_init(Chip8 *chip8);
int chip8_load_rom(Chip8 *chip8, const char *rom_path);

#endif
