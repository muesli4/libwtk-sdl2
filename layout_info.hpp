#ifndef LAYOUT_INFO_HPP
#define LAYOUT_INFO_HPP

#include "font_atlas.hpp"

struct layout_info
{
    layout_info(font_atlas & fa);

    // fonts
    vec text_size(std::string t, int max_line_width = -1) const;
    int text_minimum_width(std::string t) const;
    unsigned int font_height() const;
    int font_line_skip() const;

    // theme

    private:

    // mutable is a necessary evil, but doesn't change it in a meaningful way
    mutable std::reference_wrapper<font_atlas> _fa;
};

#endif

