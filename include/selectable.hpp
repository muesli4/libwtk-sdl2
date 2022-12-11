// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_SELECTABLE_HPP
#define LIBWTK_SDL2_SELECTABLE_HPP

#include "widget.hpp"

struct selectable : widget
{
    widget * find_selectable(navigation_type nt, point center) override;
    widget * navigate_selectable(navigation_type nt, point center) override;

    void on_select() override;
    void on_unselect() override;

    ~selectable() override;
};

#endif

