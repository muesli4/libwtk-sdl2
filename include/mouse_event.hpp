#ifndef LIBWTK_SDL2_MOUSE_EVENT_HPP
#define LIBWTK_SDL2_MOUSE_EVENT_HPP

#include <optional>

#include "geometry.hpp"

struct mouse_down_event
{
    point position;
};

struct mouse_movement
{
    point origin;
    vec length;
};

struct mouse_up_event
{
    point position;

    std::optional<mouse_movement> opt_movement;
};

struct mouse_move_event
{
    // TODO add optional button of down position then it is not necessary to
    // keep such state in the individual widgets
    point position;
};

#endif

