#ifndef MOUSE_EVENT_HPP
#define MOUSE_EVENT_HPP

#include <optional>

#include <SDL2/SDL.h>

#include "geometry.hpp"
#include "swipe_event.hpp"

struct mouse_down_event
{
    point position;
};

struct mouse_up_event
{
    point position;

    // If the event loop decides the drag could be interpreted as a swipe the
    // widget may decide to interpret it that way.
    std::optional<swipe_event> opt_swipe_event;
};

#endif

