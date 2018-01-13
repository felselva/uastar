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

#include <stdbool.h>
#include <stdint.h>

#ifndef MAX_COLS
#define MAX_COLS 32
#endif
#ifndef MAX_ROWS
#define MAX_ROWS 32
#endif

#define PATH_FINDER_PASSABLE 0
#define PATH_FINDER_NON_PASSABLE 1
#define PATH_FINDER_START 2
#define PATH_FINDER_END 3
#define PATH_FINDER_PATH 4

struct path_finder {
	int32_t cols;
	int32_t rows;
	int32_t start;
	int32_t end;
	bool has_path;
	bool open_set[MAX_COLS * MAX_ROWS];
	bool closed_set[MAX_COLS * MAX_ROWS];
	int32_t parents[MAX_COLS * MAX_ROWS];
	int32_t g_score[MAX_COLS * MAX_ROWS];
	int32_t f_score[MAX_COLS * MAX_ROWS];
	int32_t map[MAX_COLS * MAX_ROWS];
	bool (*fill_func)(struct path_finder *pf, int32_t col, int32_t row, void *data);
	int32_t (*score_func)(struct path_finder *pf, int32_t col, int32_t row, void *data);
	void *data;
};

void path_finder_fill(struct path_finder *pf, void *data);
void path_finder_find(struct path_finder *pf, void *data);
int32_t path_finder_score(struct path_finder *pf, int32_t col, int32_t row);
bool path_finder_is_path(struct path_finder *pf, int32_t col, int32_t row);
bool path_finder_is_start(struct path_finder *pf, int32_t col, int32_t row);
bool path_finder_is_end(struct path_finder *pf, int32_t col, int32_t row);
void path_finder_set_start(struct path_finder *pf, int32_t col, int32_t row);
void path_finder_set_end(struct path_finder *pf, int32_t col, int32_t row);
bool init_path_finder(struct path_finder *pf, int32_t cols, int32_t rows);

#endif
