#include "game.h"
#include <stdio.h>

#define min(A,B) ((A<B) ? A : B)
#define max(A,B) ((A>B) ? A : B)

#define DEBUG

/* axis directions for scan_diag() */
typedef enum {D_HORIZ = 0x0001,
			  D_VERT = 0x0002,
			  D_UPLEFT = 0x0004,
			  D_UPRIGHT = 0x0008,
			  D_ALL=0x000F
} DIAGONAL;

typedef int FLIP_FUN(CHIP_COLOR *row[MAX_DIM], int size, CHIP_COLOR target_color, int pos);

int scan_diag(GAME_STATE *state, const GAME_TURN *turn, DIAGONAL diag,
			  FLIP_FUN *flip_fun) {
	CHIP_COLOR	*for_flip[MAX_DIM];
	int			flip_size, pos;
	int			x, y;
	int			flip = 0;

	if (diag & D_HORIZ) {
		flip_size = 0;
		for (x = 0, y = turn->y; x < MAX_DIM; x++) {
			if (x == turn->x) pos = flip_size;
			for_flip[flip_size++] = &((*state)[x][y]);
		}
		flip += flip_fun(for_flip, flip_size, turn->color, pos);
	}
	if (diag & D_VERT) {
		flip_size = 0;
		for (x = turn->x, y = 0; y < MAX_DIM; y++) {
			if (y == turn->y) pos = flip_size;
			for_flip[flip_size++] = &((*state)[x][y]);
		}
		flip += flip_fun(for_flip, flip_size, turn->color, pos);
	}
	if (diag & D_UPLEFT) {
		flip_size = 0;
		/* simple solution */
		x = max(0, (turn->x - turn->y));
		y = max(0, (turn->y - turn->x));
		for (; (x < MAX_DIM) && (y < MAX_DIM); x++, y++) {
			if (x == turn->x) pos = flip_size;
			for_flip[flip_size++] = &((*state)[x][y]);
		}
		flip += flip_fun(for_flip, flip_size, turn->color, pos);
	}
	if (diag & D_UPRIGHT) {
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
			for_flip[flip_size++] = &((*state)[x][y]);
		}
		flip += flip_fun(for_flip, flip_size, turn->color, pos);
	}
	return flip;
}

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

int flip_row_check_only(CHIP_COLOR *row[MAX_DIM], int size, CHIP_COLOR target_color, int pos) {
	int flip = 0;
	int	i;

	for (i = pos-1; (i >= 0) && OPPOSITE_COLOR(*(row[i]), target_color); i--);
	if ( (i >= 0) && SAME_COLOR(*(row[i++]), target_color))
		for(; i < pos; i++, flip++);
	for (i = pos+1; (i < size) && OPPOSITE_COLOR(*(row[i]), target_color); i++);
   	if ( (i < size) && SAME_COLOR(*(row[i--]), target_color))
		for (; i > pos; i--, flip++);
	return flip;
} 

/* 
 * Make turn on position 'state'.
 * Return amount of flip overs
 */
int game_turn(GAME_STATE *state, const GAME_TURN *turn) {
	(*state)[turn->x][turn->y] = turn->color; 
	return scan_diag(state, turn, D_ALL, flip_row);
}

/*
 * Evaluate position 'state'
 */
double game_eval(const GAME_STATE *state) {
	return 0.0;
}

/*
 * Turns generator
 * Pick next possible turn in game position state with turn history 'turn_hist'
 * returns 0 if no possible turns left
 */
int get_next_turn(GAME_TURN *turn, const GAME_STATE *state, const GAME_TURN_HIST *hist) {
	int			first_x;
	GAME_TURN	t;

	t.color = hist->color;

	first_x = hist->x+1;
	for (t.y = hist->y; t.y < MAX_DIM; t.y++) {
		for(t.x = first_x; t.x < MAX_DIM; t.x++) {
			if (validate_turn(state, &t) == E_OK) {
				*turn = t;
				return 1;
			}
		}
		first_x = 0;
	}
	return 0;
}

/*
 * Validate turn
 * returns
 * 0 - E_OK - Ok
 * 1 - E_OCC - is already occupied
 * 2 - E_NO_OPP - no opposite chips around
 * 3 - E_NO_FLIPS - no flips
 */
int validate_turn(const GAME_STATE *state, const GAME_TURN *turn) {
	int			x, y;
	CHIP_COLOR	color;
	
	x = turn->x;
	y = turn->y;
	color = turn->color;
	/* you cannot make a turn if this place is already occupied */
	if ((*state)[x][y] != COLOR_VACANT) return E_OCC;

	/* you cannot make a turn if there is no opposite chips around */
	for (x = min(turn->x-1, 0); x < max(turn->x+1, MAX_DIM); x++) {
		for (y = min(turn->y-1, 0); y < max(turn->y+1, MAX_DIM); y++) {
			if (OPPOSITE_COLOR((*state)[x][y], color)) {
				goto flip_check;
			}
		}
	}
	return E_NO_OPP; /* no opposites around */

 flip_check:
	/* you cannot make a turn if there is no opposite chips to flip over */
	if (scan_diag(state, turn, D_HORIZ, flip_row_check_only)) {
		return E_OK; /* ok */
	}
	if (scan_diag(state, turn, D_VERT, flip_row_check_only)) {
		return E_OK; /* ok */
	}
	if (scan_diag(state, turn, D_UPLEFT, flip_row_check_only)) {
		return E_OK; /* ok */
	}
	if (scan_diag(state, turn, D_UPRIGHT, flip_row_check_only)) {
		return E_OK; /* ok */
	}
	return E_NO_FLIPS; /* no flip overs found */
}

/*
 * Append turn to history
 */
void log_turn(GAME_TURN_HIST *hist, const GAME_TURN *turn) {
	hist->x = turn->x;
	hist->y = turn->y;
	hist->color = turn->color;
}
