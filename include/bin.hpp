// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_BIN_HPP
#define LIBWTK_SDL2_BIN_HPP

#include "widget.hpp"

// Base class for containers that have a single child.
struct bin : widget
{
    bin(widget_ptr child);
    ~bin() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_down_event(mouse_down_event const & me) override;
    void on_mouse_up_event(mouse_up_event const & me) override;
    void on_mouse_move_event(mouse_move_event const & e) override;

    void on_box_allocated() override = 0;

    widget * find_selectable(navigation_type nt, point center) override;
    widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center) override;

    // Should be provided when the geometry of a child is not equal to the
    // geometry of the bin (i.e. if there is some kind of padding).
    size_hint get_size_hint(int width, int height) const override;

    std::vector<widget *> get_children() override;
    std::vector<widget const *> get_children() const override;

    protected:

    widget_ptr _child;
};

#endif
