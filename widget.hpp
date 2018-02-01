#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <memory>

#include "draw_context.hpp"
#include "mouse_event.hpp"
#include "key_event.hpp"

enum class navigation_type { NEXT_X, NEXT_Y, PREV_X, PREV_Y, NEXT, PREV };

struct widget;

typedef std::shared_ptr<widget> widget_ptr;

struct widget
{
    virtual void on_draw(draw_context & dc) const = 0;
    virtual void on_mouse_event(mouse_event const & e);
    virtual void on_key_event(key_event const & e);

    // a widget may get activated by other means (e.g., infrared remote)
    virtual void on_activate();

    void draw_when_dirty(draw_context & dc);
    void mark_dirty();

    SDL_Rect const & box() const;

    void set_parent(widget * parent);

    // In the simplest case this just sets the bounding box but may involve more
    // complex calculations with containers.
    void apply_layout(SDL_Rect box);

    // Refresh layout with existing box. This should not be called if the widget
    // does not have box already. Otherwise no space is allocated.
    virtual void apply_layout_to_children();

    // Should return the first selectable widget.
    virtual widget * find_selectable();

    // Perform a navigation request from this widget as source.
    virtual widget * navigate_selectable(navigation_type nt);

    // When a child widget has exhausted its selectable widgets it may ask its
    // parent for the next widget according to the navigation. The coordinates
    // of the widget are provided to get a more accurate result for some
    // navigation types (i.e. 2-dimensional).
    virtual widget * navigate_selectable_from_children(navigation_type nt, widget * w, int center_x, int center_y);

    private:

    bool _dirty;

    protected:

    widget * _parent;

    // cached for efficiency, may be replaced by just width and height
    SDL_Rect _box;
};

#endif

