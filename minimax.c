#include "minimax.h"

/*
 * The estimated value for each field on the game board
 */
/* CHIP_COLOR == signed char */
static const CHIP_COLOR m_weight[MAX_DIM][MAX_DIM] = {
	{ 5, -1,  3,  3,  3,  3, -1,  5},
 
	{-1, -2, -1, -1, -1, -1, -2, -1},

	{ 3, -1,  1,  1,  1,  1, -1,  3},

	{ 3, -1,  1,  1,  1,  1, -1,  3},

	{ 3, -1,  1,  1,  1,  1, -1,  3},

	{ 3, -1,  1,  1,  1,  1, -1,  3},

	{-1, -2, -1, -1, -1, -1, -2, -1},

	{ 5, -1,  3,  3,  3,  3, -1,  5},

};

/*
 * Evaluate position 'state', using estimation matrix
 */
static GAME_SCORE game_eval(const GAME_STATE *state, CHIP_COLOR color) {
	GAME_SCORE eval = 0;
	signed char	i, j;
	
	for (i = 0; i < MAX_DIM; i++)
		for (j = 0; j < MAX_DIM; j++) 
			eval += state->b[i][j]*m_weight[i][j];
	return eval * color;
}

GAME_SCORE find_best_turn_intr(GAME_TURN *best_turn,
							   const GAME_STATE *state,
							   CHIP_COLOR color,
							   int depth,
							   GAME_SCORE simt,
							   int (*is_stop)(int depth, void *param),
							   void *is_stop_param)
{
	GAME_STATE	tmp_state;
	GAME_TURN	t, adv_best_turn;
	GAME_SCORE		ascore, best_score;
	char			turns_revised = 0;
	
	/* assume there are no turns */
	best_turn->x = -1;
	best_turn->y = -1;
	best_score = GAME_SCORE_MIN;

	if (is_stop && is_stop(depth, is_stop_param))
		return best_score;

	tmp_state = *state;
	t.color = color;
	for (t.x = 0; t.x < MAX_DIM; t.x++) {
		for (t.y = 0; t.y < MAX_DIM; t.y++) {
			if (make_turn(&tmp_state, &t)) {
				if (depth) {
					/* find best for contendor */
					ascore = -find_best_turn_intr(&adv_best_turn,
												  &tmp_state,
												  ALTER_COLOR(color),
												  depth-1,
												  -best_score,
												  is_stop,
												  is_stop_param);
				} else {
					ascore = game_eval(&tmp_state, color);
				}
				tmp_state = *state; /* undo make_turn() */
				turns_revised++;
				if (ascore > best_score) {
					best_score = ascore;
					*best_turn = t;
				}
#ifndef ALPHA_BETA_CUT_OFF
				/* Check for Alpha or Beta cut on the game tree */ 
				if (best_score > simt) {
					goto ret;
				}
#endif
			}
		}
	}
	if (!turns_revised) { /* No more turns found */
		if (depth) {
			/* try turn of the contendor */
			best_score = -find_best_turn_intr(&adv_best_turn,
											  &tmp_state,
											  ALTER_COLOR(color),
											  depth-1,
											  -simt,
											  is_stop,
											  is_stop_param);
		} else {
			/* 
			 * we've reached the final leaf and there are no possible turns,
			 * probably game is just over at this point
			 */
			best_score = (chips_count(state, color) - chips_count(state, ALTER_COLOR(color))) * 100;
		}
	}
ret:
	return (best_score);
}

GAME_SCORE find_best_turn(GAME_TURN *best_turn, 
							   const GAME_STATE *state,
							   CHIP_COLOR color,
							   int depth)
{
	return find_best_turn_intr(best_turn, state, color, depth, GAME_SCORE_MAX, 0, 0);
}
