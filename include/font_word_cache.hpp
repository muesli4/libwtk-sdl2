// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_FONT_WORD_CACHE_HPP
#define LIBWTK_SDL2_FONT_WORD_CACHE_HPP

#include <memory>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL_ttf.h>

#include "copy_command.hpp"
#include "font.hpp"
#include "geometry.hpp"
#include "sdl_util.hpp"

struct font_not_found : std::runtime_error
{
    font_not_found(std::string);
};

struct font_render_error : std::runtime_error
{
    font_render_error(std::string);
};

struct word_fragment
{
    std::string word;
    std::size_t extra_spaces;
};

struct font_word_cache
{
    font_word_cache(SDL_Renderer * renderer, font f);
    ~font_word_cache();

    font_word_cache(font_word_cache const &) = delete;
    font_word_cache(font_word_cache && other);

    // Renders words onto textures with a cache. A proper layout is then created
    // to fit the textures on the given line width.
    //
    // The result is the required space and the commands necessary to copy it to
    // a render target. Note that it is not guaranteed that the textures are
    // still valid after another call only in between calls.
    //
    // TODO add offset parameter
    std::tuple<vec, std::vector<copy_command>> text(std::string t, int max_line_width = -1);

    vec text_size(std::string t, int max_line_width = -1);
    int text_minimum_width(std::string t);

    unsigned int font_height() const;

    int font_line_skip() const;

    void clear();

    private:

    template <typename BackInsertIt>
    vec compute_text_layout(std::string t, int max_line_width, BackInsertIt it);

    int get_word_left_kerning(std::string_view const word);
    int get_word_right_kerning(std::string_view const word);

    // Correctly handles dimension for a nullptr.
    vec texture_dim_nullptr(SDL_Texture * texture) const;

    // May return nullptr for zero-length text.
    SDL_Texture * word(std::string);

    SDL_Renderer * _renderer;
    std::unordered_map<std::string, SDL_Texture *> _prerendered;
    TTF_Font * _font;
    int _space_advance;
    int _space_minx;

};

#endif

