#ifndef PADDING_HPP
#define PADDING_HPP

#include "widget.hpp"

// TODO good idea?
struct padding : widget
{
    padding(int pad, widget_ptr wptr);
    padding(int pad_x, int pad_y, widget_ptr wptr);

    void on_draw(draw_context & dc) const override;

    void on_mouse_event(mouse_event const & me) override;

    void on_key_event(key_event const & ke) override;

    void apply_layout(SDL_Rect box);
    
    private:
    int _pad_x;
    int _pad_y;
    widget_ptr _wptr;
};

std::shared_ptr<padding> pad(int pad_x, int pad_y, widget_ptr wptr);
std::shared_ptr<padding> pad(int pad, widget_ptr wptr);

#endif

