#include <gb/gb.h>
#include <gbdk/bcd.h>

#include "ecto.h"
#include "hUGEDriver.h"

#define SFX_ON rAUDTERM & 0b10001

#define LEFT_MIN 0b10011
#define LEFT_MAX 0b110110
#define RIGHT_MIN 0b1111011
#define RIGHT_MAX 0b10011110
#define TOP_MIN 0b11011
#define TOP_MAX 0b111110
#define BOTTOM_MIN 0b1110011
#define BOTTOM_MAX 0b10010110

#define ASSIST ecto_state & 0b1000

uint8_t puzzle_id = 0, hearts, num_lsb, num_msb,
buff[0b1001], timer, clear_timer, buff_assist[0b1001];
uint16_t goal, num_assist;
BCD goal_bcd, bcd_assist;
const uint8_t CLEAR_MSG[0b110] = {0b10000, 1, 0b1010011, 0b1001111, 0b1011111, 0b1101010},
CLEAR_MSG_69[0b100] = {0b1011011, 0b1010111, 0b1010001, 0b1010011};

extern const hUGESong_t MUS_SECOND_HALF;

void number_update(void)
{
	if (puzzle_id < 0b100000) {
		num_msb = 0;

		if (puzzle_id < 0b10) {
			num_lsb &= 0b11; // Tier One; only need to check last two bits

			for (uint8_t i = 0; i < 0b10; i++)
				set_bkg_tile_xy(0b1010 - i, 0b111, (num_lsb >> i) & 1); // Draw bits according to memory
		} else if (puzzle_id < 0b100) {
			num_lsb &= 0b1111; // Tier Two; only need to check last four bits

			for (uint8_t i = 0; i < 0b100; i++)
				set_bkg_tile_xy(0b1011 - i, 0b111, (num_lsb >> i) & 1);
		} else {
			for (uint8_t i = 0; i < 0b1000; i++)
				set_bkg_tile_xy(0b1101 - i, 0b111, (num_lsb >> i) & 1);
		}
	} else {
		for (uint8_t i = 0; i < 0b1000; i++) {
			set_bkg_tile_xy(0b10001 - i, 0b111, (num_lsb >> i) & 1);
			set_bkg_tile_xy(0b1001 - i, 0b111, (num_msb >> i) & 1);
		}
	}

	if (ASSIST) {
		num_assist = ((uint16_t)num_msb << 0b1000) + num_lsb;
		uint2bcd(num_assist, &bcd_assist);
		bcd2text(&bcd_assist, 0b100, buff_assist);
		for (uint8_t i = 0; i < 0b111; i++) {
			if (buff_assist[i] != 0b100)
				break;
			buff_assist[i] = 0b1111111; // Hide leading zeroes
		}
		set_bkg_tiles(0b1000, 0b1000, 0b1000, 1, buff_assist);
	}
}

