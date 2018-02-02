#include "draw_context.hpp"

draw_context::draw_context(SDL_Window * w, font_atlas & fa)
    : _fa(fa)
{
    // sometimes a renderer is already associated with the window
    _renderer = SDL_GetRenderer(w);

    if (_renderer == nullptr)
        _renderer = SDL_CreateRenderer(w, -1, 0);
    if (_renderer == nullptr)
        throw std::runtime_error("Failed to create renderer: " + std::string(SDL_GetError()));

    auto ret = SDL_GetRendererOutputSize(_renderer, &_width, &_height);
    if (ret != 0)
    {
        throw std::runtime_error("Failed to get renderer output size: " + std::string(SDL_GetError()));
    }
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
    SDL_RenderCopyEx(_renderer, tex, srcrect, dstrect, 0, nullptr, SDL_FLIP_NONE);
    SDL_DestroyTexture(tex);
}

void draw_context::draw_button_box(SDL_Rect box, bool activated, bool selected)
{
    set_color(activated ? _theme.button_pressed_bg_color : (selected ? _theme.button_selected_bg_color : _theme.button_bg_color));
    SDL_RenderFillRect(_renderer, &box);
    set_color(_theme.button_frame_color);
    SDL_RenderDrawRect(_renderer, &box);
}

void draw_context::draw_button_text(SDL_Rect box, std::string const & text)
{
    auto text_surf_ptr = _fa.text(text);
    SDL_SetSurfaceColorMod(text_surf_ptr.get(), _theme.button_fg_color.r, _theme.button_fg_color.g, _theme.button_fg_color.b);
    // center text in box
    SDL_Rect target_rect = {box.x + (box.w - text_surf_ptr->w) / 2, box.y + (box.h - text_surf_ptr->h) / 2, text_surf_ptr->w, text_surf_ptr->h};

    blit(text_surf_ptr.get(), nullptr, &target_rect);
}

void draw_context::draw_entry_box(SDL_Rect box)
{
    set_color(_theme.entry_bg_color);
    SDL_RenderFillRect(_renderer, &box);
    set_color(_theme.entry_frame_color);
    SDL_RenderDrawRect(_renderer, &box);
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

int draw_context::width()
{
    return _width;
}

int draw_context::height()
{
    return _height;
}

