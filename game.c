#include "game.h"

#define min(A,B) ((A<B) ? A : B)
#define max(A,B) ((A>B) ? A : B)
#define dim(X)	(sizeof(X)/sizeof(X[0]))

typedef struct {
	signed char x, y;
	signed char px, py;
} AXIS_ITERATOR;

typedef enum {
	AXIS_DIR_BACKWARD = -1,
	AXIS_DIR_FORWARD = 1
} AXIS_DIR;

static CHIP_COLOR *axis_iter_next(GAME_STATE *state, AXIS_ITERATOR *iter, AXIS_DIR dir) {
	iter->x += iter->px*dir;
	iter->y += iter->py*dir;
	if (iter->x >= 0 && iter->x < MAX_DIM && iter->y >= 0 && iter->y < MAX_DIM)
		return &state->b[iter->x][iter->y];
	else
		return 0;
}

/*
 * Flip over 1D array of chips.
 * Return number of captured ones.
 */
static char flip_row(GAME_STATE *state, AXIS_ITERATOR *iter, CHIP_COLOR target_color) {
	CHIP_COLOR	*pcolor;
	CHIP_COLOR opposite_color = ALTER_COLOR(target_color);
	char flip = 0;
	signed char n = 0;
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
static char flip_axises(GAME_STATE *state, const GAME_TURN *turn) {
	AXIS_ITERATOR i;
	char flip = 0;
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
 * Make quick validation; sutable for automatic turn generation
 * returns. The final answer could be made only by flip_axises()
 * 1 - Yes
 * 0 - No
 */
static int quick_validate_turn(const GAME_STATE *state, const GAME_TURN *turn) {
	/* you cannot make a turn if this place is already occupied */
	return (state->b[turn->x][turn->y] == COLOR_VACANT);
}

/* 
 * Make turn on position 'state'.
 * Return amount of flip overs
 */
int make_turn(GAME_STATE *state, const GAME_TURN *turn) {
	char flip = 0;

	if (quick_validate_turn(state, turn)) {
		if ((flip = flip_axises(state, turn)) > 0) {
			/* if there is something to capture */
			state->b[turn->x][turn->y] = turn->color;
		}
	}
	return flip;
}


/*
 * Validate turn
 * Don't use it in automatic procedures, because of it is slow.
 * returns
 * E_OK - Ok
 * E_OCC - is already occupied
 * E_NO_FLIPS - no flips
 */
int validate_turn(const GAME_STATE *state, const GAME_TURN *turn) {
	GAME_STATE	tmp_state;
	int			e_code;

	if (!quick_validate_turn(state, turn)) {
		e_code = E_OCC;
	} else {
		/* Quick checking is OK */
		tmp_state = *state;
		if (! flip_axises(&tmp_state, turn)) {
			/* Slow flip_axises is ERR */
			e_code = E_NO_FLIPS;
		} else {
			e_code = E_OK;
		}
	}
	return e_code;
}

/*
 * Turns generator. Don't use it in solution search, because of it is slow.
 * Makes a list of possible turns in game position 'state' by color 'color'
 * returns 0 if no possible turns left
 */
int make_turn_list(GAME_TURN turn[MAX_DIM * MAX_DIM], const GAME_STATE *state, CHIP_COLOR color) {
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
int chips_count(const GAME_STATE *state, CHIP_COLOR color) {
	char	i = 0;
	signed char	x, y;

	for (x = 0; x < MAX_DIM; x++) {
		for (y = 0; y < MAX_DIM; y++) {
			if (SAME_COLOR(state->b[x][y], color))
				i++;
		}
	}
	return i;
}

/*
 * Game is over when neither color can make a turn
 */
int game_is_over(const GAME_STATE *state) {
	const static CHIP_COLOR all_colors[] = {COLOR_NEG, COLOR_POS, COLOR_VACANT};
	GAME_TURN	t;
	const CHIP_COLOR  *color = all_colors;
	
	for (t.color = *color; t.color != COLOR_VACANT; t.color = *color++)
		for (t.x = 0; t.x < MAX_DIM; t.x++)
			for (t.y = 0; t.y < MAX_DIM; t.y++)
				if (validate_turn(state, &t) == E_OK)
					return 0;
	return 1;
}
