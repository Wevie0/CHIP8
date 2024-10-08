#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

const size_t START_ADDRESS = 0x200;
const size_t FONT_SIZE = 80;
const size_t FONT_START_ADDRESS = 0x50;

typedef struct chip8
{
    uint8_t registers[16];
    uint8_t memory[4096];
    uint16_t opcode;
    uint16_t pc;
    uint16_t index;
    uint16_t stack[16];
    uint8_t sptr;
    uint8_t delay;
    uint8_t sound;
    uint8_t keys[16];
    // length * width
    uint32_t video[64 * 32];
} chip8;

char randByte()
{
    char r = rand() % 256;
    return r;
}

void loadROM(chip8 *chip8, const char *const filename)
{
    uint8_t *buffer;
    FILE *file;
    size_t fsize;

    file = fopen(filename, "rb");

    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    // Get size of the file
    fseek(file, 0, SEEK_END);
    fsize = ftell(file);
    rewind(file);
    buffer = malloc(fsize * sizeof(uint8_t));

    // Read file
    fread(buffer, sizeof(uint8_t), fsize, file);
    fclose(file);

    for (int i = 0; i < fsize; i++)
    {
        chip8->memory[START_ADDRESS + i] = buffer[i];
    }

    free(buffer);
}

chip8 *new_chip8()
{
    const uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    chip8 *chip = malloc(sizeof(chip8));
    // 0 initalize
    *chip = (chip8){};

    // initalize font
    for (size_t i = 0; i < FONT_SIZE; i++)
    {
        chip->memory[FONT_START_ADDRESS + i] = font[i];
    }

    // Init start address
    chip->pc = START_ADDRESS;

    return chip;
}

void cycle(chip8 *chip8)
{
    // current instruction
    uint16_t ins = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1];
    chip8->pc += 2;

    // switch

    char first_nibble = (ins >> 12) & 0xF;
    char second_nibble = (ins >> 8) & 0xF;
    char third_nibble = (ins >> 4) & 0xF;
    char fourth_nibble = ins & 0xF;
    short last_byte = third_nibble << 4 | fourth_nibble;

    uint8_t *vx = &chip8->registers[second_nibble];
    uint8_t *vy = &chip8->registers[third_nibble];
    uint8_t *vf = &chip8->registers[0xF];

    switch (first_nibble)
    {
    case 0x0:

        switch (fourth_nibble)
        {
        case 0x0:
            memset(chip8->video, 0, sizeof(chip8->video));
            break;
        case 0xE:
            // return
            break;
        default:
            printf("%s: 0x%X\n", "Illegal Instruction", ins);
            break;
        }
        break;
    case 0x1:
        // JUMP
        uint16_t address = ins & 0x0FFFu;
        chip8->pc = address;
        break;
    case 0x2:
        // CALL
        break;
    case 0x3:
        if (*vx == last_byte)
        {
            chip8->pc += 2;
        }
        break;
    case 0x4:
        if (*vx != last_byte)
        {
            chip8->pc += 2;
        }
        break;
    case 0x5:
        if (*vx == *vy)
        {
            chip8->pc += 2;
        }
        break;
    case 0x6:
        *vx = last_byte;
        break;
    case 0x7:
        *vx += last_byte;
        break;
    case 0x8:
        switch (fourth_nibble)
        {
        case 0x0:
            *vx = *vy;
            break;
        case 0x1:
            *vx |= *vy;
            break;
        case 0x2:
            *vx &= *vy;
            break;
        case 0x3:
            *vx ^= *vy;
            break;
        case 0x4:

            // detect wrap around
            if (*vx + *vy < *vx)
            {
                *vf = 1;
            }
            else
            {
                *vf = 0;
            }

            *vx += *vf;
            break;
        case 0x5:
            // detect wrap around
            if (*vx - *vy > *vx)
            {
                *vf = 0;
            }
            else
            {
                *vf = 1;
            }

            *vx -= *vy;
            break;
        case 0x6:
            *vf = *vx & 1;
            *vx >>= 1;
            break;
        case 0x7:
            if (*vy - *vx > *vy)
            {
                *vf = 0;
            }
            else
            {
                *vf = 1;
            }
            *vx = *vy - *vx;
            break;
        case 0xE:
            if ((*vx & 0x80u) >> 7 == 1)
            {
                *vf = 1;
            }
            else
            {
                *vf = 0;
            }
            *vx <<= 1;
            break;
        default:
            printf("%s: 0x%X\n", "Illegal Instruction", ins);
        }
        break;
    case 0x9:
        if (*vx != *vy)
        {
            chip8->pc += 2;
        }
        break;
    case 0xA:
        uint16_t address = ins & 0x0FFFu;
        chip8->index = address;
        break;
    case 0xB:
        uint16_t address = ins & 0x0FFFu;
        chip8->pc = chip8->registers[0] + address;
        break;
    case 0xC:
        *vx = randByte() & last_byte;
        break;
    case 0xD:
        break;
    case 0xE:
        break;
    case 0xF:
        break;
    default:
        break;
    }

    if (chip8->delay > 0)
    {
        chip8->delay--;
    }

    if (chip8->sound > 0)
    {
        chip8->sound--;
    }
}

void destroy_chip8(chip8 *chip8)
{
    free(chip8);
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    return 0;
}