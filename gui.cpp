// TODO think about a way where the widget might not arbitrarily draw to the
//      screen (basically bounding box drawn in offset mode)

// TODO implement mouse move event for several widgets

// TODO should mouse click select selectable widgets? if so, the mouse_down
//      event will have to go to a unqiue widget. This also makes sense when
//      considering the fact that widgets should only respond to mouse down
//      events within their box.

// TODO provide means to add widgets outside of constructors (redo layout only for local container or upwards?)
// TODO should widgets relayout when their content changes?
// TODO find something similar to width-for-height/heigh-for-width

// - Out of consistency widgets should not react to MOUSEUP events when they
//   haven't been activted by a MOUSEDOWN event in their area before. However,
//   They might very well react to events that are not in their area if the
//   mouse went down in their area before.

#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <iostream>
#include <exception>
#include <optional>

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
#include "list_view.hpp"
#include "notebook.hpp"
#include "label.hpp"
#include "empty.hpp"
#include "widget_context.hpp"

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
    std::vector<std::string> test_values{"a", "b", "c", "d", "testwdfkosadjflkajskdfjlaskdjflkasdjdfklajsdlkfjasldkdfjflkasddjflkdsjlfkjdsalkkfjdkk", "test1", "test2", "a very long string this is indeed", "foo", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a"};

    auto nb = std::make_shared<notebook, std::initializer_list<widget_ptr>>
        ({ std::make_shared<color_widget>()
         , std::make_shared<swipe_area>([](swipe_action act){ std::cout << "swipe = " << (int)act << std::endl; }, [](){ std::cout << "press" << std::endl; })
         });

    auto lv = std::make_shared<list_view>(0, 2, 4, test_values, [](){ std::cout << "press list_view" << std::endl; });

    padding main_widget(20, hbox(
        { { true, vbox({ { true, lv }, { false, nb } }, 20, false) }
        , { true, std::make_shared<color_widget>() }
        , { false, vbox( { { false, std::make_shared<button>("Button 1", [](){ std::cout << "click1" << std::endl; }) }
                         , { false, pad(20, 80, std::make_shared<color_widget>())}
                         , { true, std::make_shared<label>("This text should hopefully produce a linebreak. Otherwise something is not working correctly.\n\nYou may use Tab and Shift+Tab to focus widgets or use Shift and the corresponding arrow key for a 2-dimensional direction.") }
                         }, 20, true) }
        , { false, vbox( { { false, std::make_shared<button>("Color", [nb](){ nb->set_page(0); })}
                         , { false, std::make_shared<button>("Swipe", [nb](){ nb->set_page(1); })}
                         , { true, std::make_shared<empty>() }
                         , { false, std::make_shared<button>("Quit", [](){ SDL_Event ev { .type = SDL_QUIT }; SDL_PushEvent(&ev); })}
                         }
                       , 20, false) }
        }, 20, false));


    // setup necessary context (as in local to a window or other unit of management)
    widget_context ctx(window, { "/usr/share/fonts/TTF/DejaVuSans.ttf", 15 }, main_widget);

    // draw initial state
    ctx.draw();

    SDL_Event ev;
    while (SDL_WaitEvent(&ev))
    {
        if (ev.type == SDL_QUIT)
            break;
        else if (ev.type == SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_CTRL) && ev.key.keysym.sym == SDLK_q)
            break;
        else
        {
            ctx.process_event(ev);
        }

        // render
        ctx.draw();
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

