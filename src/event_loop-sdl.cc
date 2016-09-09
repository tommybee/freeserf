/*
 * event_loop-sdl.cc - User and system events handling
 *
 * Copyright (C) 2012-2014  Jon Lund Steffensen <jonlst@gmail.com>
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

#include "src/event_loop-sdl.h"

#include <SDL.h>

#include "src/log.h"
#include "src/gfx.h"
#include "src/freeserf.h"
#include "src/video-sdl.h"

event_loop_t *
event_loop_t::get_instance() {
  if (instance == NULL) {
    instance = new event_loop_sdl_t();
  }

  return instance;
}

/* How fast consequtive mouse events need to be generated
 in order to be interpreted as click and double click. */
#define MOUSE_TIME_SENSITIVITY  600
/* How much the mouse can move between events to be still
 considered as a double click. */
#define MOUSE_MOVE_SENSITIVITY  8

event_loop_sdl_t::event_loop_sdl_t() {
}

typedef enum {
  USER_EVENT_STEP,
  USER_EVENT_QUIT,
  USER_EVENT_CALL,
} USER_EVENT_TYPE;

static Uint32
timer_callback(Uint32 interval, void *param) {
  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user.type = SDL_USEREVENT;
  event.user.code = USER_EVENT_STEP;
  event.user.data1 = 0;
  event.user.data2 = 0;
  SDL_PushEvent(&event);

  return interval;
}

void
event_loop_sdl_t::quit() {
  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user.type = SDL_USEREVENT;
  event.user.code = USER_EVENT_QUIT;
  event.user.data1 = 0;
  event.user.data2 = 0;
  SDL_PushEvent(&event);
}

void
event_loop_sdl_t::deferred_call(deferred_callee_t *deferred_callee,
                                void *data) {
  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user.type = SDL_USEREVENT;
  event.user.code = USER_EVENT_CALL;
  event.user.data1 = deferred_callee;
  event.user.data2 = data;
  SDL_PushEvent(&event);
}

/* event_loop() has been turned into a SDL based loop.
 The code for one iteration of the original game_loop is
 in game_loop_iter. */
