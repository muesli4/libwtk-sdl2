#ifndef LIBWTK_SDL2_FONT_MANAGER_HPP
#define LIBWTK_SDL2_FONT_MANAGER_HPP

#include <string>
#include <vector>

#include <SDL2/SDL_render.h>

#include "font.hpp"
#include "font_word_cache.hpp"
#include "geometry.hpp"

struct font_manager
{
    font_manager(SDL_Renderer * renderer, std::vector<font> fonts);
    font_manager(font_manager const &) = delete;
    font_manager & operator=(font_manager const &) = delete;

    std::tuple<vec, std::vector<copy_command>> text(std::string t, int max_line_width = -1, int font_idx = 0);
    vec text_size(std::string t, int max_line_width = -1, int font_idx = 0);
    int text_minimum_width(std::string t, int font_idx = 0);
    unsigned int font_height(int font_idx = 0) const;
    int font_line_skip(int font_idx = 0) const;

    private:

    SDL_Renderer * _renderer;
    std::vector<font_word_cache> _font_word_caches;

};

#endif

