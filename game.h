#ifndef __GAME_H__
#define __GAME_H__

#define MAX_DIM			8

typedef enum { 
	COLOR_VACANT=0x0000,
	COLOR_WHITE=0x0001,
	COLOR_BLACK=0x0002} CHIP_COLOR;

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

/* 
 * We need to store only last turn to evaluate next one
 */
typedef GAME_TURN GAME_TURN_HIST;

/* invalid for OPPOSITE_COLOR(COLOR_VACANT, COLOR_VACANT)*/
#define SAME_COLOR(A, B)		(A == B)
#define OPPOSITE_COLOR(A, B)	(!SAME_COLOR(A, COLOR_VACANT) && !SAME_COLOR(B, COLOR_VACANT) && !SAME_COLOR(A, B))

#define E_OK		0
#define E_OCC		1
#define E_NO_OPP	2
#define E_NO_FLIPS	3

/*
 * Validate turn
 * returns
 * 0 - E_OK - Ok
 * 1 - E_OCC - is already occupied
 * 2 - E_NO_OPP - no opposite chips around
 * 3 - E_NO_FLIPS - no flips
 */
int validate_turn(const GAME_STATE *state, const GAME_TURN *turn);


/* 
 * Make turn on position 'state'.
 * Return amount of flip overs
 */
int game_turn(GAME_STATE *state, const GAME_TURN *turn);

/*
 * Turns generator
 * Pick next possible turn in game position state with turn history 'turn_hist'
 * returns 0 if no possible turns left
 */
int get_next_turn(GAME_TURN *turn, const GAME_STATE *state, const GAME_TURN_HIST *hist);

/*
 * Append turn to history
 */
void log_turn(GAME_TURN_HIST *hist, const GAME_TURN *turn);

#endif /* #ifndef __GAME_H__ */
