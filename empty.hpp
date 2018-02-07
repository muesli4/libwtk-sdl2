#ifndef EMPTY_HPP
#define EMPTY_HPP

#include "widget.hpp"

struct empty : widget
{
    empty();
    ~empty() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    vec min_size_hint() const override;
};

#endif

