// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_SWIPE_HPP
#define LIBWTK_SDL2_SWIPE_HPP

#include <optional>
#include <string>

#include "mouse_event.hpp"

struct swipe_config
{
    int lower_threshold;
    double dir_unambig_factor;
};

enum class swipe_direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

struct swipe_info
{
    enum class type
    {
        PRESS,
        DIRECTION
    };

    type type;

    // only valid if type is type::DIRECTION
    swipe_direction dir;
};

std::string to_string(swipe_direction dir);

std::optional<swipe_info> get_swipe_info(mouse_movement const & m, swipe_config const & cfg);

#endif
