#ifndef LIBWTK_SDL2_LAYOUT_INFO_HPP
#define LIBWTK_SDL2_LAYOUT_INFO_HPP

#include "font_word_cache.hpp"

struct layout_info
{
    layout_info(font_word_cache & fwc);

    // fonts
    vec text_size(std::string t, int max_line_width = -1) const;
    int text_minimum_width(std::string t) const;
    unsigned int font_height() const;
    int font_line_skip() const;

    // theme

    private:

    // mutable is a necessary evil, but doesn't change it in a meaningful way
    mutable std::reference_wrapper<font_word_cache> _fwc;
};

#endif

