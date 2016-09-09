/*
 * video-sdl.h - SDL graphics rendering
 *
 * Copyright (C) 2012-2015  Jon Lund Steffensen <jonlst@gmail.com>
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

#ifndef SRC_VIDEO_SDL_H_
#define SRC_VIDEO_SDL_H_

#include <exception>
#include <string>

#include <SDL.h>

#include "src/video.h"

class video_frame_t {
 public:
  SDL_Texture *texture;

  video_frame_t() : texture(NULL) {}
};

class video_image_t {
 public:
  unsigned int w;
  unsigned int h;
  SDL_Texture *texture;

  video_image_t() : w(0), h(0), texture(NULL) {}
};

class SDL_Exception : public Video_Exception {
 protected:
  std::string sdl_error;

 public:
  explicit SDL_Exception(const std::string &description) throw();
  virtual ~SDL_Exception() throw() {}

  virtual std::string get_platform() const { return "SDL"; }
};

class video_sdl_t : public video_t {
 protected:
  static int bpp;
  static Uint32 Rmask;
  static Uint32 Gmask;
  static Uint32 Bmask;
  static Uint32 Amask;
  static Uint32 pixel_format;

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *screen_texture;
  video_frame_t *screen;
  bool fullscreen;
  SDL_Cursor *cursor;
  float zoom_factor;

 public:
  video_sdl_t() throw(Video_Exception);
  virtual ~video_sdl_t();

  virtual void set_resolution(unsigned int width, unsigned int height,
                              bool fullscreen) throw(Video_Exception);
  virtual void get_resolution(unsigned int *width, unsigned int *height);
  virtual void set_fullscreen(bool enable) throw(Video_Exception);
  virtual bool is_fullscreen();

  virtual video_frame_t *get_screen_frame();
  virtual video_frame_t *create_frame(unsigned int width, unsigned int height);
  virtual void destroy_frame(video_frame_t *frame);

  virtual video_image_t *create_image(void *data, unsigned int width,
                                      unsigned int height);
  virtual void destroy_image(video_image_t *image);

  virtual void warp_mouse(int x, int y);

  virtual void draw_image(const video_image_t *image, int x, int y,
                           int y_offset, video_frame_t *dest);
  virtual void draw_frame(int dx, int dy, video_frame_t *dest, int sx, int sy,
                          video_frame_t *src, int w, int h);
  virtual void draw_rect(int x, int y, unsigned int width, unsigned int height,
                         const video_color_t color, video_frame_t *dest);
  virtual void fill_rect(int x, int y, unsigned int width, unsigned int height,
                         const video_color_t color, video_frame_t *dest);
  virtual void swap_buffers();

  virtual void set_cursor(void *data, unsigned int width, unsigned int height);

  virtual float get_zoom_factor() { return zoom_factor; }
  virtual bool set_zoom_factor(float factor);

 protected:
  SDL_Surface *create_surface(int width, int height);
  SDL_Surface *create_surface_from_data(void *data, int width, int height);
  SDL_Texture *create_texture(int width, int height);
  SDL_Texture *create_texture_from_data(void *data, int width, int height);
};

#endif  // SRC_VIDEO_SDL_H_
