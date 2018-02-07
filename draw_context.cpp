#include "draw_context.hpp"

#include "sdl_util.hpp"

layout_info::layout_info(font_atlas & fa)
    : _fa(fa)
{
}

vec layout_info::text_size(std::string t, int max_line_width) const
{
    return _fa.get().text_size(t, max_line_width);
}

int layout_info::text_minimum_width(std::string t) const
{
    return _fa.get().text_minimum_width(t);
}

unsigned int layout_info::font_height() const
{
    return _fa.get().font_height();
}

int layout_info::font_line_skip() const
{
    return _fa.get().font_line_skip();
}

draw_context::draw_context(SDL_Window * w, font_atlas & fa)
    : _fa(fa)
{
    // sometimes a renderer is already associated with the window
    _renderer = SDL_GetRenderer(w);

    if (_renderer == nullptr)
        _renderer = SDL_CreateRenderer(w, -1, 0);
    if (_renderer == nullptr)
        throw std::runtime_error("Failed to create renderer: " + std::string(SDL_GetError()));
}

void draw_context::present()
{
    SDL_RenderPresent(_renderer);
}

void draw_context::draw_rect_filled(SDL_Rect r)
{
    SDL_RenderFillRect(_renderer, &r);
}

void draw_context::draw_rect(SDL_Rect r)
{
    SDL_RenderDrawRect(_renderer, &r);
}

void draw_context::blit(SDL_Surface * s, const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{
    // TODO inefficient
    SDL_Texture * tex = SDL_CreateTextureFromSurface(_renderer, s);
    //SDL_RenderCopyEx(_renderer, tex, srcrect, dstrect, 0, nullptr, SDL_FLIP_NONE);
    SDL_RenderCopy(_renderer, tex, srcrect, dstrect);
    SDL_DestroyTexture(tex);
}

void draw_context::draw_button_box(SDL_Rect box, bool activated, bool selected)
{
    set_color(activated ? _theme.button_pressed_bg_color : (selected ? _theme.button_selected_bg_color : _theme.button_bg_color));
    SDL_RenderFillRect(_renderer, &box);
    set_color(_theme.button_frame_color);
    SDL_RenderDrawRect(_renderer, &box);
}

void draw_context::draw_button_text(std::string const & text, SDL_Rect abs_rect)
{
    auto text_surf_ptr = _fa.text(text);
    SDL_SetSurfaceColorMod(text_surf_ptr.get(), _theme.button_fg_color.r, _theme.button_fg_color.g, _theme.button_fg_color.b);

    // center text in abs_rect
    SDL_Rect target_rect = {abs_rect.x + (abs_rect.w - text_surf_ptr->w) / 2, abs_rect.y + (abs_rect.h - text_surf_ptr->h) / 2, text_surf_ptr->w, text_surf_ptr->h};

    blit(text_surf_ptr.get(), nullptr, &target_rect);
}

void draw_context::draw_entry_box(SDL_Rect box)
{
    set_color(_theme.entry_bg_color);
    SDL_RenderFillRect(_renderer, &box);
    set_color(_theme.entry_frame_color);
    SDL_RenderDrawRect(_renderer, &box);
}

void draw_context::draw_entry_text(std::string text, SDL_Rect abs_rect, int texture_x_offset, int texture_y_offset)
{
    int const clipped_width = abs_rect.w - texture_x_offset;
    int const clipped_height = abs_rect.h - texture_y_offset;

    if (clipped_width >= 0 && clipped_height >= 0)
    {
        auto text_surf_ptr = _fa.text(text);

        int const avail_width = text_surf_ptr->w - texture_x_offset;
        int const avail_height = text_surf_ptr->h - texture_y_offset;

        // don't overstep texture boundaries
        SDL_Rect source_rect { texture_x_offset, texture_y_offset, std::min(avail_width, clipped_width), std::min(avail_height, clipped_height) };

        // avoid automatic stretch of blitting
        SDL_Rect target_rect { abs_rect.x, abs_rect.y, source_rect.w, source_rect.h };

        // TODO entry text color
        SDL_SetSurfaceColorMod(text_surf_ptr.get(), 0, 0, 0);
        blit(text_surf_ptr.get(), &source_rect, &target_rect);
    }
}

void draw_context::draw_background(SDL_Rect box)
{
    set_color(_theme.bg_color);
    SDL_RenderFillRect(_renderer, &box);
}

void draw_context::draw_entry_pressed_background(SDL_Rect box)
{
    set_color(_theme.entry_selected_bg_color);
    SDL_RenderFillRect(_renderer, &box);
}

void draw_context::draw_entry_active_background(SDL_Rect box)
{
    set_color(_theme.active_color);
    SDL_RenderFillRect(_renderer, &box);
}

void draw_context::draw_entry_position_indicator(SDL_Rect box)
{
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(_renderer, 150, 250, 150, 200);

    SDL_RenderFillRect(_renderer, &box);
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_NONE);
}

void draw_context::set_color(SDL_Color c)
{
    SDL_SetRenderDrawColor(_renderer, c.r, c.g, c.b, 0);
}

void draw_context::set_color_alpha(SDL_Color c)
{
    SDL_SetRenderDrawColor(_renderer, c.r, c.g, c.b, c.a);
}
