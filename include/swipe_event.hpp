#ifndef LIBWTK_SDL2_SWIPE_EVENT_HPP
#define LIBWTK_SDL2_SWIPE_EVENT_HPP

#include "geometry.hpp"

enum class swipe_action
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

struct swipe_event
{
    point position;
    vec length;

    swipe_action action;
};

#endif

