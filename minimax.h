#ifndef __MINIMAX_H__
#define __MINIMAX_H__

#include "game.h"
#include <limits.h>

#define	GAME_SCORE			int
#define GAME_SCORE_MAX		INT_MAX-1
#define GAME_SCORE_MIN		INT_MIN+1

/* Use Alpha-Beta restrictions (several times faster) */
#define ALPHA_BETA_CUT_ON

/*
 * Find best turn on position 'state' by color 'color'
 * Look ahead in 'depth' moves
 * Stop search if estimation simt (stop if more than) reached
 * The last argument lets to cut branches which will not
 * make sense for the final score evaluation, the obviously loosing
 * ones. In game theory this approach is called Alpha-Beta restrictions. 
 */
GAME_SCORE find_best_turn(GAME_TURN *best_turn, 
					  const GAME_STATE state,
					  CHIP_COLOR color,
					  int depth,
					  GAME_SCORE simt);

#endif /* #ifdnef __MINIMAX_H__ */
