#include "game.h"
#include <stdio.h>
#include <string.h>

#define min(A,B) ((A<B) ? A : B)
#define max(A,B) ((A>B) ? A : B)

/* #define DEBUG */

/*
 * Flip over 1D array of chips. Actually array of pointers to chips.
 * Return number of captured ones.
 */
int flip_row(CHIP_COLOR *row[MAX_DIM], int size, CHIP_COLOR target_color, int pos) {

	int flip = 0;
	int	i;

#ifdef DEBUG
	char ch;
	printf("flip_row(for_flip, %d, %d, %d)\n", size, target_color, pos);
	for (i=0; i<size; i++) {
			switch (*row[i]) {
			case COLOR_WHITE:
				ch = '+';
				break;
			case COLOR_BLACK:
				ch = '*';
				break;
			case COLOR_VACANT:
			default:
				ch = '.';
			}
			printf(" %c", ch);
	}
	printf("\n");
	for (i=0; i<pos; i++) {
		printf("  ");
	}
	printf(" ^\n");
#endif
	for (i = pos-1; (i >= 0) && OPPOSITE_COLOR(*(row[i]), target_color); i--);
	if ( (i >= 0) && SAME_COLOR(*(row[i++]), target_color))
		while (i < pos) {
			*(row[i++]) = target_color;
			flip++;
		}
	for (i = pos+1; (i < size) && OPPOSITE_COLOR(*(row[i]), target_color); i++);
   	if ( (i < size) && SAME_COLOR(*(row[i--]), target_color))
		while (i > pos) {
			*(row[i--]) = target_color;
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
int flip_axises(GAME_STATE state, const GAME_TURN *turn) {
	CHIP_COLOR	*for_flip[MAX_DIM];
	int			flip_size, pos = 0;
	int			x, y;
	int			flip = 0;

	/* LEFT->RIGHT */
	flip_size = 0;
	for (x = 0, y = turn->y; x < MAX_DIM; x++) {
		if (x == turn->x) pos = flip_size;
		for_flip[flip_size++] = &(state[x][y]);
	}
	flip += flip_row(for_flip, flip_size, turn->color, pos);

	/* UP->DOWN */
	flip_size = 0;
	for (x = turn->x, y = 0; y < MAX_DIM; y++) {
		if (y == turn->y) pos = flip_size;
		for_flip[flip_size++] = &(state[x][y]);
	}
	flip += flip_row(for_flip, flip_size, turn->color, pos);

	/* UP_LEFT -> DOWN_RIGHT */ 
	flip_size = 0;
	/* simple solution */
	x = max(0, (turn->x - turn->y));
	y = max(0, (turn->y - turn->x));
	for (; (x < MAX_DIM) && (y < MAX_DIM); x++, y++) {
		if (x == turn->x) pos = flip_size;
		for_flip[flip_size++] = &(state[x][y]);
	}
	flip += flip_row(for_flip, flip_size, turn->color, pos);

	/* UP_RIGHT->DOWN_LEFT */
	/* choose between two possible UP_RIGHT cases (y=0 or x=7) */
	flip_size = 0;
	/* try y=0 */
	y = 0;
	x = turn->y + turn->x;
	if (x >= MAX_DIM) {
		/* try x=7 */
		x = MAX_DIM - 1;
		y = turn->y + turn->x - x;
		if (y < 0) {
			printf("!!! Unresolved equation !!!\n");
			exit(1);
		}
	}
	for (; (x >= 0) && (y < MAX_DIM); x--, y++) {
		if (x == turn->x) pos = flip_size;
		for_flip[flip_size++] = &(state[x][y]);
	}
	flip += flip_row(for_flip, flip_size, turn->color, pos);

	return flip;
}


/* 
 * Make turn on position 'state'.
 * Return amount of flip overs
 */
int make_turn(GAME_STATE state, const GAME_TURN *turn) {
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
int quick_validate_turn(const GAME_STATE state, const GAME_TURN *turn) {
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
int validate_turn(const GAME_STATE state, const GAME_TURN *turn) {
	GAME_STATE	tmp_state;
	int			e_code;

	if (! (e_code = quick_validate_turn(state, turn))) {
		/* Quick checking is OK */
		memcpy(tmp_state, state, sizeof(GAME_STATE));
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
int make_turn_list(GAME_TURN turn[MAX_DIM * MAX_DIM], const GAME_STATE state, CHIP_COLOR color) {
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
int chips_count(const GAME_STATE state, CHIP_COLOR color) {
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

int game_is_over(const GAME_STATE state) {
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

