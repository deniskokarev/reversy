#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "game.h"
#include "minimax.h"

void print_position(const GAME_STATE state) {
	int		x, y;
	char	ch;

	for (y = 0; y < MAX_DIM; y++) {
		for (x = 0; x < MAX_DIM; x++) {
			switch (state[x][y]) {
			case COLOR_WHITE:
				ch = '+';
				break;
			case COLOR_BLACK:
				ch = '*';
				break;
			case COLOR_VACANT:
			default:
				ch = '.';
			}
			printf(" %c", ch);
		}
		printf("\n");
	}
}

void read_position(GAME_STATE state, FILE *fin) {
	int		x = 0, y = 0;
	char	ch;

	while (!feof(fin)) {
		ch = fgetc(fin);
		switch(ch) {
		case '+':
			state[x++][y] = COLOR_WHITE;
			break;
		case '*':
			state[x++][y] = COLOR_BLACK;
			break;
		case '.':
			state[x++][y] = COLOR_VACANT;
			break;
		case '\n':
			y++;
			x = 0;
			break;
		case EOF:
			return;
		default:
		}
	}
}

void usage(char *cmd_name) {
	printf("Game \"Reversy\"\n\
Usage: %s [-r <file> [-t <first turn color>]] [-c <your color>]\n\
\t[-r <file>] read startup game position\n\
\t[-t <first turn color>] who's moving first\n\
\t[-c <your color>] your chips color\n\
<color> can be '+' or '*'\n\
<file> wiht startup position should have 8 rows with 8 space delimetered fields\n\
'+' or '*' chips, or '.' for vacant place\n\
This is interactive program the input command list is:\n\
<x>:<y> - make turn at x:y position\n\
write:<file name> - write current position to file\n\
exit - exit the game\n", cmd_name); 
}

int main(int argc, char *argv[]) {
	char		*fname = "";
	char		command[32];
	signed char		c;
	FILE		*fin;
	GAME_STATE	state;
	GAME_TURN	turn;
	GAME_TURN	turn_list[MAX_DIM * MAX_DIM];
	int			e_code = 0;
	int			i;

	char		scolor[2];
	CHIP_COLOR	c_who_is_first = COLOR_BLACK;
	CHIP_COLOR	c_computer = COLOR_WHITE;
	CHIP_COLOR	c_user = COLOR_BLACK;

	opterr = 0;
	while((c = getopt(argc, argv, "r:t:c:")) > 0) {
		switch(c) {
		case 'r':
			fname = optarg;
			break;
		case 't':
			if (sscanf(optarg, "%[+*]", scolor) != 1) {
				printf("Error in t: option, it should be -t <+|*>\n");
				exit(1);
			}
			c_who_is_first = ((scolor[0] == '+') ? COLOR_WHITE : COLOR_BLACK);
			break;
		case 'c':
			if (sscanf(optarg, "%[+*]", scolor) != 1) {
				printf("Error in l: option, it should be -c <+|*>\n");
				exit(1);
			}
			c_user = ((scolor[0] == '+') ? COLOR_WHITE : COLOR_BLACK);
			c_computer = ((scolor[0] == '+') ? COLOR_BLACK : COLOR_WHITE);
			break;
		case '?':
			printf("Unknown option %c\n", c);
		case 'h':
			usage(argv[0]);
		default:
			exit(1);
		}
	}
	if (fname[0]) {
		if ((fin = fopen(fname, "r"))) {
			printf("Reading from %s ...\n", fname);
			read_position(state, fin);
		} else {
			perror("Can't open game position file");
			exit(1);
		}
	} else {
		memset(state, COLOR_VACANT, sizeof(GAME_STATE));
		state[3][3] = COLOR_BLACK;
		state[3][4] = COLOR_WHITE;
		state[4][4] = COLOR_BLACK;
		state[4][3] = COLOR_WHITE;
	}
	printf("Starting position is:\n");
	print_position(state);
	turn.color = c_who_is_first;
	do {
		if (game_is_over(state)) {
			printf("Game is over\n");
			exit(0);
		}
		printf("your command>");
		fflush(stdin);
		scanf("%32s\n", command);
		switch(command[0]) {
		case 'e':
			/* exit */
			printf("Bye\n");
			exit(0);
		case 'p':
			/* possible turns */
			printf("Possible turns by color %d\n", turn.color);
			for (i=make_turn_list(turn_list, state, turn.color)-1; i>=0; i--) {
				printf("[%d:%d]\n", turn_list[i].x+1, turn_list[i].y+1);
			}
			break;
		default:
			if (sscanf(command, "%d:%d", &turn.x, &turn.y) != 2) {
				printf("Thinking...\n");
				find_best_turn(&turn, state, turn.color, 4);
				if (turn.x < 0) {
					printf("There are no turns by color %d\n", turn.color);
					turn.color = (turn.color == COLOR_WHITE) ? COLOR_BLACK : COLOR_WHITE;
					continue;
				}
			} else {
				turn.x --;
				turn.y --;
			}
			printf("Making turn by '%d' to [%d:%d]\n", turn.color, turn.x+1, turn.y+1);
			if (! (e_code = validate_turn(state, &turn))) {
				printf("We captured %d chip(s)\n", make_turn(state, &turn));
				print_position(state);
				turn.color = (turn.color == COLOR_WHITE) ? COLOR_BLACK : COLOR_WHITE;
			} else {
				printf("Invalid turn. Error %d\n", e_code);
			}
			fflush(stdout);
		}
	} while(!feof(stdin));
	return 0;
}
