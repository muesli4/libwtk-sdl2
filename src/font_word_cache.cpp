// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <algorithm>

#include "font_word_cache.hpp"
#include "utf8.hpp"
#include "sdl_util.hpp"

font_not_found::font_not_found(std::string msg)
    : std::runtime_error("font not found: " + msg)
{
}

font_render_error::font_render_error(std::string msg)
    : std::runtime_error("failed to render font: " + msg)
{
}

font_word_cache::font_word_cache(SDL_Renderer * renderer, font f)
    : _renderer(renderer)
{
    // load font and generate glyphs
    _font = TTF_OpenFont(f.path.c_str(), f.size);

    if (_font == nullptr)
        throw font_not_found(TTF_GetError());

    // cache join point metrics
    TTF_GlyphMetrics(_font, ' ', &_space_minx, nullptr, nullptr, nullptr, &_space_advance);
}

font_word_cache::~font_word_cache()
{
    TTF_CloseFont(_font);

    clear();
}

font_word_cache::font_word_cache(font_word_cache && other)
    : _renderer(other._renderer)
    , _prerendered(std::move(other._prerendered))
    , _font(other._font)
    , _space_advance(other._space_advance)
    , _space_minx(other._space_minx)
{
    other._font = nullptr;
}

// http://stackoverflow.com/questions/18534494/convert-from-utf-8-to-unicode-c
#include <deque>
uint32_t utf8_to_ucs4(std::deque<uint8_t> coded)
{
    int charcode = 0;
    int t = coded.front();
    coded.pop_front();
    if (t < 128)
    {
        return t;
    }
    int high_bit_mask = (1 << 6) -1;
    int high_bit_shift = 0;
    int total_bits = 0;
    const int other_bits = 6;
    while((t & 0xC0) == 0xC0)
    {
        t <<= 1;
        t &= 0xff;
        total_bits += 6;
        high_bit_mask >>= 1;
        high_bit_shift++;
        charcode <<= other_bits;
        charcode |= coded.front() & ((1 << other_bits)-1);
        coded.pop_front();
    }
    charcode |= ((t >> high_bit_shift) & high_bit_mask) << total_bits;
    return charcode;
}

// get the last utf8 character from a string
uint32_t get_last_ucs4(std::string_view const s)
{
    char const * ptr = s.data() + s.size() - 1;
    if (*ptr == 0) ptr = " ";

    std::deque<uint8_t> d;
    while (is_utf8_following_byte(*ptr))
    {
        d.push_front(*ptr);
        ptr--;
    }
    d.push_front(*ptr);
    return utf8_to_ucs4(d);
}

uint32_t get_first_ucs4(std::string_view const s)
{
    char const * ptr = s.data();
    if (*ptr == 0) ptr = " ";

    std::deque<uint8_t> d;
    d.push_back(*ptr);
    ptr++;
    while (is_utf8_following_byte(*ptr))
    {
        d.push_back(*ptr);
        ptr++;
    }
    return utf8_to_ucs4(d);
}

int font_word_cache::get_word_left_kerning(std::string_view const word)
{
    return TTF_GetFontKerningSizeGlyphs(_font, get_last_ucs4(word), ' ');
}

int font_word_cache::get_word_right_kerning(std::string_view const word)
{
    return TTF_GetFontKerningSizeGlyphs(_font, ' ', get_first_ucs4(word));
}

std::vector<word_fragment> split_words(std::string t)
{
    std::vector<word_fragment> words;

    std::size_t pos;
    std::size_t last_pos = 0;
    while ((pos = t.find(' ', last_pos)) != std::string::npos)
    {
        std::string const & w = t.substr(last_pos, pos - last_pos);

        std::size_t num_spaces = 1;

        while (pos + num_spaces < t.size())
        {
            if (t[pos + num_spaces] == ' ')
            {
                num_spaces++;
            }
            else
                break;
        }

        words.push_back(word_fragment{ w, num_spaces - 1 });

        last_pos = pos + num_spaces;
    }
    {
        std::string const & w = t.substr(last_pos);
        if (!w.empty())
            words.push_back(word_fragment{ w, 0 });
    }

    return words;
}

vec font_word_cache::texture_dim_nullptr(shared_texture_ptr const & texture) const
{
    if (texture == nullptr)
    {
        return { 0, font_line_skip() };
    }
    else
    {
        return texture_dim(texture.get());
    }
}

