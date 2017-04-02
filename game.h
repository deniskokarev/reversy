#ifndef __GAME_H__
#define __GAME_H__

#define MAX_DIM			8

#ifndef XMEMCPY
/* inline memcpy(void *tgt, void *src, int nsize) function */
#define XMEMCPY(T, S, N) { \
	int i; \
	char *t1, *s1; \
	long long *t8 = (long long*)T, *s8 = (long long*)S; \
	for(i=0; i<(N>>3); i++) \
		*t8++ = *s8++; \
	t1 = (char*)t8; \
	s1 = (char*)s8; \
	for(i<<=3; i<N; i++) \
		*t1++ = *s1++; \
}
#endif

typedef signed char CHIP_COLOR;

#define	COLOR_VACANT	((CHIP_COLOR)0)
#define	COLOR_POS		((CHIP_COLOR)1)
#define	COLOR_NEG		((CHIP_COLOR)-1)

/*
 * This is a Reversy game field 8x8 grid
 */
typedef CHIP_COLOR GAME_STATE [MAX_DIM][MAX_DIM];

/*
 * Every turn has X and Y coordinates
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

/*
 * Turns generator. Don't use it in automatic procedures, because it is slow.
 * Makes a list of possible turns in game position 'state' by color 'color'
 * returns 0 if no possible turns left
 */
int make_turn_list(GAME_TURN turn[MAX_DIM * MAX_DIM], GAME_STATE state, CHIP_COLOR color);

/* 
 * Make turn on position 'state'.
 */
int make_turn(GAME_STATE state, GAME_TURN *turn);

/*
 * Validate turn
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
