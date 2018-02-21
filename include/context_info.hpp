#ifndef LIBWTK_SDL2_CONTEXT_INFO_HPP
#define LIBWTK_SDL2_CONTEXT_INFO_HPP

#include "font_manager.hpp"
#include "swipe.hpp"

struct context_info
{
    context_info(font_manager & fm, swipe_config swipe_cfg);

    // fonts
    vec text_size(std::string t, int max_line_width = -1, int font_idx = 0) const;
    int text_minimum_width(std::string t, int font_idx = 0) const;
    unsigned int font_height(int font_idx = 0) const;
    int font_line_skip(int font_idx = 0) const;

    // theme


    // input
    swipe_config const swipe_cfg;

    private:

    // mutable is a necessary evil, but doesn't change it in a meaningful way
    mutable std::reference_wrapper<font_manager> _fm;
};

#endif

