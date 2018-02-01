#ifndef PADDING_HPP
#define PADDING_HPP

#include "widget.hpp"

// TODO good idea?
struct padding : widget
{
    padding(int pad, widget_ptr wptr);
    padding(int pad_x, int pad_y, widget_ptr wptr);

    void on_draw(draw_context & dc, selection_context const & sc) const override;

    void on_mouse_event(mouse_event const & me) override;

    void on_key_event(key_event const & ke) override;

    void apply_layout_to_children() override;

    widget * find_selectable(navigation_type nt) override;
    widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center) override;

    ~padding() override;

    private:
    int _pad_x;
    int _pad_y;
    widget_ptr _wptr;
};

std::shared_ptr<padding> pad(int pad_x, int pad_y, widget_ptr wptr);
std::shared_ptr<padding> pad(int pad, widget_ptr wptr);

#endif

