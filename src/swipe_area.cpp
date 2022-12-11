// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "swipe_area.hpp"

swipe_area::swipe_area(std::function<void(swipe_direction)> swipe_callback, std::function<void()> press_callback)
    : _swipe_callback(swipe_callback)
    , _press_callback(press_callback)
{
}

swipe_area::~swipe_area()
{
}

void swipe_area::on_draw(draw_context & dc, selection_context const & sc) const
{
}

/*
void swipe_area::on_mouse_down_event(mouse_down_event const & e)
{
}
*/

void swipe_area::on_mouse_up_event(mouse_up_event const & e)
{
    auto opt_info = get_swipe_info_with_context_info(e);
    if (opt_info.has_value())
    {
        auto const & info = opt_info.value();

        if (info.type == swipe_info::type::PRESS)
        {
            _press_callback();
        }
        else
        {
            _swipe_callback(info.dir);
        }
    }
}

size_hint swipe_area::get_size_hint(int width, int height) const
{
    // TODO how to dimension ?
    int v = get_context_info().font_line_skip();
    return size_hint({ v, v });
}
