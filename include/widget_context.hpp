#ifndef LIBWTK_SDL2_WIDGET_CONTEXT_HPP
#define LIBWTK_SDL2_WIDGET_CONTEXT_HPP

#include <optional>
#include <string>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_video.h>

#include "context_info.hpp"
#include "draw_context.hpp"
#include "font.hpp"
#include "font_word_cache.hpp"
#include "mouse_tracker.hpp"
#include "selection_context.hpp"

struct widget;

struct widget_context
{

    widget_context(SDL_Renderer * renderer, font f, widget & main_widget);
    widget_context(SDL_Renderer * renderer, font f, widget & main_widget, SDL_Rect box);

    void process_event(SDL_Event & ev);

    void draw(bool only_when_dirty = true, bool present = true);

    void select_widget(widget & w);
    void unselect_widget();
    bool is_selected_widget(widget const & w);

    void change_widget_area(SDL_Rect new_box);

    // forwarded
    void activate();
    void navigate_selection(navigation_type nt);

    private:

    void set_context_info(widget *);

    SDL_Rect _box;
    SDL_Renderer * _renderer;
    font_word_cache _fwc;
    draw_context _dc;
    selection_context _sc;
    mouse_tracker _mt;
    widget & _main_widget;
    context_info _context_info;
    std::optional<vec> _mouse_down_position;
};

#endif

