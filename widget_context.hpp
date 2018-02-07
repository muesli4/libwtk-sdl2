#ifndef WIDGET_CONTEXT_HPP
#define WIDGET_CONTEXT_HPP

#include <string>
#include <SDL2/SDL.h>

#include "font_atlas.hpp"
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

    widget_context(SDL_Window * window, font f, widget & main_widget);
    widget_context(SDL_Window * window, SDL_Rect box, font f, widget & main_widget);

    void process_event(SDL_Event & ev);

    void draw(bool only_when_dirty = true, bool present = true);

    void select_widget(widget & w);
    void unselect_widget();
    bool is_selected_widget(widget const & w);

    private:

    void set_layout_info(widget *);

    font_atlas _fa;
    draw_context _dc;
    selection_context _sc;
    swipe_detector _sd;
    widget & _main_widget;
    layout_info _layout_info;
};

#endif