uint8_t puzzle_init(uint8_t pid)
{
	for (uint8_t i = 0; i < 0b110; i++)
		set_bkg_tile_xy(0b101 - i, 0, (pid >> i) & 1);

	num_lsb = 0;
	num_msb = 0;

	for (uint8_t i = 0; i < 0b110; i++)
		set_bkg_tile_xy(i, 1, 0b1111111); // Clear clear message
	for (uint8_t i = 0; i < 0b101; i++)
		set_bkg_tile_xy(i + 0b1010, 0b1011, 0b1111111); // Clear BCD goal

	switch (pid) {
	case 0:
		goal = 1;
		hearts = 1;
		break;

	case 1:
		goal = 0b10;
		hearts = 0b10;
		break;

	case 0b10:
		goal = 0b100;
		hearts = 0b11;
		break;

	case 0b11:
		goal = 0b101;
		hearts = 0b100;
		break;

	case 0b100:
		goal = 0b111;
		hearts = 0b101;
		break;

	case 0b101:
		goal = 0b1100;
		hearts = 0b101;
		break;

	case 0b110:
		goal = 0b1110;
		hearts = 0b110;
		break;

	case 0b111:
		goal = 0b1011;
		hearts = 0b110;
		break;

	case 0b1000:
		goal = 0b1001;
		hearts = 0b101;
		break;

	case 0b1001:
		goal = 0b10011;
		hearts = 0b111;
		break;

	case 0b1010:
		goal = 0b11011;
		hearts = 0b1000;
		break;

	case 0b1011:
		goal = 0b10111;
		hearts = 0b111;
		break;

	case 0b1100:
		goal = 0b101010;
		hearts = 0b1000;
		break;

	case 0b1101:
		goal = 0b110011;
		hearts = 0b1001;
		break;

	case 0b1110:
		goal = 0b1011111;
		hearts = 0b1001;
		break;

	case 0b1111:
		goal = 0b11111111;
		hearts = 1;
		break;

	case 0b10000:
		goal = 0b10000000;
		hearts = 0b11;
		break;

	case 0b10001:
		goal = 0b100000;
		hearts = 0b101;
		break;

	case 0b10010:
		goal = 0b1000101;
		hearts = 0b1001;
		break;

	case 0b10011:
		goal = 0b1101001;
		hearts = 0b1010;
		break;

	case 0b10100:
		goal = 0b10100010;
		hearts = 0b1010;
		break;

	case 0b10101:
		goal = 0b111010;
		hearts = 0b111;
		break;

	case 0b10110:
		goal = 0b11111100;
		hearts = 0b11;
		break;

	case 0b10111:
		goal = 0b11111000;
		hearts = 0b100;
		break;

	case 0b11000:
		goal = 0b1111000;
		hearts = 0b110;
		break;

	case 0b11001:
		goal = 0b11001100;
		hearts = 0b1011;
		break;

	case 0b11010:
		goal = 0b10101010;
		hearts = 0b1011;
		break;

	case 0b11011:
		goal = 0b1111110;
		hearts = 0b11;
		break;

	case 0b11100:
		goal = 0b1011000;
		hearts = 0b1001;
		break;

	case 0b11101:
		goal = 0b11010010;
		hearts = 0b1011;
		break;

	case 0b11110:
		goal = 0b10111101;
		hearts = 0b1010;
		break;

	case 0b11111:
		goal = 0b1101111;
		hearts = 0b1010;
		break;

	case 0b100000:
		goal = 0b1111111111111111;
		hearts = 1;
		break;

	case 0b100001:
		goal = 0b1000000000000000;
		hearts = 0b11;
		break;

	case 0b100010:
		goal = 0b10000000000;
		hearts = 0b1000;
		break;

	case 0b100011:
		goal = 0b111111111111110;
		hearts = 0b11;
		break;

	case 0b100100:
		goal = 0b100000001;
		hearts = 0b1010;
		break;

	case 0b100101:
		goal = 0b100010000;
		hearts = 0b1010;
		break;

	case 0b100110:
		goal = 0b1010101010;
		hearts = 0b1110;
		break;

	case 0b100111:
		goal = 0b1111101000;
		hearts = 0b1110;
		break;

	case 0b101000:
		goal = 0b1010000000;
		hearts = 0b1011;
		break;

	case 0b101001:
		goal = 0b110100100;
		hearts = 0b1100;
		break;

	case 0b101010:
		goal = 0b10000100000;
		hearts = 0b1100;
		break;

	case 0b101011:
		goal = 0b1011010000;
		hearts = 0b1101;
		break;

	case 0b101100:
		goal = 0b100111011;
		hearts = 0b1101;
		break;

	case 0b101101:
		goal = 0b1111111111110111;
		hearts = 0b1000;
		break;

	case 0b101110:
		goal = 0b1111111111110011;
		hearts = 0b111;
		break;

	case 0b101111:
		goal = 0b1111111111100111;
		hearts = 0b1001;
		break;

	case 0b110000:
		goal = 0b11111101000;
		hearts = 0b1110;
		break;

	case 0b110001:
		goal = 0b1111111100000000;
		hearts = 0b1001;
		break;

	case 0b110010:
		goal = 0b1011111111111101;
		hearts = 0b10010;
		break;

	case 0b110011:
		goal = 0b10101010101010;
		hearts = 0b10100;
		break;

	case 0b110100:
		goal = 0b110011001100;
		hearts = 0b10001;
		break;

	case 0b110101:
		goal = 0b1111111110011000;
		hearts = 0b1010;
		break;

	case 0b110110:
		goal = 0b11000000111001;
		hearts = 0b10010;
		break;

	case 0b110111:
		goal = 0b1011101000;
		hearts = 0b1101;
		break;

	case 0b111000:
		goal = 0b111000111000;
		hearts = 0b10000;
		break;

	case 0b111001:
		goal = 0b1110000000000111;
		hearts = 0b10000;
		break;

	case 0b111010:
		goal = 0b1001110110000;
		hearts = 0b10001;
		break;

	case 0b111011:
		goal = 0b1000010000100001;
		hearts = 0b10011;
		break;

	case 0b111100:
		goal = 0b1010011010;
		hearts = 0b1110;
		break;

	case 0b111101:
		goal = 0b10011100010000;
		hearts = 0b10001;
		break;

	case 0b111110:
		goal = 0b10001010111;
		hearts = 0b1111;
		break;

	default:
		goal = 0b1010000000001011;
		hearts = 0b10100;
	}

	if (!(ASSIST)) {
		for (uint8_t i = 0; i < hearts; i++)
			set_bkg_tile_xy(i, 1, 0b10); // Draw hearts
	}

	uint2bcd(goal, &goal_bcd);
	bcd2text(&goal_bcd, 0b100, buff); // 0b100 is the index of the decimal 0 character in VRAM
	for (uint8_t i = 0; i < 0b111; i++) {
		if (buff[i] != 0b100)
			break;
		buff[i] = 0b1111111; // Hide leading zeroes
	}
	set_bkg_tiles(0b1000, 0b1010, 0b1000, 1, buff); // Draw BCD goal

	number_update();

	return pid;
}

inline void lose_heart(void)
{
	hearts--;
	set_bkg_tile_xy(hearts, 1, 0b1101000);
}

