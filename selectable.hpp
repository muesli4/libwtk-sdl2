#ifndef SELECTABLE_HPP
#define SELECTABLE_HPP

#include "widget.hpp"

struct selectable : widget
{
    widget * find_selectable(navigation_type nt) override;
    widget * navigate_selectable(navigation_type nt) override;

    void on_select() override;
    void on_unselect() override;

    ~selectable() override;
};

#endif