void
event_loop_sdl_t::run() {
  SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_TIMER);

  SDL_TimerID timer_id = SDL_AddTimer(TICK_LENGTH, timer_callback, 0);
  if (timer_id == 0) {
    return;
  }

  int drag_button = 0;
  int drag_x = 0;
  int drag_y = 0;

  unsigned int last_click[6] = {0};
  int last_click_x = 0;
  int last_click_y = 0;

  SDL_Event event;

  gfx_t *gfx = gfx_t::get_instance();
  frame_t *screen = NULL;

  while (SDL_WaitEvent(&event)) {
    unsigned int current_ticks = SDL_GetTicks();

    switch (event.type) {
      case SDL_MOUSEBUTTONUP:
        if (drag_button == event.button.button) {
          drag_button = 0;
        }

        if (event.button.button <= 3) {
          int x = static_cast<int>(static_cast<float>(event.button.x) *
                                   gfx->get_zoom_factor());
          int y = static_cast<int>(static_cast<float>(event.button.y) *
                                   gfx->get_zoom_factor());
          notify_click(x, y, (event_button_t)event.button.button);

          if (current_ticks - last_click[event.button.button] <
                MOUSE_TIME_SENSITIVITY &&
              event.button.x >= (last_click_x - MOUSE_MOVE_SENSITIVITY) &&
              event.button.x <= (last_click_x + MOUSE_MOVE_SENSITIVITY) &&
              event.button.y >= (last_click_y - MOUSE_MOVE_SENSITIVITY) &&
              event.button.y <= (last_click_y + MOUSE_MOVE_SENSITIVITY)) {
            notify_dbl_click(x, y, (event_button_t)event.button.button);
          }

          last_click[event.button.button] = current_ticks;
          last_click_x = event.button.x;
          last_click_y = event.button.y;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        break;
      case SDL_MOUSEMOTION:
        for (int button = 1; button <= 3; button++) {
          if (event.motion.state & SDL_BUTTON(button)) {
            if (drag_button == 0) {
              drag_button = button;
              drag_x = event.motion.x;
              drag_y = event.motion.y;
            }

            notify_drag(drag_x, drag_y,
                        event.motion.x - drag_x, event.motion.y - drag_y,
                        (event_button_t)drag_button);

            SDL_WarpMouseInWindow(NULL, drag_x, drag_y);

            break;
          }
        }
        break;
      case SDL_MOUSEWHEEL: {
        SDL_Keymod mod = SDL_GetModState();
        if ((mod & KMOD_CTRL) != 0) {
          zoom(0.2f * static_cast<float>(event.wheel.y));
        }
        break;
      }
      case SDL_KEYDOWN: {
        if (event.key.keysym.sym == SDLK_q &&
            (event.key.keysym.mod & KMOD_CTRL)) {
          quit();
          break;
        }

        unsigned char modifier = 0;
        if (event.key.keysym.mod & KMOD_CTRL) {
          modifier |= 1;
        }
        if (event.key.keysym.mod & KMOD_SHIFT) {
          modifier |= 2;
        }
        if (event.key.keysym.mod & KMOD_ALT) {
          modifier |= 4;
        }

        switch (event.key.keysym.sym) {
          /* Map scroll */
          case SDLK_UP: {
            notify_drag(0, 0, 0, -32, EVENT_BUTTON_LEFT);
            break;
          }
          case SDLK_DOWN: {
            notify_drag(0, 0, 0, 32, EVENT_BUTTON_LEFT);
            break;
          }
          case SDLK_LEFT: {
            notify_drag(0, 0, -32, 0, EVENT_BUTTON_LEFT);
            break;
          }
          case SDLK_RIGHT: {
            notify_drag(0, 0, 32, 0, EVENT_BUTTON_LEFT);
            break;
          }

          case SDLK_PLUS:
          case SDLK_KP_PLUS:
          case SDLK_EQUALS:
            notify_key_pressed('+', 0);
            break;
          case SDLK_MINUS:
          case SDLK_KP_MINUS:
            notify_key_pressed('-', 0);
            break;

            /* Video */
          case SDLK_f:
            if (event.key.keysym.mod & KMOD_CTRL) {
              gfx->set_fullscreen(!gfx->is_fullscreen());
            }
            break;
          case SDLK_RIGHTBRACKET:
            zoom(-0.2f);
            break;
          case SDLK_LEFTBRACKET:
            zoom(0.2f);
            break;

            /* Misc */
          case SDLK_F10:
            notify_key_pressed('n', 1);
            break;

          default:
            notify_key_pressed(event.key.keysym.sym, modifier);
            break;
        }

        break;
      }
      case SDL_QUIT:
        notify_key_pressed('c', 1);
        break;
      case SDL_WINDOWEVENT:
        if (SDL_WINDOWEVENT_SIZE_CHANGED == event.window.event) {
          unsigned int width = event.window.data1;
          unsigned int height = event.window.data2;
          gfx->set_resolution(width, height, gfx->is_fullscreen());
          notify_resize(width, height);
        }
        break;
      case SDL_USEREVENT:
        switch (event.user.code) {
          case USER_EVENT_QUIT:
            SDL_RemoveTimer(timer_id);
            if (screen != NULL) {
              delete screen;
              screen = NULL;
            }
            return;
          case USER_EVENT_STEP:
            /* Update and draw interface */
            notify_update();

            if (screen == NULL) {
              screen = gfx->get_screen_frame();
            }
            notify_draw(screen);

            /* Swap video buffers */
            gfx->swap_buffers();
            break;
          case USER_EVENT_CALL: {
            deferred_callee_t *deferred_callee =
              reinterpret_cast<deferred_callee_t*>(event.user.data1);
            if (deferred_callee != NULL) {
              deferred_callee->deferred_call(event.user.data2);
            }
            break;
          }
          default:
            break;
        }
        break;
    }
  }

  SDL_RemoveTimer(timer_id);
  if (screen != NULL) {
    delete screen;
    screen = NULL;
  }
}

void
event_loop_sdl_t::zoom(float delta) {
  gfx_t *gfx = gfx_t::get_instance();
  float factor = gfx->get_zoom_factor();
  if (gfx->set_zoom_factor(factor + delta)) {
    unsigned int width = 0;
    unsigned int height = 0;
    gfx->get_resolution(&width, &height);
    notify_resize(width, height);
  }
}
