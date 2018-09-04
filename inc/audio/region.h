/*
 * audio/region.h - A region in the timeline having a start
 *   and an end
 *
 * Copyright (C) 2018 Alexandros Theodotou
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

typedef struct _Position
{
  int       bar;
  int       beat;
  int       subbeat;
  int       tick;
} Position;

typedef struct _Region
{
  Position     start_pos;
  Position     end_pos;
} Region;

/**
 * Initializes given position to all 0
 */
void
init_position_to_zero (Position * position);

/**
 * Sets position to given bar
 */
void
set_position_to_bar (Position * position,
                      int        bar_no);