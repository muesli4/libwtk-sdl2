#ifndef LIBWTK_SDL2_CONTEXT_INFO_HPP
#define LIBWTK_SDL2_CONTEXT_INFO_HPP

#include "font_word_cache.hpp"

struct context_info
{
    context_info(font_word_cache & fwc, int swipe_lower_threshold, double dir_unambig_factor);

    // fonts
    vec text_size(std::string t, int max_line_width = -1) const;
    int text_minimum_width(std::string t) const;
    unsigned int font_height() const;
    int font_line_skip() const;

    // theme


    // input
    double const dir_unambig_factor;
    int const swipe_lower_threshold;

    private:

    // mutable is a necessary evil, but doesn't change it in a meaningful way
    mutable std::reference_wrapper<font_word_cache> _fwc;
};

#endif

