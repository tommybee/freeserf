/*
 * freeserf.h - Various definitions.
 *
 * Copyright (C) 2013  Jon Lund Steffensen <jonlst@gmail.com>
 *
 * This file is part of freeserf.
 *
 * freeserf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freeserf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freeserf.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SRC_FREESERF_H_
#define SRC_FREESERF_H_

/* The length between game updates in miliseconds. */
#define TICK_LENGTH  20
#define TICKS_PER_SEC  (1000/TICK_LENGTH)

class game_t;

bool save_game(int autosave, game_t *game);

#endif  // SRC_FREESERF_H_
