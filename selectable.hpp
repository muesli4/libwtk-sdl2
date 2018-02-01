#ifndef SELECTABLE_HPP
#define SELECTABLE_HPP

#include "widget.hpp"

struct selectable : widget
{
    widget * find_selectable() override;
    widget * navigate_selectable(navigation_type nt) override;

    ~selectable() override;
};

#endif

