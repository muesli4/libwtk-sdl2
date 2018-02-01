#ifndef DRAW_CONTEXT_HPP
#define DRAW_CONTEXT_HPP

#include <exception>
#include <SDL2/SDL.h>

#include "font_atlas.hpp"

// TODO replace rendering with a virtual interface that might as well blit images
struct color_theme
{
    color_theme()
        : button_bg_color{25, 25, 25}
        , button_fg_color{235, 235, 235}
        , button_frame_color{105, 105, 105}
        , button_selected_bg_color{105, 55, 55}
        , entry_bg_color{255, 255, 255}
        , entry_frame_color{100, 100, 100}
        , entry_selected_bg_color{250, 200, 200}
        , bg_color{0, 0, 0}
        , active_color{230, 230, 255}
    {}

    SDL_Color button_bg_color;
    SDL_Color button_fg_color;
    SDL_Color button_frame_color;
    SDL_Color button_selected_bg_color;
    SDL_Color entry_bg_color;
    SDL_Color entry_frame_color;
    SDL_Color entry_selected_bg_color;
    SDL_Color bg_color;

    // displays the status of something, but not related to direct user interaction
    SDL_Color active_color;
};

struct draw_context
{
    draw_context(SDL_Window * w, font_atlas & fa);

    void present();

    void draw_rect_filled(SDL_Rect r);

    void draw_rect(SDL_Rect r);

    void blit(SDL_Surface * s, const SDL_Rect * srcrect, const SDL_Rect * dstrect);

    void draw_button_box(SDL_Rect box, bool activated, bool selected);

    void draw_button_text(SDL_Rect box, std::string const & text);

    void draw_entry_box(SDL_Rect box);

    void draw_background(SDL_Rect box);

    void draw_entry_pressed_background(SDL_Rect box);

    void draw_entry_active_background(SDL_Rect box);

    void draw_entry_position_indicator(SDL_Rect box);

    void set_color(SDL_Color c);
    void set_color_alpha(SDL_Color c);


    int width();
    int height();

    private:

    SDL_Renderer * _renderer;

    font_atlas & _fa;

    int _width;
    int _height;

    color_theme _theme;
};

#endif

