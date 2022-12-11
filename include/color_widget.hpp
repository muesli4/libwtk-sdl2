// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_COLOR_WIDGET_HPP
#define LIBWTK_SDL2_COLOR_WIDGET_HPP

#include "selectable.hpp"

struct color_widget : selectable
{
    color_widget();

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_up_event(mouse_up_event const & e) override;
    void on_key_event(key_event const & e) override;
    void on_activate() override;
    size_hint get_size_hint(int width, int height) const override;

    ~color_widget() override;

    private:
    void recolor();

    int _color_src;
};

#endif

