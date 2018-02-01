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
        for (auto cptr : _children)
            cptr->set_parent(this);

        mark_dirty();
    }

    void on_draw(draw_context & dc, selection_context const & sc) const override
    {
        for (auto cptr : _children)
            cptr->on_draw(dc, sc);
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
    }

    widget * find_selectable() override
    {
        for (auto cptr : _children)
        {
            auto w = cptr->find_selectable();
            if (w != nullptr)
            {
                return w;
            }
        }
        return nullptr;
    }

    /*
    widget * navigate_selectable(navigation_type nt) const override
    {
        if (nt == navigation_type::NEXT || nt == navigation_type::NEXT_X)
        {

        }
        else if (nt == navigation_type::NEXT || nt == navigation_type::NEXT_X)
        {
        }
    }
    */

    widget * navigate_selectable_from_children(navigation_type nt, widget * w, int center_x, int center_y) override
    {
        if (nt == navigation_type::NEXT_Y || nt == navigation_type::PREV_Y)
        {
            return navigate_upwards(nt, center_x, center_y);
        }
        else
        {
            auto begin = _children.begin();
            auto end = _children.end();
            auto it = std::find_if(begin, end, [w](widget_ptr wptr){ return wptr.get() == w; });

            if (nt == navigation_type::NEXT || nt == navigation_type::NEXT_X)
            {
                it++;
            }
            else if (nt == navigation_type::PREV || nt == navigation_type::PREV_X)
            {
                it--;
            }


            if (it == end || it < begin)
            {
                return navigate_upwards(nt, _box.x + _box.w / 2, _box.y + _box.h / 2);
            }
            else
            {
                return (*it)->find_selectable();
            }
        }
    }

    ~container() override
    {
    }

    protected:

    widget * navigate_upwards(navigation_type nt, int center_x, int center_y)
    {
        if (_parent == nullptr)
            return nullptr;
        else
            return _parent->navigate_selectable_from_children(nt, this, center_x, center_y);
    }


    std::vector<widget_ptr> _children;
};

void event_loop(SDL_Window * window)
{
    //color_widget cw;
    container main_widget
        {// std::make_shared<color_widget>()
        //, pad(10, std::make_shared<color_widget>())
         pad(20, 80, std::make_shared<color_widget>())
        , pad(10, std::make_shared<button>("Button 1", [](){ std::cout << "click1" << std::endl;}))
        //, std::make_shared<container, std::initializer_list<widget_ptr>>(
        //        { pad(10, std::make_shared<button>("Button 2", [](){ std::cout << "click2" << std::endl;}))
        //        , pad(10, std::make_shared<button>("Button 3", [](){ std::cout << "click3" << std::endl;}))
        //        , pad(10, std::make_shared<button>("Button 4", [](){ std::cout << "click4" << std::endl;}))
        //        })
        };

    // setup necessary contexts (as in local to a window or other unit of management)
    selection_context sc;
    font_atlas fa("/usr/share/fonts/TTF/DejaVuSans.ttf", 15);
    draw_context dc(window, fa);

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
        else if (ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP)
        {
            main_widget.on_mouse_event(mouse_event_from_sdl(ev.button));
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
                    //sc.navigate_selection(navigation_type::PREV, &main_widget);
                }
                else
                {
                    sc.navigate_selection(navigation_type::NEXT, &main_widget);
                }
            }
            else if (keysym.sym == SDLK_RETURN)
                sc.dispatch_activation();
            else
                sc.dispatch_key_event(key_event());
        }

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