void puzzle_update(void)
{
	if ((num_lsb == (goal & 0b11111111)) && (num_msb == (goal >> 0b1000))) {
		if (clear_timer == 0) {
			if (!(ASSIST)) {
				for (uint8_t i = 0b100; i < 0b10100; i++)
					set_bkg_tile_xy(i, 1, 0b1111111); // Clear hearts
			}
			if (puzzle_id == 0b10010)
				set_bkg_tiles(0, 1, 0b100, 1, CLEAR_MSG_69);
			else
				set_bkg_tiles(0, 1, 0b110, 1, CLEAR_MSG);

			if (SFX_ON)
				rAUDTERM |= 0b10001;
			rAUD1SWEEP = 0b110111;
			rAUD1LEN = 0b1010;
			rAUD1ENV = 0b11111011;
			rAUD1LOW = 0b1000000;
			rAUD1HIGH = 0b10000110;
		}

		if (clear_timer == 0b1000000 || (joy & J_B && !(joy_prev & J_B))) {
			for (uint8_t i = 0; i < 0b101; i++) {
				set_bkg_tile_xy(i, 0, 0b1111111);
			}
			puzzle_id = puzzle_init(++puzzle_id); // Puzzle passed; go to the next one
			if (puzzle_id == 0b100000)
				hUGE_init(&MUS_SECOND_HALF);
			clear_timer = 0;
		} else {
			clear_timer++;
		}

	} else if (hearts == 0 || ((joy & J_B) && !(joy_prev & J_B))) {
		puzzle_init(puzzle_id); // Puzzle failed or restart button pressed; restart

		if (SFX_ON)
			rAUDTERM |= 0b10001;
		rAUD1SWEEP = 0b11111;
		rAUD1LEN = 0;
		rAUD1ENV = 0b11110100;
		rAUD1LOW = 0b10000000;
		rAUD1HIGH = 0b10000101;
	}

	if (joy & J_A && clear_timer == 0) {
		if (!(joy_prev & J_A))
			timer = 0;

		if (timer > 0) {
			timer--;
		} else {
			timer = 0b1100;

			if (pos_x > LEFT_MIN && pos_x < LEFT_MAX) {
				if (pos_y > TOP_MIN && pos_y < TOP_MAX) { // Increment
					num_lsb++;
					if (num_lsb == 0)
						num_msb++;
					number_update();
					if (!(ASSIST))
						lose_heart();

					if (SFX_ON)
						rAUDTERM |= 0b10001;
					rAUD1SWEEP = 0b100101;
					rAUD1LEN = 0b1000000;
					rAUD1ENV = 0b11110100;
					rAUD1LOW = 0b10000000;
					rAUD1HIGH = 0b10000110;
				}

				if (pos_y > BOTTOM_MIN && pos_y < BOTTOM_MAX) { // Decrement
					num_lsb--;
					if (num_lsb == 0b11111111)
						num_msb--;
					number_update();
					if (!(ASSIST))
						lose_heart();

					if (SFX_ON)
						rAUDTERM |= 0b10001;
					rAUD1SWEEP = 0b101101;
					rAUD1LEN = 0b1000000;
					rAUD1ENV = 0b11110010;
					rAUD1LOW = 0b100000;
					rAUD1HIGH = 0b10000111;
				}
			}

			if ((pos_x > RIGHT_MIN && pos_x < RIGHT_MAX) && (num_lsb != 0 || num_msb != 0)) { // Bit shifting doesn’t work on 0
				if (pos_y > TOP_MIN && pos_y < TOP_MAX) { // Left shift
					num_msb <<= 1;
					if (num_lsb & 0b10000000)
						num_msb |= 1;
					num_lsb <<= 1;
					number_update();
					if (!(ASSIST))
						lose_heart();

					// Due to a bug in binjgb that inverts the panning, the web build re-inverts it
					if (SFX_ON) {
//						rAUDTERM |= 1;
//						rAUDTERM &= ~(0b10000);
						rAUDTERM |= 0b10000;
						rAUDTERM &= ~(1);
					}
					rAUD1SWEEP = 0;
					rAUD1LEN = 0;
					rAUD1ENV = 0b11110010;
					rAUD1LOW = 0;
					rAUD1HIGH = 0b10000111;
				}

				if (pos_y > BOTTOM_MIN && pos_y < BOTTOM_MAX) { // Right shift
					num_lsb >>= 1;
					if (num_msb & 1)
						num_lsb |= 0b10000000;
					num_msb >>= 1;
					number_update();
					if (!(ASSIST))
						lose_heart();

					if (SFX_ON) {
//						rAUDTERM |= 0b10000;
//						rAUDTERM &= ~(1);
						rAUDTERM |= 1;
						rAUDTERM &= ~(0b10000);
					}
					rAUD1SWEEP = 0;
					rAUD1LEN = 0;
					rAUD1ENV = 0b11110010;
					rAUD1LOW = 0;
					rAUD1HIGH = 0b10000110;
				}
			}
		}
	}
}
