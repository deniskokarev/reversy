#include "minimax.h"
#include <string.h>

/*
 * The estimated value for each field on the game board
 */
GAME_SCORE m_weight[MAX_DIM][MAX_DIM] = {
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
GAME_SCORE game_eval(const GAME_STATE state, CHIP_COLOR color) {
	GAME_SCORE	eval[3] = {0, 0, 0};
	int		x, y;	

	for (x = 0; x < MAX_DIM; x++) {
		for (y = 0; y < MAX_DIM; y++) {
			eval[state[x][y]] += m_weight[x][y];
		}
	}
	return eval[color] - eval[ALTER_COLOR(color)];
}

GAME_SCORE find_best_turn(GAME_TURN *best_turn, 
					  const GAME_STATE state,
					  CHIP_COLOR color,
					  int depth,
					  GAME_SCORE simt)
{
	GAME_TURN	t, adv_best_turn;
	GAME_STATE	tmp_state;
	int			turns_revised = 0;
	int			captured = 0;
	GAME_SCORE		ascore, best_score;
	
	/* assume there are no turns */
	best_turn->x = -1;
	best_turn->y = -1;
	best_score = GAME_SCORE_MIN;
	/* copy game position */
	memcpy(tmp_state, state, sizeof(GAME_STATE));
	t.color = color;
	for (t.x = 0; t.x < MAX_DIM; t.x++) {
		for (t.y = 0; t.y < MAX_DIM; t.y++) {
			if (! quick_validate_turn(tmp_state, &t)) {
				if ((captured = make_turn(tmp_state, &t))) {
					if (depth) {
						/* find best for contendor */
						ascore = -find_best_turn(&adv_best_turn,
												 tmp_state,
												 ALTER_COLOR(color),
												 depth-1,
												 -best_score);
					} else {
						ascore = game_eval(tmp_state, color);
					}
					memcpy(tmp_state, state, sizeof(GAME_STATE)); /* undo make_turn() */
					turns_revised++;
					if (ascore > best_score) {
						best_score = ascore;
						*best_turn = t;
					}
#ifdef ALPHA_BETA_CUT_ON
					/* Check for Alpha or Beta cut on the game tree */ 
 					if (best_score > simt) {
 						return (best_score);
 					}
#endif
				}
			}
		}
	}
	if (!turns_revised) { /* No more turns found */
		if (depth) {
			/* try turn of the contendor */
			best_score = -find_best_turn(&adv_best_turn,
										 tmp_state,
										 ALTER_COLOR(color),
										 depth-1,
										 -simt);
		} else {
			/* 
			 * we've reached the final leaf and there are no possible turns,
			 * probably game is just over at this point
			 */
			best_score = (chips_count(state, color) - chips_count(state, ALTER_COLOR(color))) * 100;
		}
	}
	return (best_score);
}
