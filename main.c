#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "game.h"
#include "minimax.h"

char color2abbr(CHIP_COLOR color) {
	static char abbr[3] = { '.', '+', '*' };
	return abbr[color % 4];
}

CHIP_COLOR abbr2color(char abbr) {
	return (abbr == '.' ? COLOR_VACANT : (abbr == '+' ? COLOR_WHITE : COLOR_BLACK));
}

void print_position(const GAME_STATE state) {
	int		x, y;

	for (y = 0; y < MAX_DIM; y++) {
		for (x = 0; x < MAX_DIM; x++) {
			printf(" %c", color2abbr(state[x][y]));
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
		case '*':
		case '.':
			state[x++][y] = abbr2color(ch);
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

void usage(FILE *fout, char *cmd_name) {
	fprintf(fout, "Game \"Reversy\"\n\
Reads game position from file or standard input and\n\
prints new position to standard output. Optionally makes\n\
log file with performed operations.\n\
Usage: %s [-i <file>] [-c <color>] [-p <x:y>] [-l <file>]\n\
\t[-i <file>] read startup game position; - means stdin\n\
\t[-c <color>] whose turn is now? ('+' by default)\n\
\t[-p <x:y>] make turn at position x:y by color given in -c parameter
\t\tif no -p was given computer will make a turn.
\t[-l <file>] write turn to log
<color> can be '+' or '*'\n\
<file> with position should have 8 rows with 8 space delimetered fields\n\
'+' or '*' chips, or '.' for vacant place\n", cmd_name); 
}

int main(int argc, char *argv[]) {
	char		*fin_name = "", *flog_name = "/dev/null";
	char		scolor[2];
	char		c;
	FILE		*fin, *flog;
	GAME_STATE	state;
	GAME_TURN	turn = {0, 0, COLOR_VACANT};
	int			comp_turn = 1;
	int			e_code = 0;

	CHIP_COLOR	color = COLOR_WHITE;

	opterr = 0;
	while((c = getopt(argc, argv, "i:c:p:l:")) > 0) {
		switch(c) {
		case 'i':
			fin_name = optarg;
			break;
		case 'c':
			if (sscanf(optarg, "%[+*]", scolor) != 1) {
				fprintf(stderr, "Error in [c] option, it should be -c <+|*>, you've entered %s\n", optarg);
				exit(1);
			}
			color = abbr2color(scolor[0]);
			turn.color = color;
			break;
		case 'p':
			if ((sscanf(optarg, "%d:%d", &turn.x, &turn.y) != 2)
				|| (turn.x < 1)
				|| (turn.x > MAX_DIM)
				|| (turn.y < 1)
				|| (turn.y > MAX_DIM)) {
				fprintf(stderr, "Error in [p] option, it should be -p <pos_x:pos_y>,\n");
				fprintf(stderr, "\twhere 1 <= pos <= %d\n", MAX_DIM);
				fprintf(stderr, "You've entered %s\n", optarg);
				exit(1);
			}
			turn.x --;
			turn.y --;
			comp_turn = 0;
			break;
		case 'l':
			flog_name = optarg;
			break;
		case 'h':
			usage(stdout, argv[0]);
			exit(0);
			break;
		case '?':
			fprintf(stderr, "Unknown option %c\n", c);
			usage(stderr, argv[0]);
		default:
			exit(1);
		}
	}
	if (!(flog = fopen(flog_name, "a"))) {
		perror("Can't open log file");
		exit(1);
	}
	if (fin_name[0]) {
		if (fin_name[0] == '-') {
			fin = stdin;
		} else {
			if ((fin = fopen(fin_name, "r"))) {
				fprintf(flog, "Reading from %s ...\n", fin_name);
				read_position(state, fin);
			} else {
				perror("Can't open game position file");
				exit(1);
			}
		}
	} else {
		fprintf(flog, "Game started from the scratch\n");
		memset(state, COLOR_VACANT, sizeof(GAME_STATE));
		state[3][3] = COLOR_BLACK;
		state[3][4] = COLOR_WHITE;
		state[4][4] = COLOR_BLACK;
		state[4][3] = COLOR_WHITE;
	}
	if (comp_turn) {
		fprintf(flog, "Thinking...\n");
		find_best_turn(&turn, state, color, 5, GAME_SCORE_MAX);
		if (turn.x < 0) {
			fprintf(flog, "There are no turns by color '%c'\n", color2abbr(color));
			print_position(state);
			exit(254);
		}
	}
	fprintf(flog, "Making turn by '%c' to [%d:%d]\n", color2abbr(turn.color), turn.x+1, turn.y+1);
	if ( (e_code = make_turn(state, &turn)) ) {
		fprintf(flog, "We captured %d chip(s)\n", e_code);
		print_position(state);
	} else {
		fprintf(flog, "Invalid turn.\n");
		exit(1);
	}
	if (game_is_over(state)) {
		printf("Game is over\n");
		fprintf(flog, "Game is over\n");
		exit(255);
	}
	return 0;
}
