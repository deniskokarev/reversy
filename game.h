/**
 * @file
 * @brief Reversy Game backend for player vs player mode
 *
 * Contains all functions for basic game logic, such as validating and making turns
 * @author Denis Kokarev
 */
#ifndef __GAME_H__
#define __GAME_H__

/** board size */
#define MAX_DIM			8

/**
 * @brief it is very convenient to encode opposite sides colors as opposite numbers
 */
typedef signed char CHIP_COLOR;

#define	COLOR_VACANT	((CHIP_COLOR)0)		/**< unoccupied space */
#define	COLOR_POS		((CHIP_COLOR)1)		/**< WHITE is convenient to encode as positive */
#define	COLOR_NEG		((CHIP_COLOR)-1)	/**< BLACK is convenient to encode as negative */

/**
 * @brief This is a Reversy game field 8x8 grid
 *
 * First dimension is X (column) second dimension is Y (row)
 */
typedef struct {
	CHIP_COLOR b[MAX_DIM][MAX_DIM];
} GAME_STATE;
	 
/**
 * @brief Every turn has X (column) and Y (row) coordinates and the color
 */
typedef struct tagGAME_TURN {
	CHIP_COLOR		color;
	signed char		x, y;
} GAME_TURN;

#define SAME_COLOR(A, B)		(A == B)
#define ALTER_COLOR(A)			(-(A))

#define E_OK		0
#define E_OCC		1
#define E_NO_OPP	2
#define E_NO_FLIPS	3
#define E_NO_TURNS	4

/**
 * @brief get the list of all possible turns at the given position for the side color
 *
 * Turns generator. Don't use it in automatic procedures, because it is slow.
 * Makes a list of possible turns in game position 'state' by color 'color'
 * returns 0 if no possible turns left
 * @param[out] turn - 1D buffer capable of holding all possible turns at the position state with color color @see GAME_TURN
 * @param[in] state - current game position
 * @param[in] color - which side do we want to make turns for
 * @return numer of possible turns populated in turn buffer, where 0 means there is no possible turn for this color
 */
int make_turn_list(GAME_TURN turn[MAX_DIM * MAX_DIM], const GAME_STATE *state, CHIP_COLOR color);

/**
 * @brief Make a turn at the position 'state'
 *
 * Try to make a turn at the given board position. If the turn is legit perform all relevant chip captures
 * @param[in,out] state - game board gets updated if the turn is indeed legitimate
 * @param[in] turn - the coordinates and the color of the chip to be placed on the board
 * @return E_OK if the turn was valid, otherwise return E_* error
 */
int make_turn(GAME_STATE *state, const GAME_TURN *turn);

/**
 * @brief Validate turn without actually making it
 * @param[in] state - current board position
 * @param[in] turn - try to make this turn
 * @returns
 * E_OK - Ok
 * E_OCC - is already occupied
 * E_NO_FLIPS - no flips
 */
int validate_turn(const GAME_STATE *state, const GAME_TURN *turn);

/**
 * @brief Count chips of certain color on position 'state'
 * @param[in] state - current game board state
 * @param[in] color - count chips of the given color
 */
int chips_count(const GAME_STATE *state, CHIP_COLOR color);

/**
 * @brief Check if the game is over.
 *
 * Game is over when neither side can make a turn
 * @param[in] state - current game position
 */
int game_is_over(const GAME_STATE *state);

#endif /* #ifndef __GAME_H__ */
