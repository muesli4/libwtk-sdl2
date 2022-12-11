// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_NAVIGATION_TYPE_HPP
#define LIBWTK_SDL2_NAVIGATION_TYPE_HPP

// Describes how a widget tree might be navigated. A navigation without
// 2-dimensional context should reach all seletable widgets, whereas one with
// context should preferably hit locally adjacent widgets.
enum class navigation_type { NEXT_X, NEXT_Y, PREV_X, PREV_Y, NEXT, PREV };

/**
 * Checks whether navigation is forward (i.e. it picks the next element in a
 * direction).
 */
bool is_forward(navigation_type nt);

#endif

