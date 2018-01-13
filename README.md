# Micro A Star Path Finder

This is a minimal A* path finder implementation in C:

- Fast.
- No memory dynamic allocation.

## Usage

The maximum size of the map is defined by `MAX_COLS` and `MAX_ROWS`. Those are already declared in the header `uastar.h`, but can be modified to support a larger map.

```c
struct path_finder pf = {0};
init_path_finder(&pf, cols, rows);
pf.data = game;
pf.fill_func = fill_cb; /* Callback to fill the initial state of the cells */
pf.score_func = score_cb; /* Callback to add custom score to the path */
path_finder_fill(&pf);
path_finder_set_start(&pf, from_col, from_row);
path_finder_set_end(&pf, to_col, to_row);
path_finder_find(&pf, game_object);
```

The initialization (`init_path_finder`) must take a number of columns and rows smaller than or equal to `MAX_COLS` and `MAX_ROWS`.

The callback `fill_func` is necessary to define which cells are passable and which are non-passable:

```c
/* The parameters `col` and `row` indicate the cell of the map we are setting as passable or non-passable */
static bool fill_cb(struct path_finder *pf, int32_t col, int32_t row)
{
	struct game *game = pf->data;
	bool is_passable = true;
	if (is_wall(game, col, row)) {
		is_passable = false;
	}
	return is_passable;
}
```

The callback `score_func` is optional and is called during the `path_finder_find` execution. The callback also takes a custom pointer, useful to point to a specific object. Use it to add custom weight to the cells:

```c
/* The parameters `col` and `row` indicate the cell of the map we are setting a score */
static int32_t score_cb(struct path_finder *pf, int32_t col, int32_t row, void *data)
{
	struct game *game = pf->data;
	struct game_object *game_object = data;
	int32_t value = 0;
	if (is_danger_zone(game, col, row)) {
		value = 5; /* The higher the value, more avoided the cell is */ 
		if (is_fearless(game_object)) {
			value = 0;
		}
	}
	return value; /* Value incremented in the cell score */
}
```

To check if a cell is a path, access the path finder array directly or use `path_finder_is_path` (be careful, this function doesn't check the passed values of column and row are inside the range of the map dimensions):

```c
bool is_cell_path = path_finder_is_path(&pf, col, row);
```

If a path is found, the value of `has_path` inside the structure `path_finder` is set to `true`.

## LICENSE

The source code of this project is licensed under the terms of the ZLIB license:

Copyright (C) 2017 Felipe Ferreira da Silva

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
