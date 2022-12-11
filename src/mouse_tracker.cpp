// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mouse_tracker.hpp"

void mouse_tracker::mouse_down(point p)
{
    _opt_down_pos = p;
}

mouse_up_event mouse_tracker::mouse_up(point p)
{
    if (_opt_down_pos.has_value())
    {
        point down_p = _opt_down_pos.value();
        auto length = p - down_p;

        reset();
        return mouse_up_event { p, std::make_optional(mouse_movement { down_p, length }) };
    }
    else
    {
        return mouse_up_event { p, std::nullopt };
    }
}

mouse_move_event mouse_tracker::mouse_move(point p)
{

    // TODO make use of relative movement from SDL?

    // TODO
    /*
    std::optional<mouse_movement> opt_movement;

    if (_opt_down_pos.has_value())
    {
        _opt_down_pos.value();

        _opt_movement = 

        struct mouse_movement
        {
            point origin;
            vec length;
        };
    }
    */

    return mouse_move_event { p };
}

void mouse_tracker::reset()
{
    _opt_down_pos.reset();
}
