#include "game.h"

#define min(A,B) ((A<B) ? A : B)
#define max(A,B) ((A>B) ? A : B)
#define dim(X)	(sizeof(X)/sizeof(X[0]))

typedef struct {
	int x, y;
	int px, py;
} AXIS_ITERATOR;

typedef enum {
	AXIS_DIR_BACKWARD = -1,
	AXIS_DIR_FORWARD = 1
} AXIS_DIR;

static CHIP_COLOR *axis_iter_next(GAME_STATE state, AXIS_ITERATOR *iter, AXIS_DIR dir) {
	iter->x += iter->px*dir;
	iter->y += iter->py*dir;
	if (iter->x >= 0 && iter->x < MAX_DIM && iter->y >= 0 && iter->y < MAX_DIM)
		return &state[iter->x][iter->y];
	else
		return 0;
}

/*
 * Flip over 1D array of chips.
 * Return number of captured ones.
 */
static int flip_row(GAME_STATE state, AXIS_ITERATOR *iter, CHIP_COLOR target_color) {
	int flip = 0;
	CHIP_COLOR	*pcolor;
	int n = 0;
	CHIP_COLOR opposite_color = ALTER_COLOR(target_color);
	pcolor = axis_iter_next(state, iter, AXIS_DIR_BACKWARD);
	while (pcolor && *pcolor == opposite_color) {
		pcolor = axis_iter_next(state, iter, AXIS_DIR_BACKWARD);
		n++;
	}
	if (pcolor && *pcolor == target_color) {
		while (n>0) {
			pcolor = axis_iter_next(state, iter, AXIS_DIR_FORWARD);
			*pcolor = target_color;
			flip++;
			n--;
		}
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
	AXIS_ITERATOR i;
	int flip = 0;
	/* VERTIVAL, HORIZONTAL, DIAGONAL, CROSS-DIAGONAL in both directions */
	for (i.px = -1; i.px <= 1; i.px++) {
		for (i.py = -1; i.py <= 1; i.py++) {
			if (i.px | i.py) {	/* except direction 0,0 */
				i.x = turn->x;
				i.y = turn->y;
				flip += flip_row(state, &i, turn->color);
			}
		}
	}
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
int quick_validate_turn(const GAME_STATE state, const GAME_TURN *turn) {
	/* you cannot make a turn if this place is already occupied */
	if (state[turn->x][turn->y] != COLOR_VACANT) return E_OCC;

	const int xb = min(turn->x-1, 0);
	const int xe = max(turn->x+1, MAX_DIM);
	const int yb = min(turn->y-1, 0);
	const int ye = max(turn->y+1, MAX_DIM);
	const CHIP_COLOR ac = ALTER_COLOR(turn->color);
	/* you cannot make a turn if there is no opposite chips around */
	for (int x = xb; x < xe; x++)
		for (int y = yb; y < ye; y++)
			if (state[x][y] == ac)
				return E_OK;
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

