#ifndef __MINIMAX_H__
#define __MINIMAX_H__

#include "game.h"

#define	GAME_SCORE			short int
#define GAME_SCORE_MAX		0x7FFF-1
#define GAME_SCORE_MIN		0x8000+1

/*
 * Find best turn on position 'state' by color 'color'
 * Look ahead in 'depth' moves
 */
GAME_SCORE find_best_turn(GAME_TURN *best_turn, 
					  GAME_STATE state,
					  CHIP_COLOR color,
					  int depth);

/*
 * Similar to find_best_turn(), but with slightly more control:
 * Stop search if estimation simt (stop if more than) reached
 * The argument cuts branches which will not make sense for the final score evaluation -
 * the obviously loosing ones. In game theory this approach is called Alpha-Beta restrictions.
 * simt should be GAME_SCORE_MAX in manual invocation.
 * is_stop() is for user abort verification
 */
GAME_SCORE find_best_turn_intr(GAME_TURN *best_turn,
							   GAME_STATE state,
							   CHIP_COLOR color,
							   int depth,
							   GAME_SCORE simt,
							   int (*is_stop)(int depth, void *param),
							   void *is_stop_param);

#endif /* #ifdnef __MINIMAX_H__ */
