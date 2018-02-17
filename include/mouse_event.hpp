#ifndef LIBWTK_SDL2_MOUSE_EVENT_HPP
#define LIBWTK_SDL2_MOUSE_EVENT_HPP

#include <optional>

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

struct mouse_move_event
{
    // TODO add optional button of down position then it is not necessary to
    // keep such state in the individual widgets
    point position;
};

#endif

