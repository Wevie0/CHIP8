#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "platform.h"

const size_t START_ADDRESS = 0x200;
const size_t FONT_SIZE = 80;
const size_t FONT_START_ADDRESS = 0x50;

const size_t VIDEO_WIDTH = 64;
const size_t VIDEO_HEIGHT = 32;

#define VIDEO_SIZE (VIDEO_WIDTH * VIDEO_HEIGHT)

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

typedef struct chip8 {
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

uint8_t randByte() {
  uint8_t r = rand() % 256;
  return r;
}

void loadROM(chip8 *chip8, const char *const filename) {
  uint8_t *buffer;
  FILE *file;
  size_t fsize;

  file = fopen(filename, "rb");

  if (file == NULL) {
    perror("Error opening file\n");
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

  for (int i = 0; i < fsize; i++) {
    chip8->memory[START_ADDRESS + i] = buffer[i];
  }

  free(buffer);
}

chip8 *new_chip8() {
  chip8 *chip = malloc(sizeof(chip8));
  // 0 initialize
  *chip = (chip8){0};

  // initalize font
  for (size_t i = 0; i < FONT_SIZE; i++) {
    chip->memory[FONT_START_ADDRESS + i] = font[i];
  }

  // Init start address
  chip->pc = START_ADDRESS;

  return chip;
}

void cycle(chip8 *chip8) {
  // current instruction
  uint16_t ins = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1];
  chip8->pc += 2;

  // switch

  unsigned char first_nibble = (ins >> 12) & 0xF;
  unsigned char second_nibble = (ins >> 8) & 0xF;
  unsigned char third_nibble = (ins >> 4) & 0xF;
  unsigned char fourth_nibble = ins & 0xF;
  unsigned short last_byte = third_nibble << 4 | fourth_nibble;

  uint8_t *vx = &chip8->registers[second_nibble];
  uint8_t *vy = &chip8->registers[third_nibble];
  uint8_t *vf = &chip8->registers[0xF];
  uint16_t address = ins & 0xFFFu;
  switch (first_nibble) {
  case 0x0:

    switch (fourth_nibble) {
    case 0x0:
      memset(chip8->video, 0, sizeof(chip8->video));
      break;
    case 0xE:
      // RETURN
      chip8->sptr--;
      chip8->pc = chip8->stack[chip8->sptr];
      break;
    default:
      printf("%s: 0x%X\n", "Illegal Instruction", ins);
      break;
    }
    break;
  case 0x1:
    // JUMP
    chip8->pc = address;
    break;
  case 0x2:
    // FN CALL
    chip8->stack[chip8->sptr] = chip8->pc;
    chip8->sptr++;
    chip8->pc = address;
    break;
  case 0x3:
    if (*vx == last_byte) {
      chip8->pc += 2;
    }
    break;
  case 0x4:
    if (*vx != last_byte) {
      chip8->pc += 2;
    }
    break;
  case 0x5:
    if (*vx == *vy) {
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
    switch (fourth_nibble) {
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
      if (*vx + *vy > *vx) {
        *vf = 1;
      } else {
        *vf = 0;
      }

      *vx += *vf;
      break;
    case 0x5:
      // detect wrap around
      if (*vx - *vy > *vx) {
        *vf = 0;
      } else {
        *vf = 1;
      }

      *vx -= *vy;
      break;
    case 0x6:
      *vf = *vx & 1;
      *vx >>= 1;
      break;
    case 0x7:
      if (*vy - *vx > *vy) {
        *vf = 0;
      } else {
        *vf = 1;
      }
      *vx = *vy - *vx;
      break;
    case 0xE:
      if ((*vx & 0x80u) >> 7 == 1) {
        *vf = 1;
      } else {
        *vf = 0;
      }
      *vx <<= 1;
      break;
    default:
      printf("%s: 0x%X\n", "Illegal Instruction", ins);
    }
    break;
  case 0x9:
    if (*vx != *vy) {
      chip8->pc += 2;
    }
    break;
  case 0xA:
    chip8->index = address;
    break;
  case 0xB:
    chip8->pc = chip8->registers[0] + address;
    break;
  case 0xC:
    *vx = randByte() & last_byte;
    break;
  case 0xD:
    // DRAW
    uint8_t Vx = (ins & 0x0F00u) >> 8u;
    uint8_t Vy = (ins & 0x00F0u) >> 4u;
    uint8_t height = ins & 0x000Fu;

    // Wrap if going beyond screen boundaries
    uint8_t xPos = chip8->registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = chip8->registers[Vy] % VIDEO_HEIGHT;

    chip8->registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row) {
      uint8_t spriteByte = chip8->memory[chip8->index + row];

      for (unsigned int col = 0; col < 8; ++col) {
        uint8_t spritePixel = spriteByte & (0x80u >> col);
        uint32_t *screenPixel =
            &chip8->video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

        // Sprite pixel is on
        if (spritePixel) {
          // Screen pixel also on - collision
          if (*screenPixel == 0xFFFFFFFF) {
            chip8->registers[0xF] = 1;
          }

          // Effectively XOR with the sprite pixel
          *screenPixel ^= 0xFFFFFFFF;
        }
      }
    }

    break;
  case 0xE:
    switch (third_nibble) {
    case 0x9:
      if (chip8->keys[*vx]) {
        chip8->pc += 2;
      }
      break;
    case 0xA:
      if (!chip8->keys[*vx]) {
        chip8->pc += 2;
      }
      break;
    default:
      break;
    }

    break;
  case 0xF:
    switch (last_byte) {
    case 0x07:
      *vx = chip8->delay;
      break;
    case 0x0A:
      bool was_pressed = false;
      for (size_t i = 0; i < 16; i++) {
        if (chip8->keys[i]) {
          was_pressed = true;
          *vx = i;
          break;
        }
      }
      if (!was_pressed) {
        chip8->pc -= 2;
      }
      break;
    case 0x15:
      chip8->delay = *vx;
      break;
    case 0x18:
      chip8->sound = *vx;
      break;
    case 0x1E:
      chip8->index += *vx;
      break;
    case 0x29:
      chip8->index = FONT_START_ADDRESS + 5 * (*vx);
      break;
    case 0x33:
      uint8_t value = *vx;
      chip8->memory[chip8->index + 2] = value % 10;
      value /= 10;
      chip8->memory[chip8->index + 1] = value % 10;
      value /= 10;
      chip8->memory[chip8->index + 0] = value % 10;
      break;
    case 0x55:
      for (size_t i = 0; i < second_nibble; i++) {
        chip8->memory[chip8->index + i] = chip8->registers[i];
      }
      break;
    case 0x65:
      for (size_t i = 0; i < second_nibble; i++) {
        chip8->registers[i] = chip8->memory[chip8->index + i];
      }
      break;
    default:
      printf("%s: 0x%X\n", "Illegal Instruction", ins);
      break;
    }
    break;
  default:
    printf("%s: 0x%X\n", "Illegal Instruction", ins);
    break;
  }

  if (chip8->delay > 0) {
    chip8->delay--;
  }

  if (chip8->sound > 0) {
    chip8->sound--;
  }
}

void destroy_chip8(chip8 *chip8) { free(chip8); }

int main(int argc, char **argv) {
  srand(time(NULL));

  chip8 *chip8;
  platform *platform;
  size_t delay;
  bool quit = false;

  if (argc == 4) {
    chip8 = new_chip8();
    platform =
        new_platform("CHIP-8", atoi(argv[1]) * VIDEO_WIDTH,
                     atoi(argv[1]) * VIDEO_HEIGHT, VIDEO_WIDTH, VIDEO_HEIGHT);
    delay = atoi(argv[2]);
    loadROM(chip8, argv[3]);
  } else {
    fprintf(stderr, "Usage: %s <Scale (int)> <Delay (ms)> <ROM Path>\n",
            argv[0]);
    return 1;
  }

  clock_t last_cycle_time = clock();

  while (!quit) {
    quit = process_input(platform, chip8->keys);

    clock_t current_time = clock();

    if (current_time - last_cycle_time > (delay * CLOCKS_PER_SEC) / 1000) {
      last_cycle_time = current_time;
      cycle(chip8);
      update(platform, chip8->video, sizeof(chip8->video[0]) * VIDEO_WIDTH);
    }
  }

  destroy_chip8(chip8);
  destroy_platform(platform);

  return 0;
}