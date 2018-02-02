#include "geometry.hpp"

bool within_rect(point p, SDL_Rect const & r)
{
    return p.x >= r.x && p.x < r.x + r.w && p.y >= r.y && p.y < r.y + r.h;
}

point rect_center(SDL_Rect const & r)
{
    return { r.x + r.w / 2, r.y + r.h / 2 };
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
