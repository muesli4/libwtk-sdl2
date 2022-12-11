// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "context_info.hpp"

context_info::context_info(font_manager & fm, swipe_config swipe_cfg)
    : swipe_cfg(swipe_cfg)
    , _fm(fm)
{
}

vec context_info::text_size(std::string t, int max_line_width, int font_idx) const
{
    return _fm.get().text_size(t, max_line_width, font_idx);
}

int context_info::text_minimum_width(std::string t, int font_idx) const
{
    return _fm.get().text_minimum_width(t, font_idx);
}

unsigned int context_info::font_height(int font_idx) const
{
    return _fm.get().font_height(font_idx);
}

int context_info::font_line_skip(int font_idx) const
{
    return _fm.get().font_line_skip(font_idx);
}

