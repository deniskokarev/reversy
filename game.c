#include "game.h"

#define min(A,B) ((A<B) ? A : B)
#define max(A,B) ((A>B) ? A : B)

/*
 * Flip over 1D array of chips. Actually array of pointers to chips.
 * Return number of captured ones.
 */
int flip_row(CHIP_COLOR *state, unsigned char ofs[MAX_DIM], int size, CHIP_COLOR target_color, int pos) {
	int flip = 0;
	int	i;

	for (i = pos-1; (i >= 0) && OPPOSITE_COLOR(*(state+ofs[i]), target_color); i--);
	if ( (i >= 0) && SAME_COLOR(*(state+ofs[i++]), target_color))
		while (i < pos) {
			*(state+ofs[i++]) = target_color;
			flip++;
		}
	for (i = pos+1; (i < size) && OPPOSITE_COLOR(*(state+ofs[i]), target_color); i++);
   	if ( (i < size) && SAME_COLOR(*(state+ofs[i--]), target_color))
		while (i > pos) {
			*(state+ofs[i--]) = target_color;
			flip++;
		}
	return flip;
} 


/*
 * Scan and try to flip all axises relative to given 'turn'
 * Don't touch 'turn' field itself.
 * We make axis projection on 1D array and call flip_row()
 * for every axis.
 * Return number of captured chips. 
 */
int flip_axises(GAME_STATE state, GAME_TURN *turn) {
	static struct {
		struct {
			int size, pos;
			unsigned char ofs[MAX_DIM];
		} lr, ud, uldr, urdl;
	} axss[MAX_DIM][MAX_DIM];
	static int axss_ini = 0;
	int			flip = 0;

	if (axss_ini == 0) {
		/* initialize axises shortcut */
		int px, py;
		int x, y;
		int pos = -1, flip_size;
		unsigned char for_flip[MAX_DIM];

		axss_ini = 1;
		for (px = 0; px < MAX_DIM; px++) {
			for (py = 0; py < MAX_DIM; py++) {
				/* LEFT->RIGHT */
				flip_size = 0;
				for (x = 0, y = py; x < MAX_DIM; x++) {
					if (x == px) pos = flip_size;
					for_flip[flip_size++] = &(state[x][y])-(CHIP_COLOR*)state;
				}
				XMEMCPY(axss[px][py].lr.ofs, for_flip, sizeof(for_flip));
				axss[px][py].lr.size = flip_size;
				axss[px][py].lr.pos = pos;
				/* UP->DOWN */
				flip_size = 0;
				for (x = px, y = 0; y < MAX_DIM; y++) {
					if (y == py) pos = flip_size;
					for_flip[flip_size++] = &(state[x][y])-(CHIP_COLOR*)state;
				}
				XMEMCPY(axss[px][py].ud.ofs, for_flip, sizeof(for_flip));
				axss[px][py].ud.size = flip_size;
				axss[px][py].ud.pos = pos;
				/* UP_LEFT -> DOWN_RIGHT */ 
				flip_size = 0;
				/* simple solution */
				x = max(0, (px - py));
				y = max(0, (py - px));
				for (; (x < MAX_DIM) && (y < MAX_DIM); x++, y++) {
					if (x == px) pos = flip_size;
					for_flip[flip_size++] = &(state[x][y])-(CHIP_COLOR*)state;
				}
				XMEMCPY(axss[px][py].uldr.ofs, for_flip, sizeof(for_flip));
				axss[px][py].uldr.size = flip_size;
				axss[px][py].uldr.pos = pos;
				/* UP_RIGHT->DOWN_LEFT */
				/* choose between two possible UP_RIGHT cases (y=0 or x=7) */
				flip_size = 0;
				/* try y=0 */
				y = 0;
				x = py + px;
				if (x >= MAX_DIM) {
					/* try x=7 */
					x = MAX_DIM - 1;
					y = py + px - x;
				}
				for (; (x >= 0) && (y < MAX_DIM); x--, y++) {
					if (x == px) pos = flip_size;
					for_flip[flip_size++] = &(state[x][y])-(CHIP_COLOR*)state;
				}
				XMEMCPY(axss[px][py].urdl.ofs, for_flip, sizeof(for_flip));
				axss[px][py].urdl.size = flip_size;
				axss[px][py].urdl.pos = pos;
			}
		}
	}
	flip += flip_row((CHIP_COLOR *)state, axss[turn->x][turn->y].lr.ofs, axss[turn->x][turn->y].lr.size, turn->color, axss[turn->x][turn->y].lr.pos);
	flip += flip_row((CHIP_COLOR *)state, axss[turn->x][turn->y].ud.ofs, axss[turn->x][turn->y].ud.size, turn->color, axss[turn->x][turn->y].ud.pos);
	flip += flip_row((CHIP_COLOR *)state, axss[turn->x][turn->y].uldr.ofs, axss[turn->x][turn->y].uldr.size, turn->color, axss[turn->x][turn->y].uldr.pos);
	flip += flip_row((CHIP_COLOR *)state, axss[turn->x][turn->y].urdl.ofs, axss[turn->x][turn->y].urdl.size, turn->color, axss[turn->x][turn->y].urdl.pos);

	return flip;
}


