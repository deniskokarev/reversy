#ifndef __MINIMAX_H__
#define __MINIMAX_H__

#include "game.h"

#define CUT_NO				0
#define CUT_MORE_THAN		1
#define CUT_LESS_THAN		-1

/*
 * Find best turn on position 'state' by color 'color'
 * Look ahead in 'depth' moves
 */
double find_best_turn(GAME_TURN *best_turn, 
					  const GAME_STATE state,
					  CHIP_COLOR color,
					  int depth);

#endif /* #ifdnef __MINIMAX_H__ */
