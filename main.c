#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "game.h"
#include "minimax.h"

void print_position(const GAME_STATE *state) {
	int		x, y;
	char	ch;

	for (y = 0; y < MAX_DIM; y++) {
		for (x = 0; x < MAX_DIM; x++) {
			switch ((*state)[x][y]) {
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

void read_position(GAME_STATE *state, FILE *fin) {
	int		x = 0, y = 0;
	char	ch;

	while (!feof(fin)) {
		ch = fgetc(fin);
		switch(ch) {
		case '+':
			(*state)[x++][y] = COLOR_WHITE;
			break;
		case '*':
			(*state)[x++][y] = COLOR_BLACK;
			break;
		case '.':
			(*state)[x++][y] = COLOR_VACANT;
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

int main(int argc, char *argv[]) {
	char		*fname = "";
	signed char		c;
	FILE		*fin;
	GAME_STATE	state;
	int			e_code = 0;

	char		m_scolor[16];
	GAME_TURN	turn = {0, 0, COLOR_VACANT};
	GAME_TURN	next_turn = {0, 0, COLOR_VACANT};
	GAME_TURN_HIST	turn_hist;

	opterr = 0;
	while((c = getopt(argc, argv, "f:m:l:")) > 0) {
		switch(c) {
		case 'f':
			fname = optarg;
			break;
		case 'm':
			if (sscanf(optarg, "%[+*]:%d:%d", m_scolor, &turn.x, &turn.y) != 3) {
				printf("Error in m: option, it should be [+|*]:x:y\n");
				exit(1);
			}
			turn.color = ((m_scolor[0] == '+') ? COLOR_WHITE : COLOR_BLACK);
			turn.x--;
			turn.y--;
			break;
		case 'l':
			if (sscanf(optarg, "%[+*]", m_scolor) != 1) {
				printf("Error in l: option, it should be [+|*]\n");
				exit(1);
			}
			next_turn.color = ((m_scolor[0] == '+') ? COLOR_WHITE : COLOR_BLACK);
			break;
		case '?':
			printf("Unknown option %c\n", c);
			exit(1);
			break;
		default:
		}
	}
	if (! fname[0]) {
		printf("File name must be specified\n");
		exit(1);
	}
	if ((fin = fopen(fname, "r"))) {
		read_position(&state, fin);
		print_position(&state);
	}
	if (turn.color != COLOR_VACANT) {
		printf("Making turn by '%s' to [%d:%d]\n", m_scolor, turn.x+1, turn.y+1);
		if ((e_code = validate_turn(&state, &turn))) {
			printf("Invalid turn. Error %d\n", e_code);
			exit(1);
		}
		printf("We captured %d chip(s)\n", game_turn(&state, &turn));
		print_position(&state);
	}
	if (next_turn.color != COLOR_VACANT) {
		log_turn(&turn_hist, &next_turn);
		while(get_next_turn(&next_turn, &state, &turn_hist)) {
			printf("[%d:%d]\n", next_turn.x+1, next_turn.y+1);
			log_turn(&turn_hist, &next_turn);
		}
	}
	return 0;
}
