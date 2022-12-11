// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_DRAW_CONTEXT_HPP
#define LIBWTK_SDL2_DRAW_CONTEXT_HPP

#include <exception>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_render.h>

#include "copy_command.hpp"
#include "font_manager.hpp"
#include "geometry.hpp"

// TODO replace rendering with a virtual interface that might as well blit images
struct color_theme
{
    color_theme();

    SDL_Color button_bg_color;
    SDL_Color button_fg_color;
    SDL_Color button_frame_color;
    SDL_Color button_pressed_bg_color;
    SDL_Color button_selected_bg_color;

    SDL_Color entry_selected_bg_color;
    SDL_Color entry_highlight_bg_color;

    SDL_Color entry_box_bg_color;
    SDL_Color entry_box_frame_color;
    SDL_Color entry_box_selected_frame_color;

    SDL_Color bg_color;

    // displays the status of something, but not related to direct user interaction
    SDL_Color active_color;

    SDL_Color hightlight_color;
};

/*
struct theme
{
    enum class box_type
    {
        BUTTON,
        LIST,
        TEXT_ENTRY
    };

    enum class state
    {
        IDLE = 0
        PRESSED = 1,
        SELECTED = 2,
    };

    static state operator|(state l, state r)
    {
        return static_cast<state>(static_cast<int>(l) | static_cast<int>(r));
    }

    static bool operator&(state l, state r)
    {
        return static_cast<int>(l) & static_cast<int>(r);
    }

    virtual int box_border_width(box_type bt, state s) = 0;
    virtual void draw_box(box_type bt, state s, rect box) = 0;
};

struct default_theme : theme
{
    int box_border_width(box_type bt, state s) override
    {
    }
    void draw_box(box_type bt, state s, rect box) override
    {
    }

    struct box_colors
    {
        SDL_Color bg;
        SDL_Color fg;
        SDL_Color frame;
    };

    default_theme()
        : button_bg_color{25, 25, 25}
        , button_fg_color{235, 235, 235}
        , button_frame_color{105, 105, 105}
        , button_pressed_bg_color{105, 55, 55}
        , button_selected_bg_color{55, 55, 105}
        , entry_bg_color{255, 255, 255}
        , entry_frame_color{100, 100, 100}
        , entry_selected_bg_color{250, 200, 200}
        , bg_color{0, 0, 0}
        , active_color{230, 230, 255}
    {
        box_colors base { {25, 25, 25}, {235, 235, 235}, {105, 105, 105} };

        // 0 - theme::box_type::BUTTON
        _colors[(int)box_type::BUTTON][(int)state::IDLE] = base;
        base.bg = {105, 55, 55};
        _colors[(int)box_type::BUTTON][(int)state::PRESSED] = base;
        base.frame = {55, 55, 105} // TODO brighter
        _colors[(int)box_type::BUTTON][(int)state::PRESSED | state::SELECTED] = base;
        base.bg = {25, 25, 25};
        _colors[(int)box_type::BUTTON][(int)state::SELECTED] = base;

        // 1 - theme::box_type::LIST
        base = {{255, 255, 255}, {0, 0, 0}, {100, 100, 100}};
        _colors[(int)box_type::LIST][(int)state::IDLE] = base;
        _colors[(int)box_type::LIST][(int)state::PRESSED] = base;
        base.frame = {55, 55, 105};
        _colors[(int)box_type::LIST][(int)state::PRESSED | state::SELECTED] = base;
        _colors[(int)box_type::LIST][(int)state::SELECTED] = base;

        // 2 - theme::box_type::TEXT_ENTRY
    }

    private:

    // for each state includes colors for: background, foreground (e.g., font), frame
    box_colors[2][4] _colors;
};
*/

struct draw_context
{
    // draw to a window exclusively
    draw_context(SDL_Renderer * renderer, font_manager & fm);

    void present();

    // button (heightened box)
    void draw_button_box(rect box, bool activated, bool selected);
    void draw_button_text(std::string const & text, rect abs_rect);

    // entry (lowered box)
    void draw_entry_box(rect box, bool selected);
    void draw_entry_text(std::string text, rect abs_rect, int texture_x_offset = 0, int texture_y_offset = 0);
    void draw_entry_pressed_background(rect box);
    void draw_entry_active_background(rect box);
    void draw_entry_hightlighted_background(rect box);
    void draw_entry_position_indicator(rect box);

    void draw_radio_entry(rect box, bool active, bool selected);

    // Draws a string in the box and returns the actually used height within the
    // box.
    int draw_label_text(rect box, std::string text, bool wrap, int font_idx = 0);

    void draw_background(rect box);

    // low-level drawing TODO move or refactor
    void set_color(SDL_Color c);
    void set_color_alpha(SDL_Color c);
    void draw_rect_filled(rect r);
    void draw_rect(rect r);
    void blit(SDL_Surface * s, const rect * srcrect, const rect * dstrect);
    void run_copy_commands(std::vector<copy_command> const & commands, point origin, SDL_Color color);
    void copy_texture(SDL_Texture * t, rect src, rect dst);
    void copy_texture(SDL_Texture * t, rect dst);

    private:

    SDL_Renderer * _renderer;

    font_manager & _fm;

    //rect _clip_box;

    color_theme _theme;
};

#endif

