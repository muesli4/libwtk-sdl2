#include "swipe.hpp"

std::string to_string(swipe_direction dir)
{
    switch(dir)
    {
        case swipe_direction::UP:    return "up";
        case swipe_direction::DOWN:  return "down";
        case swipe_direction::LEFT:  return "left";
        case swipe_direction::RIGHT: return "right";
        default:                     return "invalid direction";
    }
}

std::optional<swipe_direction> get_swipe_direction(mouse_movement const & m, int swipe_lower_threshold, double dir_unambig_factor)
{
    vec const swipe_vec = m.length;
    vec const abs_swipe_vec = abs(swipe_vec);

    swipe_direction dir;

    // vertical swipe
    if (abs_swipe_vec.h * dir_unambig_factor >= abs_swipe_vec.w)
    {
        if (swipe_vec.h < 0)
        {
            dir = swipe_direction::UP;
        }
        else
        {
            dir = swipe_direction::DOWN;
        }
    }
    // horizontal swipe
    else if (abs_swipe_vec.w * dir_unambig_factor >= abs_swipe_vec.h)
    {
        if (swipe_vec.w > 0)
        {
            dir = swipe_direction::RIGHT;
        }
        else
        {
            dir = swipe_direction::LEFT;
        }
    }
    else
    {
        return std::nullopt;
    }

    // A direction is not enough, it also has to be below the threshold.
    if (square(abs_swipe_vec.w) + square(abs_swipe_vec.h) > square(swipe_lower_threshold))
    {
        return std::make_optional(dir);
    }
    else
    {
        return std::nullopt;
    }
}
