/*
Copyright (c) 2018 Felipe Ferreira da Silva

This software is provided 'as-is', without any express or implied warranty. In
no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject to
the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim
     that you wrote the original software. If you use this software in a
     product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

Compiling with GCC:
  gcc test.c uastar.c -o test
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include "uastar.h"

#define CHARACTER_START_ON_PASSABLE 'S'
#define CHARACTER_START_ON_UNPASSABLE 's'
#define CHARACTER_END_ON_PASSABLE 'E'
#define CHARACTER_END_ON_UNPASSABLE 'e'
#define CHARACTER_PATH '*'
#define CHARACTER_CLOSED 'X'
#define CHARACTER_OPEN 'O'
#define CHARACTER_UNPASSABLE '#'
#define SLEEP_USECS 25000

uint32_t passable_chance;

static uint8_t fill_cb(struct path_finder *path_finder, int32_t col, int32_t row)
{
	uint8_t is_passable;
	is_passable = 0;
	/* Fill the map randomly with passable cells */
	if ((double)rand() / (double)RAND_MAX <= (double)passable_chance / 100.0) {
		is_passable = 1;
	}
	return is_passable;
}

static void print_map(struct path_finder *path_finder, uint8_t print_open_and_closed)
{
	int32_t row;
	int32_t col;
	printf("  Passable chance: %u\n", passable_chance);
	printf("            Start: \'%c\' (or \'%c\' if fall in a wall)\n", CHARACTER_START_ON_PASSABLE, CHARACTER_START_ON_UNPASSABLE);
	printf("              End: \'%c\' (or \'%c\' if fall in a wall)\n", CHARACTER_END_ON_PASSABLE, CHARACTER_END_ON_UNPASSABLE);
	printf("        Open path: \'%c\'\n", CHARACTER_OPEN);
	printf("      Closed path: \'%c\'\n", CHARACTER_CLOSED);
	printf("             Path: \'%c\'\n", CHARACTER_PATH);
	printf("       Unpassable: \'%c\'\n", CHARACTER_UNPASSABLE);
	printf("Map:\n");
	col = 0;
	while (col < path_finder->cols + 2) {
		printf("#");
		col = col + 1;
	}
	printf("\n");
	row = 0;
	while (row < path_finder->rows) {
		col = 0;
		printf("#");
		while (col < path_finder->cols) {
			if (path_finder_is_start(path_finder, col, row) == 1) {
				if (path_finder_is_passable(path_finder, col, row) == 1) {
					printf("%c", CHARACTER_START_ON_PASSABLE);
				} else {
					printf("%c", CHARACTER_START_ON_UNPASSABLE);
				}
			} else if (path_finder_is_end(path_finder, col, row) == 1) {
				if (path_finder_is_passable(path_finder, col, row) == 1) {
					printf("%c", CHARACTER_END_ON_PASSABLE);
				} else {
					printf("%c", CHARACTER_END_ON_UNPASSABLE);
				}
			} else if (path_finder_is_passable(path_finder, col, row) == 0) {
				printf("%c", CHARACTER_UNPASSABLE);
			} else if (path_finder_is_path(path_finder, col, row) == 1) {
				printf("%c", CHARACTER_PATH);
			} else if (print_open_and_closed == 1 && path_finder_is_open(path_finder, col, row) == 1 && path_finder_is_closed(path_finder, col, row) == 0) {
				printf("%c", CHARACTER_OPEN);
			} else if (print_open_and_closed == 1 && path_finder_is_closed(path_finder, col, row) == 1) {
				printf("%c", CHARACTER_CLOSED);
			} else {
				printf(" ");
			}
			col++;
		}
		printf("#\n");
		row++;
	}
	col = 0;
	while (col < path_finder->cols + 2) {
		printf("#");
		col = col + 1;
	}
	printf("\n");
	if (path_finder->has_path) {
		printf("A path was found!\n\n");
	} else {
		printf("No path was found!\n\n");
	}
}

int main(int argc, char **args)
{
	int32_t width;
	int32_t height;
	int32_t s_col;
	int32_t s_row;
	int32_t e_col;
	int32_t e_row;
	uint8_t show_progress;
	struct path_finder path_finder;
	if (argc != 10) {
		puts("Usage: test [show progress 0/1] [passable chance 0-100] [random seed integer] [start column] [start row] [end column] [end row] [width > 0] [height > 0]");
		goto done;
	}
	show_progress = atoi(args[1]);
	passable_chance = atoi(args[2]);
	srand(atoi(args[3]));
	s_col = atoi(args[4]);
	s_row = atoi(args[5]);
	e_col = atoi(args[6]);
	e_row = atoi(args[7]);
	width = atoi(args[8]);
	height = atoi(args[9]);
	if (width < 1 || height < 1 || height * width > PATH_FINDER_MAX_CELLS) {
		printf("Failed due width or height smaller than 1 or the number of cells (width * height) is larger than %u.\n", PATH_FINDER_MAX_CELLS);
		goto done;
	}
	if (s_col < 0 || s_col > width - 1
	|| e_col < 0 || e_col > width - 1
	|| s_row < 0 || s_row > height - 1
	|| e_row < 0 || e_row > height - 1) {
		puts("Invalid coordinates of start or end.");
		goto done;
	}
	path_finder_initialize(&path_finder);
	path_finder.cols = width;
	path_finder.rows = height;
	path_finder.fill_func = fill_cb;
	path_finder.score_func = NULL;
	path_finder_fill(&path_finder);
	path_finder_set_start(&path_finder, s_col, s_row);
	path_finder_set_end(&path_finder, e_col, e_row);
	if (show_progress == 0) {
		path_finder_find(&path_finder, NULL);
	} else {
		path_finder_begin(&path_finder);
		while (path_finder_find_step(&path_finder, NULL) == 1) {
			/* Print progress map */
			print_map(&path_finder, 1);
			usleep(SLEEP_USECS);
		}
	}
	/* Print final map */
	print_map(&path_finder, 0);
done:
	return EXIT_SUCCESS;
}
