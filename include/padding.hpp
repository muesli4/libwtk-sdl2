#ifndef PADDING_HPP
#define PADDING_HPP

#include "bin.hpp"

struct padding : bin
{
    padding(int pad, widget_ptr child);
    padding(int pad_x, int pad_y, widget_ptr child);
    padding(int pad_left, int pad_right, int pad_top, int pad_bottom, widget_ptr child);

    ~padding() override;

    void apply_layout_to_children() override;

    vec min_size_hint() const override;

    // padding interface
    void set_pad_left(int pad_left);
    void set_pad_right(int pad_right);
    void set_pad_top(int pad_top);
    void set_pad_bottom(int pad_bottom);

    private:

    int _pad_left;
    int _pad_right;
    int _pad_top;
    int _pad_bottom;
};


std::shared_ptr<padding> pad(int pad_x, int pad_y, widget_ptr wptr);
std::shared_ptr<padding> pad(int pad, widget_ptr wptr);

std::shared_ptr<padding> pad_left(int pad, widget_ptr wptr);
std::shared_ptr<padding> pad_right(int pad, widget_ptr wptr);
std::shared_ptr<padding> pad_top(int pad, widget_ptr wptr);
std::shared_ptr<padding> pad_bottom(int pad, widget_ptr wptr);

// overloads to just modify, works because widgets are passed by reference
std::shared_ptr<padding> pad_left(int pad, std::shared_ptr<padding> wptr);
std::shared_ptr<padding> pad_right(int pad, std::shared_ptr<padding> wptr);
std::shared_ptr<padding> pad_top(int pad, std::shared_ptr<padding> wptr);
std::shared_ptr<padding> pad_bottom(int pad, std::shared_ptr<padding> wptr);

#endif

