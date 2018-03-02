#ifndef LIBWTK_SDL2_EMPTY_HPP
#define LIBWTK_SDL2_EMPTY_HPP

#include "widget.hpp"

struct empty : widget
{
    empty();
    ~empty() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    size_hint get_size_hint(int width, int height) const override;
};

#endif

