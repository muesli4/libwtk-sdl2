// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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

std::optional<swipe_info> get_swipe_info(mouse_movement const & m, swipe_config const & cfg)
{
    vec const swipe_vec = m.length;
    vec const abs_swipe_vec = abs(swipe_vec);

    swipe_direction dir;

    bool is_swipe = square(abs_swipe_vec.w) + square(abs_swipe_vec.h)
                    > square(cfg.lower_threshold);

    // vertical swipe
    if (abs_swipe_vec.h * cfg.dir_unambig_factor >= abs_swipe_vec.w)
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
    else if (abs_swipe_vec.w * cfg.dir_unambig_factor >= abs_swipe_vec.h)
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
    // blind spot
    else if (is_swipe)
    {
        return std::nullopt;
    }

    // A direction is not enough, it also has to be below the threshold.
    if (is_swipe)
    {
        return std::make_optional(swipe_info{ swipe_info::type::DIRECTION, dir });
    }
    // press
    else
    {
        return std::make_optional(swipe_info{ swipe_info::type::PRESS });
    }
}
