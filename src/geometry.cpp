#include "geometry.hpp"

int square(int i)
{
    return i * i;
}

bool within_rect(point p, SDL_Rect const & r)
{
    return within_bound(p.x, r.x, r.w) && within_bound(p.y, r.y, r.h);
    //return p.x >= r.x && p.x < r.x + r.w && p.y >= r.y && p.y < r.y + r.h;
}

bool within_bound(int val, int lower, int length)
{
    return val >= lower && val < lower + length;
}

point rect_center(SDL_Rect const & r)
{
    return { r.x + r.w / 2, r.y + r.h / 2 };
}

point rect_origin(SDL_Rect const & r)
{
    return { r.x, r.y };
}

SDL_Rect rect(vec v)
{
    return { 0, 0, v.w, v.h };
}

vec operator+(vec v, vec u)
{
    return { v.w + u.w, v.h + u.h };
}

point operator+(point p, vec v)
{
    return { p.x + v.w, p.y + v.h };
}

vec operator-(vec v)
{
    return { - v.w, - v.h };
}

point operator-(point p, vec v)
{
    return p + (- v);
}

bool operator<(vec v, vec w)
{
    return v.w < w.w && v.h < w.h;
}

vec operator*(vec v, int s)
{
    return { v.w * s, v.h * s };
}

vec abs(vec v)
{
    return { std::abs(v.w), std::abs(v.h) };
}

vec operator-(point p, point q)
{
    return { p.x - q.x, p.y - q.y };
}

