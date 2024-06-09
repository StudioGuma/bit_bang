#ifndef PUZZLES_H_
#define PUZZLES_H_

#define SFX_ON rAUDTERM & 0b10001

#define PL_WEB 0b11111111

#define LEFT_MIN 0b10011
#define LEFT_MAX 0b110110
#define RIGHT_MIN 0b1111011
#define RIGHT_MAX 0b10011110
#define TOP_MIN 0b11011
#define TOP_MAX 0b111110
#define BOTTOM_MIN 0b1111011
#define BOTTOM_MAX 0b10010110

extern uint8_t puzzle_id, timer;
extern const uint8_t PLATFORM;

uint8_t puzzle_init(uint8_t pid);
void puzzle_update(void);

#endif
