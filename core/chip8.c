#include <string.h>
#include <stdio.h>
#include "chip8.h"

void chip8_init(Chip8 *chip8)
{
    memset(chip8, 0, sizeof(*chip8));
    chip8->PC = CHIP8_PROGRAM_START;
}

int chip8_load_rom(Chip8 *chip8, const char *rom_path)
{
    FILE *rom = fopen(rom_path, "rb");
    if (!rom)
    {
        char msg[256];
        snprintf(msg, sizeof(msg), "Error loading rom : %s", rom_path);
        perror(msg);
        return -1;
    }

    size_t bytes_read = fread(&chip8->memory[CHIP8_PROGRAM_START], 1, CHIP8_MAX_ROM_SIZE, rom);

    if (bytes_read == 0)
    {
        fclose(rom);
        fprintf(stderr, "Error loading rom : %s: Empty file\n", rom_path);
        return -1;
    }
    else if (fgetc(rom) != EOF)
    {
        fclose(rom);
        fprintf(stderr, "Error loading rom : %s: file too big\n", rom_path);
        return -1;
    }

    fclose(rom);
    return 0;
}

uint16_t chip8_fetch(Chip8 *chip8)
{
    uint8_t high_byte = chip8->memory[chip8->PC & CHIP8_ADDR_MASK];
    uint8_t low_byte = chip8->memory[(chip8->PC + 1) & CHIP8_ADDR_MASK];

    uint16_t opcode = (high_byte << 8) | low_byte;
    chip8->PC = (chip8->PC + 2) & CHIP8_ADDR_MASK;

    return opcode;
}

