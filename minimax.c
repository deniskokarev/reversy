#include "minimax.h"
#include <string.h>
#include <math.h>

double m_weight[MAX_DIM][MAX_DIM] = {
	{ 5.0, -1.0,  3.0,  3.0,  3.0,  3.0, -1.0,  5.0},
 
	{-1.0, -2.0, -1.0, -1.0, -1.0, -1.0, -2.0, -1.0},

	{ 3.0, -1.0,  1.0,  1.0,  1.0,  1.0, -1.0,  3.0},

	{ 3.0, -1.0,  1.0,  1.0,  1.0,  1.0, -1.0,  3.0},

	{ 3.0, -1.0,  1.0,  1.0,  1.0,  1.0, -1.0,  3.0},

	{ 3.0, -1.0,  1.0,  1.0,  1.0,  1.0, -1.0,  3.0},

	{-1.0, -2.0, -1.0, -1.0, -1.0, -1.0, -2.0, -1.0},

	{ 5.0, -1.0,  3.0,  3.0,  3.0,  3.0, -1.0,  5.0},

};

/*
 * Evaluate position 'state'
 */
double game_eval(const GAME_STATE state, CHIP_COLOR color) {
	double	eval = 0.0;
	int		x, y;	

	for (x = 0; x < MAX_DIM; x++) {
		for (y = 0; y < MAX_DIM; y++) {
			if (SAME_COLOR(state[x][y], color)) {
				eval += m_weight[x][y];
			} else if (!SAME_COLOR(state[x][y], COLOR_VACANT)) {
				eval -= m_weight[x][y];
			}
		}
	}
	return eval;
}

/*
 * Find best turn on position 'state' by color 'color'
 * Look ahead in 'depth' moves
 */
double find_best_turn(GAME_TURN *best_turn, 
				   const GAME_STATE state,
				   CHIP_COLOR color,
   				   int depth)
{
	GAME_TURN	t, adv_best_turn;
	GAME_STATE	tmp_state;
	int			turns_revised = 0;
	int			captured = 0;
	double		ascore, best_score;
	
	/* assume there are no turns */
	best_turn->x = -1;
	best_turn->y = -1;
	best_score = -99999999.9;
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
												 depth-1);
					} else {
						ascore = game_eval(tmp_state, color);
					}
					memcpy(tmp_state, state, sizeof(GAME_STATE));
					turns_revised++;
					if (ascore > best_score) {
						best_score = ascore;
						*best_turn = t;
					}
				}
			}
		}
	}
	if (!turns_revised) { /* No more turns */
		if (game_is_over(state)) {
			/* because - Game is over */
			best_score = (chips_count(state, color) - chips_count(state, ALTER_COLOR(color))) * 100.0;
		} else {
			/* try turn of the contendor */
			best_score = -find_best_turn(&adv_best_turn,
										 tmp_state,
										 ALTER_COLOR(color),
										 depth);
		}
	}
	return (best_score);
}
