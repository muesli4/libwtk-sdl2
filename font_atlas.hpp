#ifndef FONT_ATLAS_HPP
#define FONT_ATLAS_HPP

#include <stdexcept>
#include <unordered_map>
#include <memory>

#include <SDL2/SDL_ttf.h>

#include "geometry.hpp"

struct font_not_found : std::runtime_error
{
    font_not_found(std::string);
};

struct font_atlas
{
    font_atlas(std::string font_path, int ptsize);
    ~font_atlas();

    // render a text with an optional width specification
    std::unique_ptr<SDL_Surface, void(*)(SDL_Surface *)> text(std::string t, int max_line_width = -1);

    vec text_size(std::string t, int max_line_width = -1);
    int text_minimum_width(std::string t);

    unsigned int font_height() const;

    int font_line_skip() const;

    void clear();

    private:

    int get_word_left_kerning(std::string const & word);
    int get_word_right_kerning(std::string const & word);

    SDL_Surface * word(std::string);

    std::unordered_map<std::string, SDL_Surface *> _prerendered;
    TTF_Font * _font;
    int _space_advance;
    int _space_minx;
};

#endif

