# Micro A Star Path Finder

This is a minimal A* path finder implementation in C:

- Fast.
- No dynamic memory allocation.

## Usage

The maximum size of the map is defined by the macro `PATH_FINDER_MAX_CELLS`, which can be modified to support larger maps.

The size of the map is defined by the variables `cols` and `rows` in the `path_finder` structure, and the total of cells must be smaller or equal to `PATH_FINDER_MAX_CELLS`.

```c
struct path_finder path_finder = {0};
path_finder.cols = 24;
path_finder.rows = 24;
init_path_finder(&path_finder);
path_finder.data = game;
/* Callback to fill the initial state of the cells */
path_finder.fill_func = fill_cb;
/* Callback to set the custom score to the cells while finding a path */
path_finder.score_func = score_cb;
/* Fill with the initial state of the cells and the custom score */
path_finder_fill(&path_finder, game_object);
path_finder_set_start(&path_finder, from_col, from_row);
path_finder_set_end(&path_finder, to_col, to_row);
/* Find the path */
path_finder_find(&path_finder, game_object);
```

The callback `fill_func` is necessary to define which cells are passable and which are non-passable:

```c
/* The parameters `col` and `row` indicate the cell of the map we are setting as passable or non-passable */
static bool fill_cb(struct path_finder *path_finder, int32_t col, int32_t row, void *data)
{
	struct game *game;
	struct game_object *game_object;
	bool is_passable;
	game = path_finder->data;
	game_object = data;
	is_passable = 0;
	if (is_wall(game, col, row) == 1) {
		is_passable = 0;
	}
	return is_passable;
}
```

The callback `score_func` is optional and is called during the `path_finder_find` execution. The callback also takes a custom pointer, useful to point to a specific object. Use it to add custom score to the cells:

```c
/* The parameters `col` and `row` indicate the cell of the map we are setting a score */
static int32_t score_cb(struct path_finder *path_finder, int32_t col, int32_t row, void *data)
{
	struct game *game;
	struct game_object *game_object;
	int32_t value;
	game = path_finder->data;
	game_object = data;
	value = 0;
	if (is_danger_zone(game, col, row) == 1) {
		/* The higher the value, the more avoided the cell is */
		value = 5;
		if (is_fearless(game_object) == 1) {
			/* Unless the character is fearless */
			value = 0;
		}
	}
	/* The value returned is incremented in the cell score */
	return value;
}
```

To check if a cell is a path, access the path finder array directly or use `path_finder_is_path` (be careful, this function doesn't check the passed values of column and row are inside the range of the map dimensions, as they should be):

```c
cell_is_path = path_finder_is_path(&path_finder, col, row);
```

If a path is found by the path finder, the value of `has_path` inside the structure `path_finder` is set to `1`.

## LICENSE

The source code of this project is licensed under the terms of the ZLIB license:

Copyright (C) 2017 Felipe Ferreira da Silva

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
