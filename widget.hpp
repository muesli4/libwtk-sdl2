#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <memory>

#include "draw_context.hpp"
#include "mouse_event.hpp"
#include "key_event.hpp"

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

    // In the simplest case this just sets the bounding box but may involve more
    // complex calculations with containers.
    void apply_layout(SDL_Rect box);

    // Refresh layout with existing box. This should not be called if the widget
    // does not have box already. Otherwise no space is allocated.
    virtual void apply_layout_to_children();

    private:

    bool _dirty;

    protected:

    // cached for efficiency FIXME ?
    SDL_Rect _box;
};

#endif

