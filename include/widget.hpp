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
#include "region.hpp"
#include "selection_context.hpp"
#include "swipe.hpp"

struct widget;

/**
 * The type of smart pointer that widgets use to be stored in containers.
 */
typedef std::shared_ptr<widget> widget_ptr;

enum class dirty_type
{
    CLEAN,
    CHILD_DIRTY,
    DIRTY
};

// This will choose the maximum dirty_type that is necessary such that a widget
// tree will be properly drawn.
dirty_type combine(dirty_type a, dirty_type b);

/**
 * Abstract base class for widgets. A derived class will have to implement at
 * least \ref on_draw() and \ref min_size_hint(). There exist useful default
 * implementation for all other methods. Note that there are also several other
 * abstract classes that already implement certain features.
 *
 * Widgets are usually created as \ref widget_ptr to provide easy usage.
 */
struct widget : region
{

    widget();
    widget(widget const &) = delete;
    widget & operator=(widget const &) = delete;
    virtual ~widget();

    /**
     * @name  Drawing and Dirty-Checking
     * @{
     */

    /**
     * This is called when a child is marked as dirty. The widget may then
     * decide what to do.
     *
     * You might use \ref mark_dirty() to mark this widget dirty or
     * \ref mark_child_dirty(widget * child) to only propagate the change
     * upwards (this is the default implementation).
     */
    virtual void on_child_dirty(widget * child);

    /**
     * Return the visible children in Z-order. The default implementation
     * assumes that all children are visible.
     * Once an invisible widget gets visible again the parent is responsible to
     * mark itself at least as `dirty_type::CHILD_DIRTY` and to propagate the
     * change with \ref notify_parent_child_dirty().
     */
    virtual std::vector<widget *> get_visible_children();
    virtual std::vector<widget const *> get_visible_children() const;

    /**
     * Sets all dirty flags to clean. Drawing in general will not reset the
     * dirty state. This is because it won't have to when not using dirty-based
     * drawing and most importantly if double buffering is used the widget has
     * to be drawn two times.
     */
    void clear_dirty();

    void mark_dirty();
    void mark_child_dirty(widget * child);

    /**
     * Recursively draws all widgets without dirty checking. There is also no
     * need to call \ref clear_dirty().
     */
    void draw(draw_context & dc, selection_context const & sc) const;

    /**
     * Recursively draws all dirty widgets.
     */
    void draw_dirty(draw_context & dc, selection_context const & sc) const;

    /**
     *  Draw the widget. No children should be drawn here.
     */ 
    virtual void on_draw(draw_context & dc, selection_context const & sc) const = 0;

    /** @} */

    /**
     * @name Input Event Handlers
     * @{
     */

    virtual void on_mouse_up_event(mouse_up_event const & e);
    virtual void on_mouse_down_event(mouse_down_event const & e);
    virtual void on_mouse_move_event(mouse_move_event const & e);
    virtual void on_key_event(key_event const & e);

    /**
     *  A widget may get activated by other means (e.g., infrared remote or
     *  return key).
     */
    virtual void on_activate();

    /** @} */

    /**
     * Get the \ref context_info associated with the widgets context. This is
     * meant for font settings or available area to help with widget layout and
     * drawing.
     */
    context_info const & get_context_info() const;

    /**
     * Should not be set manually if you do not know what you're doing.
     */
    void set_context_info(context_info const & ci);

    /**
     * Should not be set manually. Containers are responsible for linking their
     * children to them.
     */
    void set_parent(widget * parent);

    /**
     * Return child widgets in Z-order. Should be implemented by containers.
     */
    virtual std::vector<widget *> get_children();
    virtual std::vector<widget const *> get_children() const;

    /**
     * @name Selection Navigation
     */

    /**
     * Should return the first selectable widget according to the navigation
     * type. If a widget is selectable it should return itself. Containers
     * should return the first selectable child widget. Otherwise nullptr is
     * returned.
     *
     * The point may serve as a hint to provide a more accurate selection.
     */
    virtual widget * find_selectable(navigation_type nt, point center);

    /**
     * Perform a navigation request from this widget as source.
     *
     * A widget that is not selectable doesn't have to implement this.
     * Containers on the other hand should implement this to enable navigation
     * of their children.
     *
     * The point may serve as a hint to provide a more accurate selection.
     */
    virtual widget * navigate_selectable(navigation_type nt, point center);

    /**
     * When a child widget has exhausted its selectable widgets it may ask its
     * parent for the next widget according to the navigation. The coordinates
     * of the widget are provided to get a more accurate result for some
     * navigation types (i.e. 2-dimensional). The widget passed has to be the
     * direct child, otherwise the parent is not able to find it.
     *
     * Only containers should implement this.
     */
    virtual widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center);

    virtual void on_select();
    virtual void on_unselect();

    /** @} */

    private:

    dirty_type _dirty;

    protected:

    /**
     * @name Helpers for custom widget implementations
     * @{
     */

    /**
     * Helper to implement widgets that support swipes. Uses the default values
     * of the context to generate a possible direction from a mouse up event.
     */
    std::optional<swipe_info> get_swipe_info_with_context_info(mouse_up_event const & e);

    /**
     * Helper to continue a navigation in a parent once the current widget is
     * exhausted.
     */
    widget * navigate_selectable_parent(navigation_type nt, point center);

    /** @} */

    widget * _parent;

    private:

    void notify_parent_child_dirty();

    context_info const * _context_info;
};

int opt_min(int opt_length, int length);
int opt_max(int opt_length, int length);
int opt_or_value(int opt_length, int length);
int opt_change(int opt_length, int length);

#endif

