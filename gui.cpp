// TODO think about a way where the widget might not arbitrarily draw to the
//      screen (basically bounding box drawn in offset mode)

// TODO implement mouse move event for several widgets
// TODO widget: add hit test for widgets, then there is no need to propagate the mouse event
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
#include "table.hpp"
#include "widget_context.hpp"

widget_ptr cw()
{
    return std::make_shared<color_widget>();
}

widget_ptr num_button()
{
    static int k = 0;
    k++;
    int n = k;
    return std::make_shared<button>(std::string("Button #") + std::to_string(n), [=](){ std::cout << "click" << n << std::endl; });
}

void event_loop(SDL_Window * window)
{
    std::vector<std::string> test_values{"a", "b", "c", "d", "testwdfkosadjflkajskdfjlaskdjflkasdjdfklajsdlkfjasldkdfjflkasddjflkdsjlfkjdsalkkfjdkk", "test1", "test2", "a very long string this is indeed", "foo", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a"};

    auto nb = std::make_shared<notebook>(std::vector<widget_ptr>
        { cw()
        , std::make_shared<swipe_area>([](swipe_action act){ std::cout << "swipe = " << (int)act << std::endl; }, [](){ std::cout << "press" << std::endl; })
        });

    auto nb_indicator = std::make_shared<label>("Active Notebook Widget: Color");
    auto nb_controls = hbox({ { false, std::make_shared<button>("Color", [nb, nb_indicator](){ nb->set_page(0); nb_indicator->set_text("Active Notebook Widget: Color"); })}
                            , { false, std::make_shared<button>("Swipe", [nb, nb_indicator](){ nb->set_page(1); nb_indicator->set_text("Active Notebook Widget: Swipe"); })}
                            }, 20, true);

    auto lv = std::make_shared<list_view>(0, 2, 4, test_values, [](){ std::cout << "press list_view" << std::endl; });

    padding main_widget(20, hbox(
        { { true, vbox({ { true, lv }, { false, nb_indicator }, { true, nb }, { false, nb_controls } }, 20, false) }
        , { true, cw() }
        , { false, vbox( { { false, num_button() }
                         , { false, cw() }
                         , { false, std::make_shared<label>("This text should hopefully produce a linebreak. Otherwise something is not working correctly.\n\nYou may use Tab and Shift+Tab to focus widgets or use Shift and the corresponding arrow key for a 2-dimensional direction.") }
                         , { true, std::make_shared<table, vec, std::vector<table::entry>>({ 4, 4 },
                                 { { { 0, 0, 2, 2 }, num_button() }
                                 , { { 0, 2, 1, 1 }, num_button() }
                                 , { { 1, 2, 1, 1 }, num_button() }
                                 , { { 2, 0, 1, 3 }, num_button() }
                                 , { { 0, 3, 3, 1 }, num_button() }
                                 , { { 3, 0, 1, 1 }, num_button() }
                                 , { { 3, 1, 1, 3 }, num_button() }
                                 }) }

                         , { false, std::make_shared<label>(std::vector<paragraph>{paragraph("Text 1, Paragraph 1."), paragraph("Text 1, Paragraph 2.")}) }
                         , { true, cw() }
                         , { true, std::make_shared<label>(std::vector<paragraph>{paragraph("Text 2, Paragraph 1.")}) }
                         }, 20, false) }
        , { false, vbox( { { false, num_button() }
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

