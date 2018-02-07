#include "widget_context.hpp"
#include "widget.hpp"

SDL_Rect box_from_window(SDL_Window * w)
{
    SDL_Rect result { .x = 0, .y = 0 };
    SDL_GetWindowSize(w, &result.w, &result.h);
    return result;
}

widget_context::widget_context(SDL_Window * window, font f, widget & main_widget)
    : widget_context(window, box_from_window(window), f, main_widget)
{
}

widget_context::widget_context(SDL_Window * window, SDL_Rect box, font f, widget & main_widget)
    : _fa(f.path, f.size)
    , _dc(window, _fa)
    , _sc()
    , _sd(_fa.font_line_skip(), 0.3)
    , _main_widget(main_widget)
    , _layout_info(_fa)
{
    std::vector<widget *> stack { &main_widget };
    do
    {
        widget * wptr = stack.back();
        stack.pop_back();
        auto cs = wptr->get_children();
        for (auto c : cs)
            stack.push_back(c);
        wptr->set_layout_info(_layout_info);
    }
    while (!stack.empty());

    main_widget.apply_layout(box);
}

void widget_context::process_event(SDL_Event & ev)
{
    if (ev.type == SDL_MOUSEBUTTONDOWN)
    {
        point p { ev.button.x, ev.button.y };
        _main_widget.on_mouse_down_event({p});
        _sd.mouse_down(p);
    }
    else if (ev.type == SDL_MOUSEBUTTONUP)
    {
        _main_widget.on_mouse_up_event(_sd.mouse_up({ ev.button.x, ev.button.y }));
    }
    else if (ev.type == SDL_KEYDOWN)
    {
        auto const & keysym = ev.key.keysym;

        if ((keysym.mod & KMOD_CTRL) && keysym.sym == SDLK_q)
            return; // TODO does not belong here
        else if (keysym.sym == SDLK_TAB)
        {
            if (keysym.mod & KMOD_SHIFT)
            {
                _sc.navigate_selection(navigation_type::PREV, &_main_widget);
            }
            else
            {
                _sc.navigate_selection(navigation_type::NEXT, &_main_widget);
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
                default: return;
            }
            _sc.navigate_selection(nt, &_main_widget);
        }
        else if (keysym.sym == SDLK_RETURN)
            _sc.dispatch_activation();
        else
            _sc.dispatch_key_event(key_event());
    }
}

void widget_context::draw(bool only_when_dirty, bool present)
{
    if (only_when_dirty)
        _main_widget.draw_when_dirty(_dc, _sc);
    else
        _main_widget.on_draw(_dc, _sc);

    if (present)
        _dc.present();
}

void widget_context::set_layout_info(widget * w)
{
    w->set_layout_info(_layout_info);
    for (auto ptr : w->get_children())
    {
        set_layout_info(ptr);
    }
}
