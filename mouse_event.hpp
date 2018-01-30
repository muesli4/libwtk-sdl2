#ifndef MOUSE_EVENT_HPP
#define MOUSE_EVENT_HPP

struct mouse_event
{
    enum class button_type { UP, DOWN };

    button_type type;
    int32_t x;
    int32_t y;
};


#endif

