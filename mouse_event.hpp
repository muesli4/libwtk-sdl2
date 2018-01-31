#ifndef MOUSE_EVENT_HPP
#define MOUSE_EVENT_HPP

#include <SDL2/SDL.h>

struct mouse_event
{
    enum class button_type { UP, DOWN };

    button_type type;
    int32_t x;
    int32_t y;
};

// TODO move then we can remove SDL2 include
mouse_event mouse_event_from_sdl(SDL_MouseButtonEvent const & mbe);

#endif

