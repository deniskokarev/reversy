/**
 * @file
 * @brief Reversy Game backend for player vs computer mode
 *
 * Has classic miniax implementatoin with alpha/beta pruning.
 * @author Denis Kokarev
 */
#ifndef __MINIMAX_H__
#define __MINIMAX_H__

#include "game.h"
#include <limits.h>

/** our score will fit into 16bit val */
#define GAME_SCORE			short
#define GAME_SCORE_MAX		SHRT_MAX ///< 0x7FFF-1
#define GAME_SCORE_MIN		SHRT_MIN ///< 0x8000+1

/**
 * @brief run minimax search for the best turn
 *
 * Find best turn on position 'state' by side 'color'
 * Look ahead in 'depth' moves
 * It just invokes find_best_turn_intr()
 * @see find_best_turn_intr()
 * @param[out] best_turn - the best turn at the given position state
 * @param[in] state - given position
 * @param[in] color - side that will be making a turn
 * @param[in] depth - how many steps ahead should we traverse the search space
 * @return - the best potentially achievable score if both sides to play optimally
 */
GAME_SCORE find_best_turn(GAME_TURN *best_turn, 
					  const GAME_STATE *state,
					  CHIP_COLOR color,
					  int depth);

/**
 * @brief run flexible minimax search for the best turn
 *
 * Find best turn on position 'state' by side 'color'
 * Look ahead in 'depth' moves
 * Similar to find_best_turn(), but more flixible.
 * It observes the alpha/beta pruning by comparing the score with given simt score.
 * It can also perform some periodic auxiliary action during the search or abort the
 * search
 * @param[out] best_turn - the best turn at the given position state
 * @param[in] state - given position
 * @param[in] color - side that will be making a turn
 * @param[in] depth - how many steps ahead should we traverse the search space
 * @param[in] simt - max score to consider or just finish the search performing alpha/beta pruning. Start with GAME_SCORE_MAX
 * @param[in] is_stop - custom user function to be called at each search node.
 *	this way you can update the search status on the screen or handle the user abort
 * @param[in,out] is_stop_param - an argument to be passed to the custom user function
 * @return - the best potentially achievable score if both sides to play optimally
 */
GAME_SCORE find_best_turn_intr(GAME_TURN *best_turn,
							   const GAME_STATE *state,
							   CHIP_COLOR color,
							   int depth,
							   GAME_SCORE simt,
							   int (*is_stop)(int depth, void *param),
							   void *is_stop_param);

#endif /* #ifdnef __MINIMAX_H__ */
