#ifndef LIBWTK_SDL2_WIDGET_HPP
#define LIBWTK_SDL2_WIDGET_HPP

#include <memory>
#include <optional>
#include <vector>

#include "context_info.hpp"
#include "draw_context.hpp"
#include "geometry.hpp"
#include "key_event.hpp"
#include "mouse_event.hpp"
#include "navigation_type.hpp"
#include "selection_context.hpp"
#include "swipe.hpp"

struct widget;

typedef std::shared_ptr<widget> widget_ptr;

/* TODO add improved layout
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
*/

struct widget
{
    widget();
    widget(widget const &) = delete;
    widget & operator=(widget const &) = delete;

    virtual void on_draw(draw_context & dc, selection_context const & sc) const = 0;
    virtual void on_mouse_up_event(mouse_up_event const & e);
    virtual void on_mouse_down_event(mouse_down_event const & e);
    virtual void on_mouse_move_event(mouse_move_event const & e);
    virtual void on_key_event(key_event const & e);

    // A widget may get activated by other means (e.g., infrared remote or
    // return key).
    virtual void on_activate();

    void draw_when_dirty(draw_context & dc, selection_context const & sc);
    void mark_dirty();

    // Get the widget's assigned bounding box.
    SDL_Rect const & get_box() const;

    // Get the layout information associated with the widgets context. This is
    // meant for font settings or available area to help with widget layout and
    // drawing.
    context_info const & get_context_info() const;

    // Should not be set manually if you do not know what you're doing.
    void set_context_info(context_info const & ci);

    // Should not be set manually. Containers are responsible for linking their
    // children to them.
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
    // should return the first selectable child widget. Otherwise nullptr is
    // returned.
    //
    // The point may serve as a hint to provide a more accurate selection.
    virtual widget * find_selectable(navigation_type nt, point center);

    // Perform a navigation request from this widget as source.
    //
    // A widget that is not selectable doesn't have to implement this.
    // Containers on the other hand should implement this to enable navigation
    // of their children.
    //
    // The point may serve as a hint to provide a more accurate selection.
    virtual widget * navigate_selectable(navigation_type nt, point center);

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

    // This should return the best possible approximation of the size the widget
    // will require in its minimal state. Any containers are responsible to
    // follow these constraints as best as possible. It is however in no way
    // guaranteed.
    virtual vec min_size_hint() const = 0;


    // This should give the additional size necessary to get the natural width
    // of a widget. This is the preferred size of the widget where it looks
    // especially good and is easily usable.
    virtual vec nat_size_inc_hint() const;

    // A widget may better estimate its size with a given width. For example,
    // when text is used it is much easier to estimate its size.
    //
    // Containers that know their width should prefer this to test children.
    //
    // Returns a negative value when not supported by the widget.
    virtual int height_for_width_hint(int width) const;

    virtual ~widget();

    private:

    bool _dirty;

    protected:

    // Helper to implement widgets that support swipes. Uses the default values
    // of the context to generate a possible direction from a mouse up event.
    std::optional<swipe_info> get_swipe_info_with_context_info(mouse_up_event const & e);

    // Helper to continue a navigation in a parent once the current widget is
    // exhausted.
    widget * navigate_selectable_parent(navigation_type nt, point center);

    widget * _parent;

    private:

    // cached for efficiency, may be replaced by just width and height
    SDL_Rect _box;

    context_info const * _context_info;
};

#endif

