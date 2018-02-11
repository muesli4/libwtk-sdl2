#ifndef BIN_HPP
#define BIN_HPP

#include "widget.hpp"

// Base class for containers that have a single child.
struct bin : widget
{
    bin(widget_ptr child);

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_down_event(mouse_down_event const & me) override;
    void on_mouse_up_event(mouse_up_event const & me) override;

    void apply_layout_to_children() override = 0;

    widget * find_selectable(navigation_type nt, point center) override;
    widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center) override;

    // Should be provided when geometry of the child changes.
    vec min_size_hint() const override;

    int height_for_width_hint(int width) const;

    std::vector<widget *> get_children() override;
    std::vector<widget const *> get_children() const override;

    ~bin() override;

    protected:

    widget_ptr _child;
};

#endif

