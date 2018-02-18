#ifndef LIBWTK_SDL2_SWIPE_HPP
#define LIBWTK_SDL2_SWIPE_HPP

#include <optional>
#include <string>

#include "mouse_event.hpp"

enum class swipe_direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

std::string to_string(swipe_direction dir);

std::optional<swipe_direction> get_swipe_direction(mouse_movement const & m, int swipe_lower_threshold, double dir_unambig_factor);

#endif
