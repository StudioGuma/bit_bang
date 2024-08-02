/*
Bit Bang — a numbers game
Copyright (C) 2024 StudioGuma

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <gb/gb.h>

#include "ecto.h"
#include "puzzles.h"
#include "hUGEDriver.h"

#include "bkg_tiles.h"
#include "bkg_map.h"
#include "title_screen.h"
#include "spr_ecto.h"

#define TITLE_STATE 0
#define GAME_STATE 1

#define MUSIC_ON rAUDTERM & 0b11101110
#define ASSIST_ON ecto_state |= 0b1000

uint8_t state;
const uint8_t THANKS[0b10011] = {0b100001, 0b1010110, 0b1001111, 0b1011011,
0b1011001, 0b1100000, 0b1111111, 0b1010100, 0b1011100, 0b1011111, 0b1111111,
0b1011101, 1, 0b1001111, 0b1100110, 0b1010111, 0b1011011, 0b1010101, 0b1101010};

extern const hUGESong_t MUS_TITLE_SCREEN, MUS_FIRST_HALF, MUS_SECOND_HALF;

void main(void)
{
	rAUDENA = AUDENA_ON;
	rAUDTERM = 0b11111111;
	rAUDVOL = 0b1110111;

	set_bkg_1bpp_data(0, 0b1110110, BKG_TILES);
	set_bkg_tiles(0, 0, 0b10100, 0b10010, TITLE_SCREEN);

	pos_x = 0b100000;
	pos_y = 0b1000000;

	hUGE_init(&MUS_TITLE_SCREEN);
	hUGE_mute_channel(HT_CH1, HT_CH_MUTE);

	SHOW_BKG;

	for (;;) {
		joy_prev = joy;
		joy = joypad();

		hUGE_dosound();

		if (joy & J_START && !(joy_prev & J_START)) {
			// Music (channels 2, 3, 4) toggle
			rAUDTERM ^= 0b11101110;

			if (MUSIC_ON) {
				rAUD1SWEEP = 0b100101;
				rAUD1LEN = 0b1000000;
				rAUD1ENV = 0b11110100;
				rAUD1LOW = 0b10000000;
				rAUD1HIGH = 0b10000110;
			} else {
				rAUD1SWEEP = 0b101101;
				rAUD1LEN = 0b1000000;
				rAUD1ENV = 0b11110010;
				rAUD1LOW = 0b100000;
				rAUD1HIGH = 0b10000111;
			}
		}

		if (joy & J_SELECT && !(joy_prev & J_SELECT)) {
			// SFX (channel 1) toggle
			if (SFX_ON) {
				rAUDTERM &= ~(0b10001);
			} else {
				rAUDTERM |= 0b10001;

				rAUD1SWEEP = 0b100101;
				rAUD1LEN = 0b1000000;
				rAUD1ENV = 0b11110100;
				rAUD1LOW = 0b10000000;
				rAUD1HIGH = 0b10000110;
			}
		}

		switch (state) {
		case TITLE_STATE:
			if (joy & J_A || joy & J_B) {
				if (joy & J_B)
					ASSIST_ON;

				set_bkg_tiles(0, 0, 0b10100, 0b10010, BKG_MAP);
				set_sprite_1bpp_data(0, 0b1001, SPR_ECTO);
				puzzle_init(puzzle_id);
				hUGE_init(&MUS_FIRST_HALF);

				state++;
				SHOW_SPRITES;
			}
			break;

		case GAME_STATE:
			ecto_update();
			puzzle_update();

			if (puzzle_id & 0b11000000) {
				puzzle_id = 0;

				set_bkg_tiles(1, 0b1010, 0b10011, 1, THANKS);
				hUGE_init(&MUS_TITLE_SCREEN);
				hUGE_mute_channel(HT_CH1, HT_CH_MUTE);

				for (uint8_t i = 0; i < 0b110; i++)
					set_bkg_tile_xy(0b101 - i, 0, (puzzle_id >> i) & 1);
				for (uint8_t i = 0; i < 0b10100; i++) {
					set_bkg_tile_xy(i, 1, 0b1111111);
					set_bkg_tile_xy(i, 0b111, 0b1111111);
					set_bkg_tile_xy(i, 0b1000, 0b1111111);
				}

				state++;
			}
			break;

		default:
			ecto_update();

			if (joy & J_A) {
				if (!(joy_prev & J_A))
					timer = 0;

				if (timer > 0) {
					timer--;
				} else {
					timer = 0b1100;

					if (pos_x > LEFT_MIN && pos_x < LEFT_MAX) {
						if (pos_y > TOP_MIN && pos_y < TOP_MAX) { // Increment
							puzzle_id++;
							puzzle_id &= 0b111111;
							for (uint8_t i = 0; i < 0b110; i++)
								set_bkg_tile_xy(0b101 - i, 0, (puzzle_id >> i) & 1);

							if (SFX_ON)
								rAUDTERM |= 0b10001;
							rAUD1SWEEP = 0b100101;
							rAUD1LEN = 0b1000000;
							rAUD1ENV = 0b11110100;
							rAUD1LOW = 0b10000000;
							rAUD1HIGH = 0b10000110;
						}

						if (pos_y > BOTTOM_MIN && pos_y < BOTTOM_MAX) { // Decrement
							puzzle_id--;
							puzzle_id &= 0b111111;
							for (uint8_t i = 0; i < 0b110; i++)
								set_bkg_tile_xy(0b101 - i, 0, (puzzle_id >> i) & 1);

							if (SFX_ON)
								rAUDTERM |= 0b10001;
							rAUD1SWEEP = 0b101101;
							rAUD1LEN = 0b1000000;
							rAUD1ENV = 0b11110010;
							rAUD1LOW = 0b100000;
							rAUD1HIGH = 0b10000111;
						}
					}

					if ((pos_x > RIGHT_MIN && pos_x < RIGHT_MAX) && (puzzle_id != 0)) { // Bit shifting doesn’t work on 0
						if (pos_y > TOP_MIN && pos_y < TOP_MAX) { // Left shift
							puzzle_id <<= 1;
							puzzle_id &= 0b111111;
							for (uint8_t i = 0; i < 0b110; i++)
								set_bkg_tile_xy(0b101 - i, 0, (puzzle_id >> i) & 1);

							if (SFX_ON) {
//								rAUDTERM |= 1;
//								rAUDTERM &= ~(0b10000);
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
							puzzle_id >>= 1;
							puzzle_id &= 0b111111;
							for (uint8_t i = 0; i < 0b110; i++)
								set_bkg_tile_xy(0b101 - i, 0, (puzzle_id >> i) & 1);

							if (SFX_ON) {
//								rAUDTERM |= 0b10000;
//								rAUDTERM &= ~(1);
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

			if ((joy & J_B) && !(joy_prev & J_B)) {
				set_bkg_tiles(0, 0, 0b10100, 0b10010, BKG_MAP);
				puzzle_init(puzzle_id);
				if (puzzle_id < 0b100000)
					hUGE_init(&MUS_FIRST_HALF);
				else
					hUGE_init(&MUS_SECOND_HALF);

				state--;
			}
		}

		vsync();
	}
}
