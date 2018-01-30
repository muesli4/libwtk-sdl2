// TODO think about a way where the widget might not arbitrarily draw to the
//      screen (basically bounding box drawn in offset mode)
#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <iostream>
#include <exception>

#include "font_atlas.hpp"
#include "draw_context.hpp"
#include "widget.hpp"
#include "color_widget.hpp"

SDL_Rect pad_box(SDL_Rect box, int padding)
{
    return { box.x + padding, box.y + padding, box.w - 2 * padding, box.h - 2 * padding };
}

typedef std::shared_ptr<widget> widget_ptr;

struct container : widget
{
    container(std::initializer_list<widget_ptr> ws)
        : _children(ws)
    {
        mark_dirty();
    }

    protected:

    std::vector<widget_ptr> _children;
};

void event_loop(SDL_Window * window)
{
    font_atlas fa("/usr/share/fonts/TTF/DejaVuSans.ttf", 15);
    draw_context dc(window, fa);

    //button main_widget("Click me!", [](){ std::cout << "click" << std::endl;});
    color_widget main_widget;

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
            mouse_event me { ev.button.type == SDL_MOUSEBUTTONDOWN ? mouse_event::button_type::DOWN : mouse_event::button_type::UP, ev.button.x, ev.button.y };

            main_widget.on_mouse_event(me);
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

