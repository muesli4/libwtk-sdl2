#include "mouse_event.hpp"

mouse_event mouse_event_from_sdl(SDL_MouseButtonEvent const & mbe)
{
    return { mbe.type == SDL_MOUSEBUTTONDOWN ? mouse_event::button_type::DOWN : mouse_event::button_type::UP, mbe.x, mbe.y };
}

