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

#include <stddef.h>
#include "uastar.h"

static int32_t path_finder_heuristic(struct path_finder *path_finder, int32_t tile)
{
	int32_t tile_y;
	int32_t tile_x;
	int32_t end_y;
	int32_t end_x;
	int32_t dx;
	int32_t dy;
	tile_y = tile / path_finder->cols;
	tile_x = tile - (tile_y * path_finder->cols);
	end_y = path_finder->end / path_finder->cols;
	end_x = path_finder->end - (end_y * path_finder->cols);
	dx = 0;
	dy = 0;
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

static uint8_t path_finder_open_set_is_empty(struct path_finder *path_finder)
{
	uint8_t empty;
	int32_t i;
	empty = 1;
	i = 0;
	while (i < path_finder->cols * path_finder->rows && empty == 1) {
		if (path_finder->open_set[i] == 1) {
			empty = 0;
		}
		i = i + 1;
	}
	return empty;
}

static int32_t path_finder_lowest_in_open_set(struct path_finder *path_finder)
{
	int32_t lowest_f;
	int32_t current_lowest;
	int32_t i;
	lowest_f = path_finder->cols * path_finder->rows;
	current_lowest = 0;
	i = 0;
	while (i < path_finder->cols * path_finder->rows) {
		if (path_finder->open_set[i] == 1) {
			if (path_finder->f_score[i] < lowest_f) {
				lowest_f = path_finder->f_score[i];
				current_lowest = i;
			}
		}
		i = i + 1;
	}
	return current_lowest;
}

static void path_finder_reconstruct_path(struct path_finder *path_finder)
{
	int32_t i;
	i = path_finder->end;
	while (i != path_finder->start) {
		if (path_finder->parents[i] != path_finder->start) {
			path_finder->path[path_finder->parents[i]] = 1;
		}
		i = path_finder->parents[i];
	}
}

void path_finder_fill(struct path_finder *path_finder)
{
	int32_t row;
	row = 0;
	while (row < path_finder->rows) {
		int32_t col;
		col = 0;
		while (col < path_finder->cols) {
			path_finder->passables[row * path_finder->cols + col] = path_finder->fill_func(path_finder, col, row);
			path_finder->o_score[row * path_finder->cols + col] = 0;
			col = col + 1;
		}
		row = row + 1;
	}
}

void path_finder_find(struct path_finder *path_finder, void *data)
{
	uint8_t run;
	int32_t neighbors[4];
	int32_t current;
	current = 0;
	path_finder->open_set[path_finder->start] = 1;
	neighbors[0] = 0;
	neighbors[1] = 0;
	neighbors[2] = 0;
	neighbors[3] = 0;
	run = 1;
	while (run == 1) {
		current = path_finder_lowest_in_open_set(path_finder);
		if (current == path_finder->end) {
			path_finder_reconstruct_path(path_finder);
			run = 0;
			path_finder->has_path = 1;
		} else if (path_finder_open_set_is_empty(path_finder) == 1) {
			run = 0;
			path_finder->has_path = 0;
		} else {
			int32_t j;
			int32_t tmp_g_score;
			path_finder->open_set[current] = 0;
			path_finder->closed_set[current] = 1;
			/* Top */
			neighbors[0] = current - path_finder->cols;
			/* Right */
			if ((current + 1) % path_finder->cols == 0) {
				neighbors[1] = -1;
			} else {
				neighbors[1] = current + 1;
			}
			/* Bottom */
			neighbors[2] = current + path_finder->cols;
			/* Left */
			if (current % path_finder->cols == 0) {
				neighbors[3] = -1;
			} else {
				neighbors[3] = current - 1;
			}
			/* Neighbors */
			tmp_g_score = 0;
			j = 0;
			while (j < 4) {
				int32_t n;
				n = neighbors[j];
				if (n > -1 && n < path_finder->rows * path_finder->cols && path_finder->closed_set[n] == 0) {
					if (path_finder->passables[n] == 0) {
						path_finder->closed_set[n] = 1;
					} else {
						tmp_g_score = path_finder->g_score[current] + 1;
						if (path_finder->open_set[n] == 0 || tmp_g_score < path_finder->g_score[n]) {
							path_finder->parents[n] = current;
							path_finder->g_score[n] = tmp_g_score;
							path_finder->f_score[n] = path_finder->g_score[n] + path_finder_heuristic(path_finder, n);
							if (path_finder->score_func != NULL) {
								path_finder->f_score[n] = path_finder->f_score[n] + path_finder->score_func(path_finder, n / path_finder->cols, n % path_finder->cols, data);
							}
							path_finder->open_set[n] = 1;
						}
					}
				}
				j = j + 1;
			}
		}
	}
}

int32_t path_finder_get_score(struct path_finder *path_finder, int32_t col, int32_t row)
{
	return path_finder->o_score[row * path_finder->cols + col];
}

int32_t path_finder_get_heuristic_score(struct path_finder *path_finder, int32_t col, int32_t row)
{
	return path_finder->f_score[row * path_finder->cols + col];
}

uint8_t path_finder_is_passable(struct path_finder *path_finder, int32_t col, int32_t row)
{
	uint8_t result = 0;
	if (path_finder->passables[row * path_finder->cols + col] == 1) {
		result = 1;
	}
	return result;
}

uint8_t path_finder_is_path(struct path_finder *path_finder, int32_t col, int32_t row)
{
	uint8_t result;
	result = 0;
	if (path_finder->path[row * path_finder->cols + col] == 1) {
		result = 1;
	}
	return result;
}

uint8_t path_finder_is_start(struct path_finder *path_finder, int32_t col, int32_t row)
{
	uint8_t result;
	result = 0;
	if (row * path_finder->cols + col == path_finder->start) {
		result = 1;
	}
	return result;
}

uint8_t path_finder_is_end(struct path_finder *path_finder, int32_t col, int32_t row)
{
	uint8_t result;
	result = 0;
	if (row * path_finder->cols + col == path_finder->end) {
		result = 1;
	}
	return result;
}

void path_finder_set_path(struct path_finder *path_finder, int32_t col, int32_t row, uint8_t path)
{
	if (col >= 0 && col < path_finder->cols && row >= 0 && row < path_finder->rows) {
		path_finder->path[path_finder->start] = path;
	}
}

void path_finder_set_start(struct path_finder *path_finder, int32_t col, int32_t row)
{
	if (col >= 0 && col < path_finder->cols && row >= 0 && row < path_finder->rows) {
		path_finder->start = row * path_finder->cols + col;
	}
}

void path_finder_set_end(struct path_finder *path_finder, int32_t col, int32_t row)
{
	if (col >= 0 && col < path_finder->cols && row >= 0 && row < path_finder->rows) {
		path_finder->end = row * path_finder->cols + col;
	}
}

void path_finder_clear_score(struct path_finder *path_finder)
{
	int32_t i;
	i = 0;
	while (i < PATH_FINDER_MAX_CELLS) {
		path_finder->o_score[i] = 0;
		i = i + 1;
	}
}

void path_finder_clear_path(struct path_finder *path_finder)
{
	int32_t i;
	i = 0;
	while (i < PATH_FINDER_MAX_CELLS) {
		path_finder->open_set[i] = 0;
		path_finder->closed_set[i] = 0;
		path_finder->parents[i] = 0;
		path_finder->g_score[i] = 0;
		path_finder->f_score[i] = 0;
		path_finder->path[i] = 0;
		i = i + 1;
	}
	path_finder->has_path = 0;
}

void init_path_finder(struct path_finder *path_finder)
{
	int32_t i;
	i = 0;
	while (i < PATH_FINDER_MAX_CELLS) {
		path_finder->open_set[i] = 0;
		path_finder->closed_set[i] = 0;
		path_finder->parents[i] = 0;
		path_finder->g_score[i] = 0;
		path_finder->f_score[i] = 0;
		path_finder->o_score[i] = 0;
		path_finder->path[i] = 0;
		path_finder->passables[i] = 0;
		i = i + 1;
	}
	path_finder->rows = 0;
	path_finder->cols = 0;
	path_finder->start = 0;
	path_finder->end = 0;
	path_finder->has_path = 0;
}
