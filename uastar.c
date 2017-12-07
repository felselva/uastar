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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "uastar.h"

static int32_t path_finder_heuristic(struct path_finder *pf, int32_t tile)
{
	int32_t tile_y = tile / pf->cols;
	int32_t tile_x = tile - (tile_y * pf->cols);
	int32_t end_y = pf->end / pf->cols;
	int32_t end_x = pf->end - (end_y * pf->cols);
	int32_t dx = 0;
	int32_t dy = 0;
	if (tile_x > end_x) {
		dx = tile_x - end_x;
	} else {
		dx = end_x - tile_x;
	}
	if (tile_y > end_y) {
		dy = tile_y - end_y;
	} else {
		dy = end_y - tile_y;
	}
	return dx + dy;
}

static bool path_finder_open_set_is_empty(struct path_finder *pf)
{
	bool empty = true;
	int32_t i = 0;
	while (i < pf->cols * pf->rows) {
		if (pf->open_set[i]) {
			empty = false;
			goto done;
		}
		i++;
	}
done:
	return empty;
}

static int32_t path_finder_lowest_in_open_set(struct path_finder *pf)
{
	int32_t lowest_f = pf->cols * pf->rows;
	int32_t current_lowest = 0;
	int32_t i = 0;
	while (i < pf->cols * pf->rows) {
		if (pf->open_set[i]) {
			if (pf->f_score[i] < lowest_f) {
				lowest_f = pf->f_score[i];
				current_lowest = i;
			}
		}
		i++;
	}
	return current_lowest;
}

static void path_finder_reconstruct_path(struct path_finder *pf)
{
	int32_t t = pf->end;
	while (t != pf->start) {
		if (pf->parents[t] != pf->start) {
			pf->map[pf->parents[t]] = PATH_FINDER_PATH;
		}
		t = pf->parents[t];
	}
}

void path_finder_fill(struct path_finder *pf)
{
	int32_t row = 0;
	int32_t col = 0;
	while (row < pf->rows) {
		col = 0;
		while (col < pf->cols) {
			if (pf->map[row * pf->cols + col] != PATH_FINDER_START && pf->map[row * pf->cols + col] != PATH_FINDER_END) {
				if (pf->fill_func(pf, col, row)) {
					pf->map[row * pf->cols + col] = PATH_FINDER_PASSABLE;
				} else {
					pf->map[row * pf->cols + col] = PATH_FINDER_NON_PASSABLE;
				}
			}
			col++;
		}
		row++;
	}
}

void path_finder_find(struct path_finder *pf, void *data)
{
	bool run = true;
	int32_t neighbors[4] = {0};
	int32_t j = 0;
	int32_t n = 0;
	int32_t tmp_g_score = 0;
	int32_t current = 0;
	pf->open_set[pf->start] = true;
	while (run) {
		current = path_finder_lowest_in_open_set(pf);
		if (current == pf->end) {
			path_finder_reconstruct_path(pf);
			run = false;
			pf->has_path = true;
		} else if (path_finder_open_set_is_empty(pf)) {
			run = false;
			pf->has_path = false;
		} else {
			pf->open_set[current] = false;
			pf->closed_set[current] = true;
			/* Top */
			neighbors[0] = current - pf->cols;
			/* Right */
			if (((current + 1) % pf->cols) == 0) {
				neighbors[1] = -1;
			} else {
				neighbors[1] = current + 1;
			}
			/* Bottom */
			neighbors[2] = current + pf->cols;
			/* Left */
			if ((current % pf->cols) == 0) {
				neighbors[3] = -1;
			} else {
				neighbors[3] = current - 1;
			}
			j = 0;
			n = 0;
			tmp_g_score = 0;
			/* Neighbors */
			for (j = 0; j < 4; j++) {
				n = neighbors[j];
				if (n > -1 && n < pf->rows * pf->cols && !pf->closed_set[n]) {
					if (pf->map[n] == PATH_FINDER_NON_PASSABLE) {
						pf->closed_set[n] = true;
					} else {
						tmp_g_score = pf->g_score[current] + 1;
						if (!pf->open_set[n] || tmp_g_score < pf->g_score[n]) {
							pf->parents[n] = current;
							pf->g_score[n] = tmp_g_score;
							pf->f_score[n] = pf->g_score[n] + path_finder_heuristic(pf, n);
							if (pf->score_func) {
								pf->f_score[n] = pf->f_score[n] + pf->score_func(pf, n % pf->cols, n / pf->cols, data);
							}
							pf->open_set[n] = true;
						}
					}
				}
			}
		}
	}
}

bool path_finder_is_path(struct path_finder *pf, int32_t col, int32_t row)
{
	bool result = false;
	if (pf->map[row * pf->cols + col] == PATH_FINDER_PATH) {
		result = true;
	}
	return result;
}

bool path_finder_is_start(struct path_finder *pf, int32_t col, int32_t row)
{
	bool result = false;
	if (pf->map[row * pf->cols + col] == PATH_FINDER_START) {
		result = true;
	}
	return result;
}

bool path_finder_is_end(struct path_finder *pf, int32_t col, int32_t row)
{
	bool result = false;
	if (pf->map[row * pf->cols + col] == PATH_FINDER_END) {
		result = true;
	}
	return result;
}
void path_finder_set_start(struct path_finder *pf, int32_t col, int32_t row)
{
	pf->start = row * pf->cols + col;
	pf->map[pf->start] = PATH_FINDER_START;
}

void path_finder_set_end(struct path_finder *pf, int32_t col, int32_t row)
{
	pf->end = row * pf->cols + col;
	pf->map[pf->end] = PATH_FINDER_END;
}

bool init_path_finder(struct path_finder *pf, int32_t cols, int32_t rows)
{
	bool result = true;
	int32_t i = 0;
	if (cols > MAX_COLS || rows > MAX_ROWS || cols < 1 || rows < 1) {
		result = false;
		goto done;
	}
	while (i < MAX_COLS * MAX_ROWS) {
		pf->open_set[i] = false;
		pf->closed_set[i] = false;
		pf->parents[i] = 0;
		pf->g_score[i] = 0;
		pf->f_score[i] = 0;
		pf->map[i] = PATH_FINDER_PASSABLE;
		i++;
	}
	pf->rows = rows;
	pf->cols = cols;
	pf->start = 0;
	pf->end = 0;
	pf->has_path = false;
	pf->fill_func = NULL;
	pf->score_func = NULL;
done:
	return result;
}
