#ifndef LIBWTK_SDL2_WIDGET_CONTEXT_HPP
#define LIBWTK_SDL2_WIDGET_CONTEXT_HPP

#include <string>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_events.h>

#include "font_word_cache.hpp"
#include "draw_context.hpp"
#include "selection_context.hpp"
#include "swipe_detector.hpp"
#include "layout_info.hpp"

struct widget;

// TODO use several fonts
struct font
{
    std::string path;
    unsigned int size;
};

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

    private:

    void set_layout_info(widget *);

    SDL_Rect _box;
    SDL_Renderer * _renderer;
    font_word_cache _fwc;
    draw_context _dc;
    selection_context _sc;
    swipe_detector _sd;
    widget & _main_widget;
    layout_info _layout_info;
};

#endif

