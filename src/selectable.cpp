// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "selectable.hpp"

widget * selectable::find_selectable(navigation_type nt, point center)
{
    return this;
}

widget * selectable::navigate_selectable(navigation_type nt, point p)
{
    return navigate_selectable_parent(nt, p);
}

selectable::~selectable()
{
}

void selectable::on_select()
{
    mark_dirty();
}

void selectable::on_unselect()
{
    mark_dirty();
}
