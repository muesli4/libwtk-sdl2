#include "context_info.hpp"

context_info::context_info(font_word_cache & fwc, int swipe_lower_threshold, double dir_unambig_factor)
    : dir_unambig_factor(dir_unambig_factor)
    , swipe_lower_threshold(swipe_lower_threshold)
    , _fwc(fwc)
{
}

vec context_info::text_size(std::string t, int max_line_width) const
{
    return _fwc.get().text_size(t, max_line_width);
}

int context_info::text_minimum_width(std::string t) const
{
    return _fwc.get().text_minimum_width(t);
}

unsigned int context_info::font_height() const
{
    return _fwc.get().font_height();
}

int context_info::font_line_skip() const
{
    return _fwc.get().font_line_skip();
}

