#ifndef PADDING_HPP
#define PADDING_HPP

#include "bin.hpp"

struct padding : bin
{
    padding(int pad, widget_ptr child);
    padding(int pad_x, int pad_y, widget_ptr child);

    void apply_layout_to_children() override;

    vec min_size_hint() const override;

    ~padding() override;

    private:

    int _pad_x;
    int _pad_y;
};

std::shared_ptr<padding> pad(int pad_x, int pad_y, widget_ptr wptr);
std::shared_ptr<padding> pad(int pad, widget_ptr wptr);

#endif

