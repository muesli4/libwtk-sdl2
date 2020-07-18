#ifndef LIBWTK_SDL2_WIDGET_CONTEXT_HPP
#define LIBWTK_SDL2_WIDGET_CONTEXT_HPP

#include <optional>
#include <string>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

#include "context_info.hpp"
#include "draw_context.hpp"
#include "font.hpp"
#include "font_word_cache.hpp"
#include "font_manager.hpp"
#include "geometry.hpp"
#include "mouse_tracker.hpp"
#include "selection_context.hpp"

struct widget;

struct widget_context
{

    widget_context(SDL_Renderer * renderer, std::vector<font> fonts, widget & main_widget);
    widget_context(SDL_Renderer * renderer, std::vector<font> fonts, widget & main_widget, rect box);
    widget_context(SDL_Renderer * renderer, std::vector<font> fonts, double dir_unambig_factor_threshold, widget & main_widget);
    widget_context(SDL_Renderer * renderer, std::vector<font> fonts, double dir_unambig_factor_threshold, widget & main_widget, rect box);

    /**
     * @name Pre-defined input event handling.
     * @{
     */

    /**
     * Process a mouse event and propagate it to the widget.
     *
     * @param ev The SDL event.
     * @param Whether the event was handled by this method.
     */
    bool process_mouse_event(SDL_Event const & ev);

    bool process_key_event(SDL_Event const & ev);

    /**
     * Default event handler that reacts to basic input events.
     * @return Has the event been handled?
     */
    bool process_event(SDL_Event const & ev);

    /**
     * @}
     */

    /**
     * Resets any state that is associated with mouse event handling. This
     * might come in handy if the application decides to not propagate events
     * for some time.
     */
    void reset_mouse_state();

    void draw(bool present = true);
    void draw_dirty(int dirty_redraws = 1);

    void select_widget(widget & w);
    void unselect_widget();
    bool is_selected_widget(widget const & w);

    void change_widget_area(rect new_box);

    void activate();
    void navigate_selection(navigation_type nt);

    private:

    void set_context_info(widget *);

    rect _box;
    SDL_Renderer * _renderer;
    font_manager _fm;
    draw_context _dc;
    selection_context _sc;
    mouse_tracker _mt;
    widget & _main_widget;
    context_info _context_info;
    std::optional<vec> _mouse_down_position;
};

#endif

