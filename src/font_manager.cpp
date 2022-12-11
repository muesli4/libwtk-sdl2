// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "font_manager.hpp"

font_manager::font_manager(SDL_Renderer * renderer, std::vector<font> fonts)
    : _renderer(renderer)
{
    for (font const & f : fonts)
    {
        _font_word_caches.emplace_back(renderer, f);
    }
}

std::size_t font_manager::load_font(font f)
{
    _font_word_caches.emplace_back(_renderer, f);
    return _font_word_caches.size() - 1;
}

std::tuple<vec, std::vector<copy_command>> font_manager::text(std::string t, int max_line_width, int font_idx)
{
    return _font_word_caches.at(font_idx).text(t, max_line_width);
}

vec font_manager::text_size(std::string t, int max_line_width, int font_idx)
{
    return _font_word_caches.at(font_idx).text_size(t, max_line_width);
}

int font_manager::text_minimum_width(std::string t, int font_idx)
{
    return _font_word_caches.at(font_idx).text_minimum_width(t);
}

unsigned int font_manager::font_height(int font_idx) const
{
    return _font_word_caches.at(font_idx).font_height();
}

int font_manager::font_line_skip(int font_idx) const
{
    return _font_word_caches.at(font_idx).font_line_skip();
}

