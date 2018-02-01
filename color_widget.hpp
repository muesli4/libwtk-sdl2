#ifndef COLOR_WIDGET_HPP
#define COLOR_WIDGET_HPP

#include "widget.hpp"

struct color_widget : widget
{
    color_widget();

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_event(mouse_event const & e) override;
    void on_key_event(key_event const & e) override;
    void on_activate() override;
    widget * find_selectable() override;
    widget * navigate_selectable(navigation_type nt) override;

    ~color_widget() override;

    private:
    void recolor();

    int _color_src;
};

#endif

