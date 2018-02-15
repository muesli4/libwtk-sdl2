#include "padding.hpp"

padding::padding(int pad, widget_ptr child) : padding(pad, pad, child)
{
}

padding::padding(int pad_x, int pad_y, widget_ptr child)
    : padding(pad_x, pad_x, pad_y, pad_y, child)
{
}

padding::padding(int pad_left, int pad_right, int pad_top, int pad_bottom, widget_ptr child)
    : bin(child)
    , _pad_left(std::max(pad_left, 0))
    , _pad_right(std::max(pad_right, 0))
    , _pad_top(std::max(pad_top, 0))
    , _pad_bottom(std::max(pad_bottom, 0))
{
}

padding::~padding()
{
}

void padding::apply_layout_to_children()
{
    _child->apply_layout(
        { get_box().x + _pad_left
        , get_box().y + _pad_top
        , get_box().w - _pad_left - _pad_right
        , get_box().h - _pad_top - _pad_bottom
        });
}

vec padding::min_size_hint() const
{
    vec v = _child->min_size_hint();

    v.w += _pad_left + _pad_right;
    v.h += _pad_top + _pad_bottom;

    return v;
}

void padding::set_pad_left(int pad_left)
{
    _pad_left = std::max(0, pad_left);
}

void padding::set_pad_right(int pad_right)
{
    _pad_right = std::max(0, pad_right);
}

void padding::set_pad_top(int pad_top)
{
    _pad_top = std::max(0, pad_top);
}

void padding::set_pad_bottom(int pad_bottom)
{
    _pad_bottom = std::max(0, pad_bottom);
}

std::shared_ptr<padding> pad(int pad_x, int pad_y, widget_ptr wptr)
{
    return std::make_shared<padding>(pad_x, pad_y, wptr);
}

std::shared_ptr<padding> pad(int pad, widget_ptr wptr)
{
    return std::make_shared<padding>(pad, wptr);
}

std::shared_ptr<padding> pad_left(int pad, widget_ptr wptr)
{
    return std::make_shared<padding>(pad, 0, 0, 0, wptr);
}

std::shared_ptr<padding> pad_right(int pad, widget_ptr wptr)
{
    return std::make_shared<padding>(0, pad, 0, 0, wptr);
}

std::shared_ptr<padding> pad_top(int pad, widget_ptr wptr)
{
    return std::make_shared<padding>(0, 0, pad, 0, wptr);
}

std::shared_ptr<padding> pad_bottom(int pad, widget_ptr wptr)
{
    return std::make_shared<padding>(0, 0, 0, pad, wptr);
}


std::shared_ptr<padding> pad_left(int pad, std::shared_ptr<padding> wptr)
{
    wptr->set_pad_left(pad);
    return wptr;
}

std::shared_ptr<padding> pad_right(int pad, std::shared_ptr<padding> wptr)
{
    wptr->set_pad_right(pad);
    return wptr;
}

std::shared_ptr<padding> pad_top(int pad, std::shared_ptr<padding> wptr)
{
    wptr->set_pad_top(pad);
    return wptr;
}

std::shared_ptr<padding> pad_bottom(int pad, std::shared_ptr<padding> wptr)
{
    wptr->set_pad_bottom(pad);
    return wptr;
}

