#include <algorithm>

#include "font_word_cache.hpp"
#include "util.hpp"
#include "sdl_util.hpp"

font_not_found::font_not_found(std::string msg)
    : std::runtime_error("font not found: " + msg)
{
}

font_render_error::font_render_error(std::string msg)
    : std::runtime_error("failed to render font: " + msg)
{
}

font_word_cache::font_word_cache(SDL_Renderer * renderer, std::string font_path, int ptsize)
    : _renderer(renderer)
{
    // load font and generate glyphs
    _font = TTF_OpenFont(font_path.c_str(), ptsize);

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
uint32_t get_last_ucs4(std::string s)
{
    char const * ptr = s.c_str() + s.size() - 1;
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

uint32_t get_first_ucs4(std::string s)
{
    char const * ptr = s.c_str();
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

int font_word_cache::get_word_left_kerning(std::string const & word)
{
    return TTF_GetFontKerningSizeGlyphs(_font, get_last_ucs4(word), ' ');
}

int font_word_cache::get_word_right_kerning(std::string const & word)
{
    return TTF_GetFontKerningSizeGlyphs(_font, ' ', get_first_ucs4(word));
}

std::vector<std::string> split_words(std::string t)
{
    std::vector<std::string> words;

    std::size_t pos;
    std::size_t last_pos = 0;
    while ((pos = t.find(' ', last_pos)) != std::string::npos)
    {
        std::string const & w = t.substr(last_pos, pos - last_pos);
        // ignore multiple spaces
        if (!w.empty())
        {
            words.push_back(w);
        }
        last_pos = pos + 1;
    }
    {
        std::string const & w = t.substr(last_pos);
        if (!w.empty())
            words.push_back(w);
    }

    return words;
}

std::tuple<vec, std::vector<copy_command>> font_word_cache::text(std::string t, int max_line_width)
{
    // FIXME: temporary solution - do not use too much memory, although with words caching it's not as bad as before
    // TODO add use count? would it cause ui lag?
    if (_prerendered.size() > 40000)
        clear();

    std::vector<copy_command> copy_commands;

    vec target_size { 0, 0 };
    
    if (!t.empty())
    {
        auto words = split_words(t);

        std::vector<SDL_Texture *> textures;

        // render or load cached word textures
        for (auto const & w : words)
        {
            SDL_Texture * t = word(w);
            textures.push_back(t);
        }
        if (!textures.empty())
        {
            std::vector<int> widths { texture_dim(textures[0]).w };
            std::vector<std::vector<int>> spaces_per_line { { 0 } };

            // TODO check if surface width does exceed line width ?
            std::vector<std::vector<SDL_Texture *>> textures_per_line { { textures[0] } };

            // assertion: textures.size() == words.size()
            for (std::size_t n = 0; n + 1 < words.size(); n++)
            {
                int & line_width = widths.back();

                // use proper kerning and advance of space to connect words
                int const join_width = get_word_left_kerning(words[n]) + _space_advance + get_word_right_kerning(words[n + 1]);

                int const new_line_width = line_width + join_width + texture_dim(textures[n + 1]).w;

                if (max_line_width == -1 || new_line_width < max_line_width)
                {
                    textures_per_line.back().push_back(textures[n + 1]);
                    spaces_per_line.back().push_back(join_width);
                    line_width = new_line_width;
                }
                else
                {
                    // TODO check if surface width does exceed line width ?
                    textures_per_line.push_back({textures[n + 1]});
                    spaces_per_line.push_back({0});
                    widths.push_back(texture_dim(textures[n + 1]).w);
                }
            }

            target_size.w = *std::max_element(widths.begin(), widths.end());
            target_size.h = font_line_skip() * static_cast<int>(widths.size());

            int x = 0;
            int y = 0;

            for (std::size_t n = 0; n < textures_per_line.size(); n++)
            {
                // blit line
                for (std::size_t m = 0; m < textures_per_line[n].size(); m++)
                {
                    SDL_Texture * t = textures_per_line[n][m];

                    x += spaces_per_line[n][m];
                    copy_commands.push_back({ t, x, y });
                    x += texture_dim(t).w;
                }
                x = 0;
                y += font_line_skip();
            }
        }
    }

    return std::make_tuple(target_size, copy_commands);
}

vec font_word_cache::text_size(std::string t, int max_line_width)
{
    if (max_line_width == -1)
    {
        int w, h;
        // TODO probably better to prerender here, since it has to been done in any case
        if (TTF_SizeUTF8(_font, t.c_str(), &w, &h) == -1)
        {
            // TODO handle error properly
            return { -1, -1 };
        }
        else
        {
            return { w, h };
        }
    }
    else
    {
        auto words = split_words(t);

        if (words.empty())
        {
            return { 0, 0 };
        }

        int actual_max_width = 0;
        int lines = 1;
        int current_line_num_words = 1;
        int current_line_width = texture_dim(word(words[0])).w;

        for (std::size_t k = 0; k < words.size(); ++k)
        {
            auto word_tex = word(words[k]);
            vec size = texture_dim(word_tex);


            int const additional_width = size.w + _space_advance;

            if (additional_width + current_line_width >= max_line_width)
            {
                // Not enough space to fit it on the current line.

                if (size.w >= max_line_width)
                {
                    // TODO word does not fit:
                    //      can return failure
                    //      or try to do word splitting
                    return { -1, -1 };
                }
                else
                {
                    actual_max_width = std::max(current_line_width, actual_max_width);
                    current_line_width = size.w;
                    current_line_num_words = 1;
                    lines += 1;
                }
            }
            else
            {
                current_line_width += additional_width;
                current_line_num_words += 1;
            }
        }

        actual_max_width = std::max(current_line_width, actual_max_width);

        return { actual_max_width, lines * font_line_skip() };
    }
}

int font_word_cache::text_minimum_width(std::string t)
{
    int max_width = 0;
    auto words = split_words(t);
    for (auto word : words)
    {
        int width;
        // TODO probably better to prerender here, since it has to been done in any case
        if (TTF_SizeUTF8(_font, word.c_str(), &width, nullptr) == -1)
            return -1;
        max_width = std::max(max_width, width);
    }
    return max_width;
}

SDL_Texture * font_word_cache::word(std::string w)
{
    auto it = _prerendered.find(w);
    if (it == _prerendered.end())
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

        return _prerendered[w] = t;
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
    for (auto p : _prerendered)
        SDL_DestroyTexture(p.second);
    _prerendered.clear();
}

