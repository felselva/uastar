/*
Copyright (C) 2017 Felipe Ferreira da Silva

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

#ifndef UASTAR_H
#define UASTAR_H

#include <stdint.h>

#ifndef PATH_FINDER_MAX_CELLS
#define PATH_FINDER_MAX_CELLS 1024
#endif

struct path_finder {
	int32_t cols;
	int32_t rows;
	int32_t start;
	int32_t end;
	uint8_t has_path;
	uint8_t open_set[PATH_FINDER_MAX_CELLS];
	uint8_t closed_set[PATH_FINDER_MAX_CELLS];
	int32_t parents[PATH_FINDER_MAX_CELLS];
	int32_t g_score[PATH_FINDER_MAX_CELLS];
	int32_t f_score[PATH_FINDER_MAX_CELLS];
	int32_t o_score[PATH_FINDER_MAX_CELLS];
	uint8_t path[PATH_FINDER_MAX_CELLS];
	uint8_t passables[PATH_FINDER_MAX_CELLS];
	uint8_t (*fill_func)(struct path_finder *path_finder, int32_t col, int32_t row);
	int32_t (*score_func)(struct path_finder *path_finder, int32_t col, int32_t row, void *data);
	void *data;
};

int32_t path_finder_get_score(struct path_finder *path_finder, int32_t col, int32_t row);
int32_t path_finder_get_heuristic_score(struct path_finder *path_finder, int32_t col, int32_t row);
uint8_t path_finder_is_possible_target(struct path_finder *path_finder, int32_t col, int32_t row);
uint8_t path_finder_is_passable(struct path_finder *path_finder, int32_t col, int32_t row);
uint8_t path_finder_is_path(struct path_finder *path_finder, int32_t col, int32_t row);
uint8_t path_finder_is_start(struct path_finder *path_finder, int32_t col, int32_t row);
uint8_t path_finder_is_end(struct path_finder *path_finder, int32_t col, int32_t row);
void path_finder_find(struct path_finder *path_finder, void *data);
void path_finder_set_path(struct path_finder *path_finder, int32_t col, int32_t row, uint8_t path);
void path_finder_set_start(struct path_finder *path_finder, int32_t col, int32_t row);
void path_finder_set_end(struct path_finder *path_finder, int32_t col, int32_t row);
void path_finder_clear_score(struct path_finder *path_finder);
void path_finder_clear_path(struct path_finder *path_finder);
void path_finder_fill(struct path_finder *path_finder);
void init_path_finder(struct path_finder *path_finder);

#endif
