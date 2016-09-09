/*
 * text-input.cc - Text input GUI component
 *
 * Copyright (C) 2015  Wicked_Digger <wicked_digger@mail.ru>
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

#include "src/text-input.h"

#include <cstdlib>

text_input_t::text_input_t()
  : gui_object_t() {
  max_length = 0;
  filter = NULL;
  draw_focus = true;
  color_focus = 0;
  color_text = 31;
  color_background = 1;
}

void
text_input_t::set_text(const char *text) {
  this->text = text;
  set_redraw();
}

std::string
text_input_t::get_text() {
  return text;
}

void
text_input_t::internal_draw() {
  frame->fill_rect(0, 0, width, height, color_background);
  if (draw_focus && focused) {
    frame->draw_rect(0, 0, width, height, color_focus);
  }
  int ch_width = width/8;
  std::string str = text;
  int cx = 0;
  int cy = 0;
  if (draw_focus) {
    cx = 1;
    cy = 1;
  }
  while (str.length()) {
    std::string substr = str.substr(0, ch_width);
    str.erase(0, ch_width);
    frame->draw_string(cx, cy, color_text, 0, substr.c_str());
    cy += 8;
  }
}

bool
text_input_t::handle_click_left(int x, int y) {
  focused_object = this;
  focused = true;
  set_redraw();
  return true;
}

bool
text_input_t::handle_key_pressed(char key, int modifier) {
  if (!focused) {
    return false;
  }

  if ((max_length != 0) && (text.length() >= max_length)) {
    return true;
  }

  if ((key == '\b') && (text.length() > 0)) {
    text = text.substr(0, text.length() - 1);
    set_redraw();
    return true;
  }

  if (filter != NULL) {
    if (!filter(key, this)) {
      return true;
    }
  }

  text += key;

  set_redraw();

  return true;
}

bool
text_input_t::handle_focus_loose() {
  focused = false;
  set_redraw();
  return true;
}

void
text_input_t::set_max_length(unsigned int max_length) {
  this->max_length = max_length;
  if (max_length != 0) {
    if (text.length() > max_length) {
      text = text.substr(0, max_length);
    }
  }
}

