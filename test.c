/*
Copyright (C) 2018 Felipe Ferreira da Silva

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
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include "uastar.h"

#define WIDTH 16
#define HEIGHT 16

double unpassable_percentage = 0.0;

static bool fill_cb(struct path_finder *pf, int32_t col, int32_t row, void *data)
{
	bool is_passable = true;
	/* Fill the map randomly with unpassable cells */
	if ((double)random() / (double)RAND_MAX <= unpassable_percentage) {
		is_passable = false;
	}
	return is_passable;
}

void print_map(struct path_finder *pf)
{
	int32_t row = 0;
	int32_t col = 0;
	printf("Map:\n");
	col = 0;
	printf("´=");
	while (col < pf->cols - 1) {
		printf("==");
		col++;
	}
	printf("==`\n");
	while (row < pf->rows) {
		col = 0;
		printf("|");
		while (col < pf->cols) {
			if (path_finder_is_start(pf, col, row)) {
				printf(" S");
			} else if (path_finder_is_end(pf, col, row)) {
				printf(" E");
			} else if (path_finder_is_path(pf, col, row)) {
				printf(" *");
			} else if (!path_finder_is_passable(pf, col, row)) {
				printf(" O");
			} else {
				printf("  ");
			}
			col++;
		}
		printf(" |\n");
		row++;
	}
	col = 0;
	printf("`=");
	while (col < pf->cols - 1) {
		printf("==");
		col++;
	}
	printf("==´\n");
	if (pf->has_path) {
		printf("A path was found!\n\n");
	} else {
		printf("No path was found!\n\n");
	}
}

int main(int argc, char **args)
{
	struct path_finder pf;
	srand(time(NULL));
	unpassable_percentage = ((double)random() / (double)RAND_MAX) / 3.0;
	printf("Unpassable percentage: %f.\n", unpassable_percentage);
	printf("                Start: S.\n");
	printf("                  End: E.\n");
	printf("                 Path: *.\n");
	printf("           Unpassable: O.\n\n");
	init_path_finder(&pf, WIDTH, HEIGHT);
	pf.fill_func = fill_cb; /* Callback to fill the initial state of the cells */
	path_finder_fill(&pf, NULL);
	path_finder_set_start(&pf, ((double)random() / (double)RAND_MAX) * (WIDTH - 1), ((double)random() / (double)RAND_MAX) * (HEIGHT - 1));
	path_finder_set_end(&pf, ((double)random() / (double)RAND_MAX) * (WIDTH - 1), ((double)random() / (double)RAND_MAX) * (HEIGHT - 1));
	path_finder_find(&pf, NULL);
	print_map(&pf);
	return EXIT_SUCCESS;
}
