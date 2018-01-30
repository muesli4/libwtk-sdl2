#ifndef COLOR_WIDGET_HPP
#define COLOR_WIDGET_HPP

#include "widget.hpp"

struct color_widget : widget
{
    color_widget();

    void on_draw(draw_context & dc) const;
    void on_mouse_event(mouse_event const & e);
    void on_key_event(key_event const & e);

    private:
    void recolor();

    int _color_src;
};

#endif

