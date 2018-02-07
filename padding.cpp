#include "padding.hpp"

padding::padding(int pad, widget_ptr child) : padding(pad, pad, child)
{
}

padding::padding(int pad_x, int pad_y, widget_ptr child)
    : bin(child)
    , _pad_x(pad_x)
    , _pad_y(pad_y)
{
}

void padding::apply_layout_to_children()
{
    _child->apply_layout(
        { get_box().x + _pad_x
        , get_box().y + _pad_y
        , get_box().w - 2 * _pad_x
        , get_box().h - 2 * _pad_y
        });
}

vec padding::min_size_hint() const
{
    vec v = _child->min_size_hint();

    v.w += 2 * _pad_x;
    v.h += 2 * _pad_y;

    return v;
}

padding::~padding()
{
}

std::shared_ptr<padding> pad(int pad_x, int pad_y, widget_ptr wptr)
{
    return std::make_shared<padding>(pad_x, pad_y, wptr);
}

std::shared_ptr<padding> pad(int pad, widget_ptr wptr)
{
    return std::make_shared<padding>(pad, wptr);
}