template <typename BackInsertIt>
vec font_word_cache::compute_text_layout(std::string t, int max_line_width, BackInsertIt it)
{
    vec target_size { 0, 0 };
    
    auto word_fragments = split_words(t);
    if (!word_fragments.empty())
    {
        int actual_max_width = 0;

        auto first_texture = word(word_fragments[0].word);
        vec const first_dim = texture_dim_nullptr(first_texture);
        int const first_left_kerning = (word_fragments[0].extra_spaces > 0 ? get_word_left_kerning(word_fragments[0].word) : 0);

        if (first_texture != nullptr)
        {
            *it = { first_texture, origin_rect(first_dim), { 0, 0 } };
            ++it;
        }

        int line_width = first_dim.w + first_left_kerning + word_fragments[0].extra_spaces * _space_advance;
        int height = 0;

        int prev_post_left_kerning = word_fragments[0].extra_spaces > 0 ? 0 : get_word_left_kerning(word_fragments[0].word);

        for (std::size_t k = 1; k < word_fragments.size(); ++k)
        {
            auto const & current_wf = word_fragments[k];
            auto current_texture = word(current_wf.word);
            vec const current_dim = texture_dim_nullptr(current_texture);

            int current_pre_left_kerning;
            int current_post_left_kerning;
            
            if (current_wf.extra_spaces > 0)
            {
                current_pre_left_kerning = get_word_left_kerning(current_wf.word);
                current_post_left_kerning = 0;
            }
            else
            {
                current_pre_left_kerning = 0;
                current_post_left_kerning = get_word_left_kerning(current_wf.word);
            }

            int const spacing = prev_post_left_kerning + _space_advance + get_word_right_kerning(current_wf.word);
            int const next_line_width = line_width + spacing + current_dim.w + current_pre_left_kerning + current_wf.extra_spaces * _space_advance;

            if (max_line_width == -1 || next_line_width <= max_line_width)
            {
                // word does fit

                if (current_texture != nullptr)
                {
                    *it = { current_texture, origin_rect(current_dim), { line_width + spacing, height } };
                    ++it;
                }

                line_width = next_line_width;
            }
            else
            {
                // word does not fit, start a new line
                actual_max_width = std::max(actual_max_width, line_width);
                line_width = current_dim.w;
                height += font_line_skip();

                if (current_texture != nullptr)
                {
                    *it = { current_texture, origin_rect(current_dim), { 0, height } };
                }
            }

            prev_post_left_kerning = current_post_left_kerning;
        }

        target_size.w = std::max(actual_max_width, line_width);
        target_size.h = height + font_line_skip();
    }

    return target_size;
}

std::tuple<vec, std::vector<copy_command>> font_word_cache::text(std::string t, int max_line_width)
{
    // FIXME: temporary solution - do not use too much memory, although with words caching it's not as bad as before
    // TODO add use count? would it cause ui lag?
    if (_prerendered.size() > 40000)
        clear();

    std::vector<copy_command> copy_commands;
    vec target_size = compute_text_layout(t, max_line_width, std::back_inserter(copy_commands));
    return std::make_tuple(target_size, copy_commands);
}

// Not really an iterator but satisfies the use case.
struct null_iterator
{
    struct assignment_dummy
    {
        assignment_dummy & operator=(copy_command const & t) { return *this; }
    };
    null_iterator & operator++() { return *this; }
    assignment_dummy operator*() { return assignment_dummy(); };
};

vec font_word_cache::text_size(std::string t, int max_line_width)
{
    return compute_text_layout(t, max_line_width, null_iterator());
}

int font_word_cache::text_minimum_width(std::string t)
{
    int max_width = 0;
    auto word_fragments = split_words(t);
    for (auto wf : word_fragments)
    {
        max_width = std::max(max_width, texture_dim_nullptr(word(wf.word)).w + static_cast<int>(wf.extra_spaces) * _space_advance);
    }
    return max_width;
}

shared_texture_ptr font_word_cache::word(std::string w)
{
    auto it = _prerendered.find(w);
    if (it == _prerendered.end())
    {
        // protect against unsupported zero length
        if (w.empty())
        {
            return nullptr;
        }
        else
        {
            // Render in white, then we can use the SDL_SetTextureColorMod to get
            // any color.
            SDL_Surface * s = TTF_RenderUTF8_Blended(_font, w.c_str(), {255, 255, 255});

            if (s == nullptr)
                throw font_render_error(TTF_GetError());

            SDL_Texture * t = SDL_CreateTextureFromSurface(_renderer, s);

            if (t == nullptr)
                throw font_render_error(SDL_GetError());

            // Use alpha blending to make use of the alpha channel.
            if (SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND) < 0)
                throw font_render_error(SDL_GetError());

            return _prerendered[w] = shared_texture_ptr(t, texture_destroyer());
        }
    }
    else
    {
        return it->second;
    }
}

unsigned int font_word_cache::font_height() const
{
    return TTF_FontHeight(const_cast<TTF_Font *>(_font));
}

int font_word_cache::font_line_skip() const
{
    return TTF_FontLineSkip(const_cast<TTF_Font *>(_font));
}

void font_word_cache::clear()
{
    _prerendered.clear();
}

