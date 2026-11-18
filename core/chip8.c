#include <string.h>
#include <stdio.h>
#include "chip8.h"

void chip8_init(Chip8 *chip8)
{
    memset(chip8, 0, sizeof(*chip8));
    chip8->PC = CHIP8_PROGRAM_START;
}

int chip8_load_rom(Chip8* chip8, const char *rom_path)
{
    FILE * rom = fopen(rom_path, "rb");
    if(!rom) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Error loading %s", rom_path);
        perror(msg);
        return -1;
    }

    size_t bytes_read = fread(&chip8->memory[CHIP8_PROGRAM_START], 1, CHIP8_MAX_ROM_SIZE, rom);

    if (bytes_read == 0) {
        fclose(rom);
        fprintf(stderr, "Error loading rom : %s: Empty file\n", rom_path);
        return -1;
    } else if (fgetc(rom) != EOF) {
        fclose(rom);
        fprintf(stderr, "Error loading rom : %s: file too big\n", rom_path);
        return -1;
    }

    fclose(rom);
    return 0;
}