#ifndef __GAME_H__
#define __GAME_H__

#define MAX_DIM			8

typedef enum { 
 	COLOR_VACANT =	0x0000,
 	COLOR_WHITE =	0x0001,
 	COLOR_BLACK =	0x0002
} CHIP_COLOR;

/*
 * This is a Reversy game field 8x8 grid
 */
typedef CHIP_COLOR GAME_STATE [MAX_DIM][MAX_DIM];

/*
 * Every turn has X and Y coordinates
 */
typedef struct tagGAME_TURN {
	CHIP_COLOR		color;
	int				x, y;
} GAME_TURN;

/* invalid for OPPOSITE_COLOR(COLOR_VACANT, COLOR_VACANT)*/
#define SAME_COLOR(A, B)		(A == B)
#define OPPOSITE_COLOR(A, B)	((A | B) == 0x0003)
#define ALTER_COLOR(A)			(A ^ 0x0003)

#define E_OK		0
#define E_OCC		1
#define E_NO_OPP	2
#define E_NO_FLIPS	3

/*
 * Turns generator. Don't use it in automatic procedures, because it is slow.
 * Makes a list of possible turns in game position 'state' by color 'color'
 * returns 0 if no possible turns left
 */
int make_turn_list(GAME_TURN turn[MAX_DIM * MAX_DIM], const GAME_STATE state, CHIP_COLOR color);

/* 
 * Make turn on position 'state'.
 * Return amount of flip overs
 */
int make_turn(GAME_STATE state, const GAME_TURN *turn);

/*
 * Make quick validation; sutable for automatic turn generation
 * returns. The final answer could be made only by flip_axises()
 * 0 - E_OK - Ok
 * otherwise - error
 */
int quick_validate_turn(const GAME_STATE state, const GAME_TURN *turn);

/*
 * Validate turn
 * Don't use it in automatic procedures, because it is slow.
 * returns
 * 0 - E_OK - Ok
 * 1 - E_OCC - is already occupied
 * 2 - E_NO_OPP - no opposite chips around
 * 3 - E_NO_FLIPS - no flips
 */
int validate_turn(const GAME_STATE state, const GAME_TURN *turn);

/*
 * Account chips of certain color on position 'state'
 */
int chips_count(const GAME_STATE state, CHIP_COLOR color);

int game_is_over(const GAME_STATE state);

#endif /* #ifndef __GAME_H__ */
