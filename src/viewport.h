/*
 * viewport.h - Viewport GUI component
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

#ifndef SRC_VIEWPORT_H_
#define SRC_VIEWPORT_H_

#include <map>

#include "src/gui.h"
#include "src/map.h"
#include "src/building.h"
#include "src/serf.h"

typedef enum {
  VIEWPORT_LAYER_LANDSCAPE = 1<<0,
  VIEWPORT_LAYER_PATHS = 1<<1,
  VIEWPORT_LAYER_OBJECTS = 1<<2,
  VIEWPORT_LAYER_SERFS = 1<<3,
  VIEWPORT_LAYER_CURSOR = 1<<4,
  VIEWPORT_LAYER_GRID = 1<<5,
  VIEWPORT_LAYER_BUILDS = 1<<6,
  VIEWPORT_LAYER_ALL = (VIEWPORT_LAYER_LANDSCAPE |
                        VIEWPORT_LAYER_PATHS |
                        VIEWPORT_LAYER_OBJECTS |
                        VIEWPORT_LAYER_SERFS |
                        VIEWPORT_LAYER_CURSOR),
} viewport_layer_t;

class interface_t;
class data_source_t;

class viewport_t : public gui_object_t, public update_map_height_handler_t {
 protected:
  /* Cache prerendered tiles of the landscape. */
  typedef std::map<unsigned int, frame_t*> tiles_map_t;
  tiles_map_t landscape_tiles;

  int offset_x, offset_y;
  unsigned int layers;
  interface_t *interface;
  unsigned int last_tick;
  data_source_t *data_source;

  map_t *map;

 public:
  viewport_t(interface_t *interface, map_t *map);
  virtual ~viewport_t();

  void switch_layer(viewport_layer_t layer) { layers ^= layer; }

  void move_to_map_pos(map_pos_t pos);
  void move_by_pixels(int x, int y);
  map_pos_t get_current_map_pos();

  void screen_pix_from_map_pix(int mx, int my, int *sx, int *sy);
  void map_pix_from_map_coord(map_pos_t pos, int h, int *mx, int *my);
  map_pos_t map_pos_from_screen_pix(int x, int y);

  void redraw_map_pos(map_pos_t pos);

  void update();

 protected:
  void draw_triangle_up(int x, int y, int m, int left, int right, map_pos_t pos,
                        frame_t *frame);
  void draw_triangle_down(int x, int y, int m, int left, int right,
                          map_pos_t pos, frame_t *frame);
  void draw_up_tile_col(map_pos_t pos, int x_base, int y_base, int max_y,
                        frame_t *frame);
  void draw_down_tile_col(map_pos_t pos, int x_base, int y_base, int max_y,
                          frame_t *frame);
  void draw_landscape();
  void draw_path_segment(int x, int y, map_pos_t pos, dir_t dir);
  void draw_border_segment(int x, int y, map_pos_t pos, dir_t dir);
  void draw_paths_and_borders();
  void draw_game_sprite(int x, int y, int index);
  void draw_serf(int x, int y, unsigned char color, int head, int body);
  void draw_shadow_and_building_sprite(int x, int y, int index);
  void draw_shadow_and_building_unfinished(int x, int y, int index,
                                           int progress);
  void draw_building_unfinished(building_t *building, building_type_t bld_type,
                                int x, int y);
  void draw_unharmed_building(building_t *building, int x, int y);
  void draw_burning_building(building_t *building, int x, int y);
  void draw_building(map_pos_t pos, int x, int y);
  void draw_water_waves(map_pos_t pos, int x, int y);
  void draw_water_waves_row(map_pos_t pos, int y_base, int cols, int x_base);
  void draw_flag_and_res(map_pos_t pos, int x, int y);
  void draw_map_objects_row(map_pos_t pos, int y_base, int cols, int x_base);
  void draw_row_serf(int x, int y, int shadow, int color, int body);
  int serf_get_body(serf_t *serf);
  void draw_active_serf(serf_t *serf, map_pos_t pos, int x_base, int y_base);
  void draw_serf_row(map_pos_t pos, int y_base, int cols, int x_base);
  void draw_serf_row_behind(map_pos_t pos, int y_base, int cols, int x_base);
  void draw_game_objects(int layers);
  void draw_map_cursor_sprite(map_pos_t pos, int sprite);
  void draw_map_cursor_possible_build();
  void draw_map_cursor();
  void draw_base_grid_overlay(int color);
  void draw_height_grid_overlay(int color);

  virtual void internal_draw();
  virtual void layout();
  virtual bool handle_click_left(int x, int y);
  virtual bool handle_dbl_click(int x, int y, event_button_t button);
  virtual bool handle_drag(int x, int y);

  frame_t *get_tile_frame(unsigned int tid, int tc, int tr);

 public:
  void changed_height(map_pos_t pos);
};

#endif  // SRC_VIEWPORT_H_