/* 
 * Make turn on position 'state'.
 * Return amount of flip overs
 */
int make_turn(GAME_STATE state, GAME_TURN *turn) {
	int flip;

	flip = flip_axises(state, turn);
	if (flip) {
		/* if there is something to capture */
		state[turn->x][turn->y] = turn->color; 
	}
	return flip;
}

/*
 * Make quick validation; sutable for automatic turn generation
 * returns. The final answer could be made only by flip_axises()
 * 0 - E_OK - Ok
 * otherwise - error
 */
int quick_validate_turn(GAME_STATE state, GAME_TURN *turn) {
	int			x, y;
	CHIP_COLOR	color;
	
	x = turn->x;
	y = turn->y;
	color = turn->color;
	/* you cannot make a turn if this place is already occupied */
	if (state[x][y] != COLOR_VACANT) return E_OCC;

	/* you cannot make a turn if there is no opposite chips around */
	for (x = min(turn->x-1, 0); x < max(turn->x+1, MAX_DIM); x++) {
		for (y = min(turn->y-1, 0); y < max(turn->y+1, MAX_DIM); y++) {
			if (OPPOSITE_COLOR(state[x][y], color)) {
				return E_OK;
			}
		}
	}
	return E_NO_OPP; /* no opposites around */
}

/*
 * Validate turn
 * Don't use it in automatic procedures, because of it is slow.
 * returns
 * 0 - E_OK - Ok
 * 1 - E_OCC - is already occupied
 * 2 - E_NO_OPP - no opposite chips around
 * 3 - E_NO_FLIPS - no flips
 */
int validate_turn(GAME_STATE state, GAME_TURN *turn) {
	GAME_STATE	tmp_state;
	int			e_code;

	if (! (e_code = quick_validate_turn(state, turn))) {
		/* Quick checking is OK */
		XMEMCPY(tmp_state, state, sizeof(GAME_STATE));
		if (! flip_axises(tmp_state, turn)) {
			/* Slow flip_axises is ERR */
			e_code = E_NO_FLIPS;
		}
	}
	return e_code;
}

/*
 * Turns generator. Don't use it in solution search, because of it is slow.
 * Makes a list of possible turns in game position 'state' by color 'color'
 * returns 0 if no possible turns left
 */
int make_turn_list(GAME_TURN turn[MAX_DIM * MAX_DIM], GAME_STATE state, CHIP_COLOR color) {
	GAME_TURN	t;
	int			i = 0;

	t.color = color;
	for (t.x = 0; t.x < MAX_DIM; t.x++) {
		for (t.y = 0; t.y < MAX_DIM; t.y++) {
			if (! validate_turn(state, &t)) {
				turn[i++] = t;
			}
		}
	}
	return i;
}

/*
 * Account chips of certain color on position 'state'
 */
int chips_count(GAME_STATE state, CHIP_COLOR color) {
	int	i = 0;
	int	x, y;

	for (x = 0; x < MAX_DIM; x++) {
		for (y = 0; y < MAX_DIM; y++) {
			if (SAME_COLOR(state[x][y], color))
				i++;
		}
	}
	return i;
}

int game_is_over(GAME_STATE state) {
	int	x, y;

	for (x = 0; x < MAX_DIM; x++) {	
		for (y = 0; y < MAX_DIM; y++) {
			if (SAME_COLOR(state[x][y], COLOR_VACANT)) {
				return 0;
			}
		}
	}
	return 1;
}

