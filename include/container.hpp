// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_CONTAINER_HPP
#define LIBWTK_SDL2_CONTAINER_HPP

#include <functional>

#include "widget.hpp"

// Base class for containers with multiple widgets that are all drawn to a
// common area.
struct container : widget
{
    ~container() override;
    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_up_event(mouse_up_event const & me) override;
    void on_mouse_down_event(mouse_down_event const & me) override;
    void on_mouse_move_event(mouse_move_event const & e) override;

    std::vector<widget *> get_children() override = 0;
    std::vector<widget const *> get_children() const override = 0;
    void on_box_allocated() override = 0;
    widget * find_selectable(navigation_type nt, point center) override = 0;

    protected:

    void init_children();

};

#endif

