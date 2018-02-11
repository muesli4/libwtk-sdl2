#ifndef COLOR_WIDGET_HPP
#define COLOR_WIDGET_HPP

#include "selectable.hpp"

struct color_widget : selectable
{
    color_widget();

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_up_event(mouse_up_event const & e) override;
    void on_key_event(key_event const & e) override;
    void on_activate() override;
    vec min_size_hint() const override;

    ~color_widget() override;

    private:
    void recolor();

    int _color_src;
};

#endif

