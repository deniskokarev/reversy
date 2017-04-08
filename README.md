# Player vs computer library for game of Reversy

Classic Minimax with Alpha-Beta pruning implementation (see https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning)

The code is suitable to run on Arduino or ARM microcontroller

* game.c - basic game functions suitable for player vs player mode
* minimax.c - find best turn using minimax with a/b pruning
* main.c - command-line tool to perform one turn on the given position
* reversy.sh - regression test tool (runs computer vs computer game loop)
