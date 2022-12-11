// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "navigation_type.hpp"

bool is_forward(navigation_type nt)
{
    return
        nt == navigation_type::NEXT
        || nt == navigation_type::NEXT_X
        || nt == navigation_type::NEXT_Y;
}

