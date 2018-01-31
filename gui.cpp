// TODO think about a way where the widget might not arbitrarily draw to the
//      screen (basically bounding box drawn in offset mode)
//
// TODO 2-dimensional focus map to navigate gui elements with a remote:
//      NEXT_X, NEXT_Y, PREV_X, PREV_Y, NEXT, PREV
//


// - Out of consistency widgets should not react to MOUSEUP events when they
//   haven't been activted by a MOUSEDOWN event in their area before. However,
//   They might very well react to events that are not in their area if the
//   mouse went down in their area before.

#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <iostream>
#include <exception>

#include "font_atlas.hpp"
#include "draw_context.hpp"
#include "widget.hpp"
#include "button.hpp"
#include "padding.hpp"
#include "color_widget.hpp"

SDL_Rect pad_box(SDL_Rect box, int padding)
{
    return { box.x + padding, box.y + padding, box.w - 2 * padding, box.h - 2 * padding };
}

struct widget_child_descriptor
{
    // determines which widgets get size (currently only the highest priority
    // gets size, since widgets don't tell their required size)
    int priority;
    widget_ptr wptr;
};

struct container : widget
{
    container(std::initializer_list<widget_ptr> ws)
        : _children(ws)
    {
        mark_dirty();
    }

    void on_draw(draw_context & dc) const override
    {
        for (auto cptr : _children)
            cptr->on_draw(dc);
    }

    void on_mouse_event(mouse_event const & me) override
    {
        // TODO need logic to capture mouse moves outside of a widget
        for (auto cptr : _children)
        {
            cptr->on_mouse_event(me);
        }
        mark_dirty();
    }

    void on_key_event(key_event const & ke) override
    {
        // TODO need notion of focus
    }

    void apply_layout_to_children() override
    {
        std::size_t const n = _children.size();

        if (n > 0)
        {
            // evenly split box
            int const child_width = _box.w / n;
            int xoffset = _box.x;

            for (auto cptr : _children)
            {
                cptr->apply_layout({ xoffset, _box.y, child_width, _box.h });
                xoffset += child_width;
            }

        }

        // TODO set assigned box (this should be done in widget)
    }

    protected:

    std::vector<widget_ptr> _children;
};

void event_loop(SDL_Window * window)
{
    font_atlas fa("/usr/share/fonts/TTF/DejaVuSans.ttf", 15);
    draw_context dc(window, fa);

    //color_widget cw;
    container main_widget
        { std::make_shared<color_widget>()
        , pad(10, std::make_shared<color_widget>())
        , pad(20, 80, std::make_shared<color_widget>())
        , pad(10, std::make_shared<button>("Click me!", [](){ std::cout << "click1" << std::endl;}))
        , pad(10, std::make_shared<button>("Click me!", [](){ std::cout << "click2" << std::endl;}))
        };

    main_widget.apply_layout(pad_box({0, 0, dc.width(), dc.height()}, 200));

    // draw initial state
    main_widget.on_draw(dc);
    dc.present();

    SDL_Event ev;
    while (SDL_WaitEvent(&ev))
    {
        // event handling
        if (ev.type == SDL_QUIT)
            break;
        else if (ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP)
        {
            main_widget.on_mouse_event(mouse_event_from_sdl(ev.button));
        }
        else if (ev.type == SDL_KEYDOWN)
            main_widget.on_key_event(key_event());

        // render
        main_widget.draw_when_dirty(dc);
        dc.present();
    }
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    // font rendering setup
    if (TTF_Init() == -1)
    {
        std::cerr << "Could not initialize font rendering:"
                  << TTF_GetError() << '.' << std::endl;
        std::exit(0);
    }

    SDL_Window * window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 1000, 0);

    if (window == nullptr)
    {
        std::cerr << "Could not open window:"
                  << SDL_GetError() << '.' << std::endl;
    }
    else
    {
        event_loop(window);
    }

    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}

