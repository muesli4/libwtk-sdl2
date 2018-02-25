// TODO think about a way where the widget might not arbitrarily draw to the
//      screen (basically bounding box drawn in offset mode)
// TODO consistent background drawing, best solution:
//      draw background only when absolutely needed (let top-level widget draw it)
// TODO improve dirty drawing:
//          mark_dirty() should only mark the widget (and not propagate upwards)
// TODO handle double buffer with dirty drawing:
//          use int instead of bool and draw to every buffer

// TODO add singleton style manager that provides global defaults for spacing and theme
//      (only used for default parameters)
//      this might then be overriden for spacing in containers and style in widget_contexts
//      (maybe even split the singletons)
// TODO implement mouse move event for several widgets
// TODO widget: add hit test for widgets, then there is no need to propagate the mouse event
// TODO should mouse click select selectable widgets? if so, the mouse_down
//      event will have to go to a unqiue widget. This also makes sense when
//      considering the fact that widgets should only respond to mouse down
//      events within their box.

// TODO instead of dirty checking either send request to relayout or redraw an
//      area.
//      - Might use clipping and normal drawing or specialized method.
//      - Use an interface beside the parrent pointer to request these kind of
//        changes. Then it can be propagated upwards and at the top anything can
//        be put in that is not even a widget (e.g., helper in widget_context or
//        a window).
//
// struct updateable
// {
//     virtual void on_size_changed() = 0;
//
//     struct redraw_request
//     {
//         widget * w;
//         SDL_Rect area;
//     };
//
//     virtual void on_child_redraw(std::vector<redraw_request> redraw_requests) = 0;
// };

// TODO provide means to add widgets outside of constructors (redo layout only for local container or upwards?)
// TODO should widgets relayout when their content changes?

// TODO which widgets need width-for-height?

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

#include "widget.hpp"
#include "button.hpp"
#include "padding.hpp"
#include "color_widget.hpp"
#include "box.hpp"
#include "swipe_area.hpp"
#include "list_view.hpp"
#include "notebook.hpp"
#include "label.hpp"
#include "empty.hpp"
#include "table.hpp"
#include "texture_view.hpp"
#include "widget_context.hpp"
#include "sdl_util.hpp"

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

void event_loop(SDL_Renderer * renderer)
{
    std::vector<std::string> test_values{"a", "b", "c", "d", "testwdfkosadjflkajskdfjlaskdjflkasdjdfklajsdlkfjasldkdfjflkasddjflkdsjlfkjdsalkkfjdkk", "test1", "test2", "a very long string this is indeed", "foo", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a"};

    auto nb = std::make_shared<notebook>(std::vector<widget_ptr>
        { cw()
        , std::make_shared<swipe_area>([](swipe_direction dir){ std::cout << "swipe " << to_string(dir) << std::endl; }, [](){ std::cout << "press" << std::endl; })
        });

    auto nb_indicator = std::make_shared<label>("Active Notebook Widget: Color");
    auto nb_controls = hbox({ { false, std::make_shared<button>("Color", [nb, nb_indicator](){ nb->set_page(0); nb_indicator->set_text("Active Notebook Widget: Color"); })}
                            , { false, std::make_shared<button>("Swipe", [nb, nb_indicator](){ nb->set_page(1); nb_indicator->set_text("Active Notebook Widget: Swipe"); })}
                            }, 20, true);

    std::shared_ptr<list_view> lv = std::make_shared<list_view>(test_values, 0, [&lv](std::size_t p){ std::cout << "press list_view at " << p << std::endl; lv->set_highlight_position(p); });

    padding main_widget(20, hbox(
        { { true, vbox({ { true, lv }, { false, nb_indicator }, { true, nb }, { false, nb_controls } }, 20, false) }
        , { false, vbox({ { true, cw() }, { false, std::make_shared<texture_view>(load_texture_from_image(renderer, PKGDATA"/test.jpeg")) }, { true, cw() } }, 20, false) }
        , { true, vbox( { { false, num_button() }
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
                                 }, 20) }

                         , { false, std::make_shared<label>(std::vector<paragraph>{paragraph("Text 1, Paragraph 1."), paragraph("Text 1, Paragraph 2.")}) }
                         , { true, cw() }
                         , { true, std::make_shared<label>(std::vector<paragraph>{paragraph("Text 2, Paragraph 1.")}) }
                         }, 20, false) }
        , { false, vbox( { { false, num_button() }
                         , { true, std::make_shared<empty>() }
                         , { false, std::make_shared<label>(std::vector<paragraph>{ paragraph("This is a bigger font.", 0, 1) }) }
                         , { true, std::make_shared<empty>() }
                         , { false, std::make_shared<button>("Quit", [](){ SDL_Event ev { .type = SDL_QUIT }; SDL_PushEvent(&ev); })}
                         }
                       , 20, false) }
        }, 20, false));

    /*
    padding main_widget(20, vbox({ { false, std::make_shared<label>("This text should hopefully produce a linebreak. Otherwise something is not working correctly.\n\nYou may use Tab and Shift+Tab to focus widgets or use Shift and the corresponding arrow key for a 2-dimensional direction.")}, { false, cw()} , { false, num_button() }, { false, num_button() } }, 20, false));
    */

    // setup necessary context (as in local to a window or other unit of management)
    widget_context ctx
        ( renderer
        , { { "/usr/share/fonts/TTF/DejaVuSans.ttf", 15 }
          , { "/usr/share/fonts/TTF/DejaVuSans.ttf", 20 }
          }
        , main_widget
        );

    // draw initial state
    ctx.draw();

    SDL_Event ev;
    while (SDL_WaitEvent(&ev))
    {
        if (ev.type == SDL_QUIT)
            break;
        else if (ev.type == SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_CTRL) && ev.key.keysym.sym == SDLK_q)
            break;
        else if (ev.type == SDL_WINDOWEVENT)
        {
            if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                //std::cout << ev.window.data1 << "x" << ev.window.data2 << std::endl;

                ctx.change_widget_area({ 0, 0, ev.window.data1, ev.window.data2 });
                // TODO 
                ctx.draw();
            }
            else if (ev.window.event != SDL_WINDOWEVENT_EXPOSED)
            {
                continue;
            }

        }
        else
        {
            ctx.process_event(ev);
            ctx.draw_dirty();
        }

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

    SDL_Window * window = SDL_CreateWindow("widget test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 1000/*775*/, 0);

    if (window == nullptr)
    {
        std::cerr << "Could not open window:"
                  << SDL_GetError() << '.' << std::endl;
    }
    else
    {
        SDL_SetWindowResizable(window, SDL_TRUE);
        event_loop(renderer_from_window(window));
    }

    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}

