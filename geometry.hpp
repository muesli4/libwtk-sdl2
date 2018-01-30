#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <SDL2/SDL.h>

// checks whether a point lies within a rectangle
bool within_rect(int32_t x, int32_t y, SDL_Rect const & r);

#endif

