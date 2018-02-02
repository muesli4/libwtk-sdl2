// TODO think about a way where the widget might not arbitrarily draw to the
//      screen (basically bounding box drawn in offset mode)

// - Out of consistency widgets should not react to MOUSEUP events when they
//   haven't been activted by a MOUSEDOWN event in their area before. However,
//   They might very well react to events that are not in their area if the
//   mouse went down in their area before.

#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <iostream>
#include <exception>
#include <algorithm>

#include "font_atlas.hpp"
#include "draw_context.hpp"
#include "selection_context.hpp"
#include "widget.hpp"
#include "button.hpp"
#include "padding.hpp"
#include "color_widget.hpp"
#include "box.hpp"
#include "swipe_area.hpp"
#include "swipe_detector.hpp"

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

/*
struct table : container
{
    struct entry
    {
        SDL_Rect placement;
        widget_ptr wptr;
    };

    table(std::initializer_list<entry> es)
    {
    }

    std::vector<std::vector<widget_ptr>> _table_fragments;

};
*/


void event_loop(SDL_Window * window)
{
    //color_widget cw;
    /*
    box main_widget(
        box::orientation::HORIZONTAL,
        { std::make_shared<color_widget>()
        , pad(10, std::make_shared<color_widget>())
        ,  pad(20, 80, std::make_shared<color_widget>())
        , pad(10, std::make_shared<button>("Button 1", [](){ std::cout << "click1" << std::endl;}))
        , std::make_shared<box, box::orientation, std::initializer_list<widget_ptr>>(
                box::orientation::VERTICAL,
                { pad(10, std::make_shared<button>("Button 2", [](){ std::cout << "click2" << std::endl;}))
                , pad(10, std::make_shared<button>("Button 3", [](){ std::cout << "click3" << std::endl;}))
                , pad(10, std::make_shared<button>("Button 4", [](){ std::cout << "click4" << std::endl;}))
                })
        });
    */

    swipe_area main_widget([](swipe_action act){ std::cout << "swipe: " << (int)act << std::endl; }, [](){ std::cout << "press" << std::endl; } );

    // setup necessary contexts (as in local to a window or other unit of management)
    selection_context sc;
    font_atlas fa("/usr/share/fonts/TTF/DejaVuSans.ttf", 15);
    draw_context dc(window, fa);
    swipe_detector sd(30, 0.3);

    main_widget.apply_layout(pad_box({0, 0, dc.width(), dc.height()}, 200));

    // draw initial state
    main_widget.on_draw(dc, sc);
    dc.present();

    SDL_Event ev;
    while (SDL_WaitEvent(&ev))
    {
        // event handling
        if (ev.type == SDL_QUIT)
            break;
        else if (ev.type == SDL_MOUSEBUTTONDOWN)
        {
            point p { ev.button.x, ev.button.y };
            main_widget.on_mouse_down_event({p});
            sd.mouse_down(p);
        }
        else if (ev.type == SDL_MOUSEBUTTONUP)
        {
            point p { ev.button.x, ev.button.y };
            main_widget.on_mouse_up_event(sd.mouse_up({ ev.button.x, ev.button.y }));
        }
        else if (ev.type == SDL_KEYDOWN)
        {
            auto const & keysym = ev.key.keysym;

            if ((keysym.mod & KMOD_CTRL) && keysym.sym == SDLK_q)
                break;
            else if (keysym.sym == SDLK_TAB)
            {
                if (keysym.mod & KMOD_SHIFT)
                {
                    sc.navigate_selection(navigation_type::PREV, &main_widget);
                }
                else
                {
                    sc.navigate_selection(navigation_type::NEXT, &main_widget);
                }
            }
            else if (keysym.mod & KMOD_SHIFT)
            {
                navigation_type nt;
                switch (keysym.sym)
                {
                    case SDLK_UP:    nt = navigation_type::PREV_Y; break;
                    case SDLK_DOWN:  nt = navigation_type::NEXT_Y; break;
                    case SDLK_LEFT:  nt = navigation_type::PREV_X; break;
                    case SDLK_RIGHT: nt = navigation_type::NEXT_X; break;
                    default: goto render;
                }
                sc.navigate_selection(nt, &main_widget);
            }
            else if (keysym.sym == SDLK_RETURN)
                sc.dispatch_activation();
            else
                sc.dispatch_key_event(key_event());
        }
render:
        // render
        main_widget.draw_when_dirty(dc, sc);
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

    SDL_Window * window = SDL_CreateWindow("widget test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 1000, 0);

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

