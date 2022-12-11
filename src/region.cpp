// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <algorithm>

#include "region.hpp"

size_hint::size_hint(vec min, vec nat)
    : minimal(min)
    , natural(nat)
{
}

size_hint::size_hint(vec min)
    : size_hint(min, min)
{
}

void region::apply_layout(rect box)
{
    _box = box;

    // If we can't assing enough space for a region make at least a sane box.
    _box.w = std::max(0, _box.w);
    _box.h = std::max(0, _box.h);

    on_box_allocated();
}

void region::on_box_allocated()
{
}

bool region::can_use_intermediate_size() const
{
    return true;
}

rect const & region::get_box() const
{
    return _box;
}

