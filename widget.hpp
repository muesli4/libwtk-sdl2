#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "draw_context.hpp"
#include "mouse_event.hpp"
#include "key_event.hpp"

struct widget
{
    virtual void on_draw(draw_context & dc) const = 0;
    virtual void on_mouse_event(mouse_event const & e) = 0;
    virtual void on_key_event(key_event const & e) = 0;

    void draw_when_dirty(draw_context & dc);
    void mark_dirty();

    SDL_Rect const & box() const;

    // in the simplest case this just sets the bounding box but may involve more
    // complex calculations with containers
    virtual void apply_layout(SDL_Rect box);

    private:

    bool _dirty;

    protected:

    // cached for efficiency
    SDL_Rect _box;
};

#endif
