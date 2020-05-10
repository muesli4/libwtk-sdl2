#include "widget_context.hpp"
#include "widget.hpp"
#include "sdl_util.hpp"

widget_context::widget_context(SDL_Renderer * renderer, std::vector<font> fonts, widget & main_widget)
    : widget_context(renderer, fonts, main_widget, box_from_renderer(renderer))
{
}

widget_context::widget_context(SDL_Renderer * renderer, std::vector<font> fonts, widget & main_widget, rect box)
    : _box(box)
    , _renderer(renderer)
    , _fm(_renderer, fonts)
    , _dc(_renderer, _fm)
    , _sc(box)
    , _mt()
    , _main_widget(main_widget)
    , _context_info( _fm
                   , { .lower_threshold = _fm.font_line_skip() * 2
                     , .dir_unambig_factor = 0.3
                     }
                   )
{
    std::vector<widget *> stack { &main_widget };
    do
    {
        widget * wptr = stack.back();
        stack.pop_back();
        auto cs = wptr->get_children();
        for (auto c : cs)
            stack.push_back(c);
        wptr->set_context_info(_context_info);
    }
    while (!stack.empty());

    main_widget.apply_layout(box);
}

point tfinger_to_point(SDL_TouchFingerEvent const & tfinger, rect const & box)
{
    return { static_cast<int>(tfinger.x * box.w), static_cast<int>(tfinger.y * box.h) };
}

bool widget_context::process_mouse_event(SDL_Event const & ev)
{
    // Note that unfortunately SDL produces duplicate events for touch devices.
    // For this reason those events are caught here.
    if (ev.type == SDL_MOUSEBUTTONDOWN)
    {
        if (ev.button.which != SDL_TOUCH_MOUSEID)
        {
            point p { ev.button.x, ev.button.y };
            _main_widget.on_mouse_down_event({p});
            _mt.mouse_down(p);
        }
    }
    else if (ev.type == SDL_MOUSEBUTTONUP)
    {
        if (ev.button.which != SDL_TOUCH_MOUSEID)
        {
            _main_widget.on_mouse_up_event(_mt.mouse_up({ ev.button.x, ev.button.y }));
        }
    }
    else if (ev.type == SDL_MOUSEMOTION)
    {
        if (ev.motion.which != SDL_TOUCH_MOUSEID)
        {
            _main_widget.on_mouse_move_event(_mt.mouse_move({ ev.motion.x, ev.motion.y }));
        }
    }
    else if (ev.type == SDL_FINGERDOWN)
    {
        point p = tfinger_to_point(ev.tfinger, _box);
        _main_widget.on_mouse_down_event({p});
        _mt.mouse_down(p);
    }
    else if (ev.type == SDL_FINGERUP)
    {
        _main_widget.on_mouse_up_event(_mt.mouse_up(tfinger_to_point(ev.tfinger, _box)));
    }
    else if (ev.type == SDL_FINGERMOTION)
    {
        _main_widget.on_mouse_move_event(_mt.mouse_move(tfinger_to_point(ev.tfinger, _box)));
    }
    else
    {
        return false;
    }

    return true;
}

bool widget_context::process_key_event(SDL_Event const & ev)
{
    // TODO hardcoded keys are probably not the best idea for reusability
    if (ev.type == SDL_KEYDOWN)
    {
        auto const & keysym = ev.key.keysym;

        if (keysym.sym == SDLK_TAB)
        {
            if (keysym.mod & KMOD_SHIFT)
            {
                navigate_selection(navigation_type::PREV);
            }
            else
            {
                navigate_selection(navigation_type::NEXT);
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
                default: return false;
            }
            navigate_selection(nt);
        }
        // TODO waiting for key events being implemented
        //_sc.dispatch_key_event(key_event());
        else if (keysym.sym == SDLK_RETURN)
            activate();
        else
            return false;
        return true;
    }
    return false;
}

bool widget_context::process_event(SDL_Event const & ev)
{
    return process_mouse_event(ev) || process_key_event(ev);
}

void widget_context::draw(bool present)
{
    _main_widget.draw(_dc, _sc);
    if (present)
        _dc.present();
}

void widget_context::draw_dirty(int dirty_redraws)
{
    _main_widget.draw_dirty(_dc, _sc);

    while (dirty_redraws > 0)
    {
        _dc.present();
        _main_widget.draw_dirty(_dc, _sc);
        dirty_redraws--;
    }

    _main_widget.clear_dirty();
}

void widget_context::select_widget(widget & w)
{
    _sc.select_widget(&w);
}

void widget_context::unselect_widget()
{
    _sc.unselect_widget();
}

bool widget_context::is_selected_widget(widget const & w)
{
    return _sc.is_selected_widget(&w);
}

void widget_context::change_widget_area(rect new_box)
{
    _box = new_box;
    _main_widget.apply_layout(_box);
    _main_widget.mark_dirty();
    _sc.change_widget_area(new_box);
}

void widget_context::activate()
{
    _sc.dispatch_activation();
}

void widget_context::navigate_selection(navigation_type nt)
{
    _sc.navigate_selection(nt, &_main_widget);
}


void widget_context::set_context_info(widget * w)
{
    w->set_context_info(_context_info);
    for (auto ptr : w->get_children())
    {
        set_context_info(ptr);
    }
}
