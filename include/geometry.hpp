#ifndef LIBWTK_SDL2_GEOMETRY_HPP
#define LIBWTK_SDL2_GEOMETRY_HPP

#include <SDL2/SDL_rect.h>

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

typedef SDL_Rect rect;

int square(int i);

bool within_bound(int val, int lower, int length);

// checks whether a point lies within a rectangle
bool within_rect(point p, rect const & r);

bool fits(vec v, rect const & r);

point rect_center(rect const & r);
point rect_origin(rect const & r);

// create a rectangle with zero origin.
rect origin_rect(vec v);
// create a rectangle out of a origin and size
rect base_rect(point origin, vec size);

// vector operations
vec operator+(vec v, vec u);
vec operator-(vec v);
bool operator<(vec v, vec w);

point operator+(point p, vec v);
point operator-(point p, vec v);

// vector scalar operations
vec operator*(vec v, int s);
vec operator/(vec v, int s);

vec abs(vec v);

vec operator-(point p, point q);

// creates a new rect by centering a box with given dimensions inside a given
// rect
rect center_vec_within_rect(vec inside, rect outside);

vec length(rect const & r);

#endif

