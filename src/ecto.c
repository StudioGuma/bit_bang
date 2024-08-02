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
#include <gbdk/metasprites.h>

#define SPEED 0b11

#define MOV_UP ecto_state & 1
#define MOV_UP_TRUE ecto_state |= 1
#define MOV_UP_FALSE ecto_state &= ~(1)
#define MOV_DOWN ecto_state & 0b10
#define MOV_DOWN_TRUE ecto_state |= 0b10
#define MOV_DOWN_FALSE ecto_state &= ~(0b10)
#define FLIPPED ecto_state & 0b100
#define FLIPPED_TRUE ecto_state |= 0b100
#define FLIPPED_FALSE ecto_state &= ~(0b100)

uint8_t pos_x, pos_y, joy, joy_prev, ecto_state;

const metasprite_t META_ECTO_IDLE[] = {
	{.dy = -0b1000, .dx = -0b1000, .dtile = 0, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b10, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 1, .props = 0b100000},
	METASPR_TERM
};

const metasprite_t META_ECTO_UP[] = {
	{.dy = -0b1000, .dx = -0b1000, .dtile = 0b11, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 0b100, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b101, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 0b100, .props = 0b100000},
	METASPR_TERM
};

const metasprite_t META_ECTO_DOWN[] = {
	{.dy = -0b1000, .dx = -0b1000, .dtile = 0b110, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 0b111, .props = 0},
	{.dy = -0b1000, .dx = 0b1000, .dtile = 0b1000, .props = 0},
	{.dy = 0b1000, .dx = 0, .dtile = 0b111, .props = 0b100000},
	METASPR_TERM
};

uint8_t ecto_update(void)
{
	MOV_UP_FALSE;
	MOV_DOWN_FALSE;

	if (joy & J_RIGHT) {
		pos_x += SPEED;
		if (FLIPPED)
			FLIPPED_FALSE;
	}
	if (joy & J_LEFT) {
		pos_x -= SPEED;
		if (!(FLIPPED))
			FLIPPED_TRUE;
	}
	if (joy & J_UP) {
		pos_y -= SPEED;
		MOV_UP_TRUE;
	}
	if (joy & J_DOWN) {
		pos_y += SPEED;
		MOV_DOWN_TRUE;
	}

	if (pos_x > 0b10100010)
		pos_x = 0b10100010;
	if (pos_x < 0b1110)
		pos_x = 0b1110;
	if (pos_y > 0b10011010)
		pos_y = 0b10011010;
	if (pos_y < 0b10110)
		pos_y = 0b10110;

	if (MOV_UP)
		return FLIPPED ? move_metasprite_flipx(META_ECTO_UP, 0, 0, 0, pos_x, pos_y)
		: move_metasprite_ex(META_ECTO_UP, 0, 0, 0, pos_x, pos_y);
	else if (MOV_DOWN)
		return FLIPPED ? move_metasprite_flipx(META_ECTO_DOWN, 0, 0, 0, pos_x, pos_y)
		: move_metasprite_ex(META_ECTO_DOWN, 0, 0, 0, pos_x, pos_y);
	else
		return FLIPPED ? move_metasprite_flipx(META_ECTO_IDLE, 0, 0, 0, pos_x, pos_y)
		: move_metasprite_ex(META_ECTO_IDLE, 0, 0, 0, pos_x, pos_y);
}