void chip8_cycle(Chip8 *chip8)
{
    uint16_t opcode = chip8_fetch(chip8);

    uint8_t type = (opcode & 0xF000) >> 12;
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t N = opcode & 0x000F;
    uint8_t NN = opcode & 0x00FF;
    uint16_t NNN = opcode & CHIP8_ADDR_MASK;

    switch (type)
    {
    case 0x0:
        if (opcode == 0x00E0) // Clear the screen
        {
            memset(chip8->screen, 0, sizeof(chip8->screen));
        }
        else if (opcode == 0x00EE) // Return from subroutine
        {
            if (chip8->SP == 0)
            {
                fprintf(stderr, "Stack underflow at 0x%04x (opcode=0x%04x)\n", chip8->PC - 2, opcode);
                break;
            }
            chip8->SP--;
            chip8->PC = chip8->stack[chip8->SP];
        }
        else
        {
            fprintf(stderr, "Unrecognized opcode : 0x%04x at 0x%04x\n", opcode, chip8->PC - 2);
        }
        break;
    case 0x1: // goto
        chip8->PC = NNN;
        break;
    case 0x2: // subroutine
        if (chip8->SP >= CHIP8_STACK_SIZE)
        {
            fprintf(stderr, "Stack overflow at 0x%04x (opcode=0x%04x)\n", chip8->PC - 2, opcode);
            break;
        }
        chip8->stack[chip8->SP] = chip8->PC;
        chip8->SP++;
        chip8->PC = NNN;
        break;
    case 0x3: // Skip next opcode
        if (chip8->V[X] == NN)
            chip8->PC += 2;
        break;
    case 0x4: // Skip next opcode
        if (chip8->V[X] != NN)
            chip8->PC += 2;
        break;
    case 0x5: // Skip next opcode
        if (chip8->V[X] == chip8->V[Y])
            chip8->PC += 2;
        break;
    case 0x6: // Load register with immediate value
        chip8->V[X] = NN;
        break;
    case 0x7: // Add immediate value to register
        chip8->V[X] += NN;
        break;
    case 0x8:
        switch (N)
        {
        case 0x0:
            chip8->V[X] = chip8->V[Y];
            break;
        case 0x1:
            chip8->V[X] |= chip8->V[Y];
            break;
        case 0x2:
            chip8->V[X] &= chip8->V[Y];
            break;
        case 0x3:
            chip8->V[X] ^= chip8->V[Y];
            break;
        case 0x4:
        {
            uint16_t sum = chip8->V[X] + chip8->V[Y];
            bool carry = sum > UINT8_MAX;
            chip8->V[X] = sum;
            chip8->V[0xF] = carry;
            break;
        }
        case 0x5:
        {
            bool no_borrow = chip8->V[X] >= chip8->V[Y];
            chip8->V[X] -= chip8->V[Y];
            chip8->V[0xF] = no_borrow;
            break;
        }
        case 0x6:
        {
            bool shifted_out = chip8->V[Y] & 0x01;
            chip8->V[X] = chip8->V[Y] >> 1;
            chip8->V[0xF] = shifted_out;
            break;
        }
        case 0x7:
        {
            bool no_borrow = chip8->V[Y] >= chip8->V[X];
            chip8->V[X] = chip8->V[Y] - chip8->V[X];
            chip8->V[0xF] = no_borrow;
            break;
        }
        case 0xE:
        {
            bool shifted_out = (chip8->V[Y] >> 7) & 0x01;
            chip8->V[X] = chip8->V[Y] << 1;
            chip8->V[0xF] = shifted_out;
            break;
        }
        default:
            fprintf(stderr, "Unrecognized opcode : 0x%04x at 0x%04x\n", opcode, chip8->PC - 2);
            break;
        }
        break;
    case 0x9: // Skip next opcode
        if (chip8->V[X] != chip8->V[Y])
            chip8->PC += 2;
        break;
    case 0xA: // I = NNN
        chip8->I = NNN;
        break;
    case 0xD: // Draw sprite to screen
    {
        uint8_t x = chip8->V[X] % CHIP8_SCREEN_WIDTH;
        uint8_t y = chip8->V[Y] % CHIP8_SCREEN_HEIGHT;
        chip8->V[0x0F] = 0;
        for (int row = 0; row < N; row++)
        {
            if (y + row >= CHIP8_SCREEN_HEIGHT)
                break;
            uint8_t sprite_byte = chip8->memory[(chip8->I + row) & CHIP8_ADDR_MASK];
            for (int col = 0; col < CHIP8_SPRITE_WIDTH; col++)
            {
                if (x + col >= CHIP8_SCREEN_WIDTH)
                    break;
                bool sprite_pixel = (sprite_byte >> (CHIP8_SPRITE_WIDTH - 1 - col)) & 0x01;
                if (sprite_pixel)
                {
                    bool *pixel = &chip8->screen[y + row][x + col];
                    if (*pixel)
                        chip8->V[0x0F] = 1;
                    *pixel = !*pixel;
                }
            }
        }
    }
    break;
    case 0xF:
        switch (NN)
        {
        case 0x1E:
            chip8->I += chip8->V[X];
            break;
        case 0x33:
        {
            uint8_t vX = chip8->V[X];
            if (chip8->I + 2 >= CHIP8_MEMORY_SIZE)
            {
                fprintf(stderr, "Memory overflow at 0x%04x (0x%04x)\n", chip8->PC - 2, opcode);
                break;
            }
            chip8->memory[chip8->I] = vX / 100;
            chip8->memory[chip8->I + 1] = (vX / 10) % 10;
            chip8->memory[chip8->I + 2] = vX % 10;
            break;
        }
        case 0x55:
            if (chip8->I + X >= CHIP8_MEMORY_SIZE)
            {
                fprintf(stderr, "Memory overflow at 0x%04x (0x%04x)\n", chip8->PC - 2, opcode);
                break;
            }
            for (int i = 0; i <= X; i++)
                chip8->memory[chip8->I + i] = chip8->V[i];
            chip8->I += X + 1;
            break;
        case 0x65:
            if (chip8->I + X >= CHIP8_MEMORY_SIZE)
            {
                fprintf(stderr, "Memory overflow at 0x%04x (0x%04x)\n", chip8->PC - 2, opcode);
                break;
            }
            for (int i = 0; i <= X; i++)
                chip8->V[i] = chip8->memory[chip8->I + i];
            chip8->I += X + 1;
            break;
        default:
            fprintf(stderr, "Unrecognized opcode : 0x%04x at 0x%04x\n", opcode, chip8->PC - 2);
            break;
        }
        break;
    default:
        fprintf(stderr, "Unrecognized opcode : 0x%04x at 0x%04x\n", opcode, chip8->PC - 2);
        break;
    }
}