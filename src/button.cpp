// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "button.hpp"

#include "geometry.hpp"

button::button(std::function<void()> callback)
    : _pressed(false)
    , _callback(callback)
{
}

void button::on_draw(draw_context & dc, selection_context const & sc) const
{
    dc.draw_button_box(get_box(), _pressed, sc.is_selected_widget(this));
    draw_drawable(dc, get_box());
}

void button::on_mouse_down_event(mouse_down_event const & e)
{
    bool const hit = within_rect(e.position, get_box());
    if (hit != _pressed)
    {
        _pressed = hit;
        mark_dirty();
    }
}

void button::on_mouse_up_event(mouse_up_event const & e)
{
    if (_pressed)
    {
        bool const hit = within_rect(e.position, get_box());
        if (hit)
            _callback();
        _pressed = false;
        mark_dirty();
    }
}

void button::on_key_event(key_event const & e)
{
    // TODO enter?
}

void button::on_activate()
{
    _callback();
}

size_hint button::get_size_hint(int width, int height) const
{
    vec const dsize = get_drawable_size();
    // TODO get border dimensions from context info
    vec const minimal = dsize + vec{ 2, 2 };

    int const h_spacing = get_context_info().font_line_skip();
    vec const natural = minimal + vec{ h_spacing - 1, h_spacing - 1 };
    return size_hint(minimal, natural);
}

button::~button()
{
}

