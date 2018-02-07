#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <memory>
#include <vector>

#include "draw_context.hpp"
#include "mouse_event.hpp"
#include "swipe_event.hpp"
#include "key_event.hpp"
#include "selection_context.hpp"
#include "geometry.hpp"

enum class navigation_type { NEXT_X, NEXT_Y, PREV_X, PREV_Y, NEXT, PREV };

struct widget;

typedef std::shared_ptr<widget> widget_ptr;

enum class allocation_type { CONSTANT, WIDTH_FOR_HEIGHT };

struct size_hint
{
    allocation_type type;
    union
    {
        struct
        {
            int width;
            double ratio;
        } ratio;
        vec constant;
    };
};

struct widget
{
    widget();

    virtual void on_draw(draw_context & dc, selection_context const & sc) const = 0;
    virtual void on_mouse_up_event(mouse_up_event const & e);
    virtual void on_mouse_down_event(mouse_down_event const & e);
    virtual void on_key_event(key_event const & e);

    // A widget may get activated by other means (e.g., infrared remote or
    // return key).
    virtual void on_activate();

    void draw_when_dirty(draw_context & dc, selection_context const & sc);
    void mark_dirty();

    SDL_Rect const & get_box() const;
    void set_layout_info(layout_info const & li);
    layout_info const & get_layout_info() const;

    void set_parent(widget * parent);

    // Return child widgets in Z-order. Should be implemented by containers.
    virtual std::vector<widget *> get_children();
    virtual std::vector<widget const *> get_children() const;

    // In the simplest case this just sets the bounding box but may involve more
    // complex calculations with containers.
    void apply_layout(SDL_Rect box);

    // Refresh layout with existing box. This should not be called if the widget
    // does not have box already. Otherwise no space is allocated.
    virtual void apply_layout_to_children();

    // Should return the first selectable widget according to the navigation
    // type. If a widget is selectable it should return itself. Containers
    // should return the first selectable child widget.
    virtual widget * find_selectable(navigation_type nt);

    // Perform a navigation request from this widget as source.
    //
    // A widget that is not selectable doesn't have to implement this.
    // Containers on the other hand should implement this to enable selection of
    // their children.
    virtual widget * navigate_selectable(navigation_type nt);

    // When a child widget has exhausted its selectable widgets it may ask its
    // parent for the next widget according to the navigation. The coordinates
    // of the widget are provided to get a more accurate result for some
    // navigation types (i.e. 2-dimensional). The widget passed has to be the
    // direct child, otherwise the parent is not able to find it.
    //
    // Only containers should implement this.
    virtual widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center);

    virtual void on_select();
    virtual void on_unselect();

    // This should return the approximate size the widget will require in its
    // minimal state. Any containers are responsible to follow these constraints
    // as best as possible. It is however in no way guaranteed.
    //virtual size_hint get_size_hint() = 0;
    virtual vec min_size_hint() const = 0;

    virtual ~widget();

    private:

    bool _dirty;

    protected:

    // Helper to continue a navigation in a parent once the current widget is
    // exhausted.
    widget * navigate_selectable_parent(navigation_type nt, point center);

    widget * _parent;

    private:

    // cached for efficiency, may be replaced by just width and height
    SDL_Rect _box;

    layout_info const * _layout_info;
};

#endif

