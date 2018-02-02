#ifndef SWIPE_EVENT_HPP
#define SWIPE_EVENT_HPP

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

