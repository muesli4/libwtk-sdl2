#ifndef LIBWTK_SDL2_GEOMETRY_HPP
#define LIBWTK_SDL2_GEOMETRY_HPP

#include <SDL2/SDL.h>

struct point
{
    int x;
    int y;
};

struct vec
{
    int w;
    int h;
};

int square(int i);

bool within_bound(int val, int lower, int length);

// checks whether a point lies within a rectangle
bool within_rect(point p, SDL_Rect const & r);

point rect_center(SDL_Rect const & r);
point rect_origin(SDL_Rect const & r);

// Create a rectangle with zero origin.
SDL_Rect rect(vec v);

vec operator+(vec v, vec u);
point operator+(point p, vec v);
vec operator-(vec v);
point operator-(point p, vec v);
bool operator<(vec v, vec w);
vec operator*(vec v, int s);

vec abs(vec v);

vec operator-(point p, point q);


#endif

