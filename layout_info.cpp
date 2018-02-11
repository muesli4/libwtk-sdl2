#include "layout_info.hpp"

layout_info::layout_info(font_word_cache & fwc)
    : _fwc(fwc)
{
}

vec layout_info::text_size(std::string t, int max_line_width) const
{
    return _fwc.get().text_size(t, max_line_width);
}

int layout_info::text_minimum_width(std::string t) const
{
    return _fwc.get().text_minimum_width(t);
}

unsigned int layout_info::font_height() const
{
    return _fwc.get().font_height();
}

int layout_info::font_line_skip() const
{
    return _fwc.get().font_line_skip();
}

