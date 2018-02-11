#include "draw_context.hpp"

#include "sdl_util.hpp"

draw_context::draw_context(SDL_Renderer * renderer, font_word_cache & fwc)
    : _renderer(renderer)
    , _fwc(fwc)
{
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

void set_texture_color_mod(SDL_Texture * t, SDL_Color c)
{
    Uint8 r, g, b;
    if (SDL_GetTextureColorMod(t, &r, &g, &b) < 0)
        throw std::runtime_error(SDL_GetError());

    if (r != c.r || g != c.g || b != c.b)
        SDL_SetTextureColorMod(t, c.r, c.g, c.b);
}

void draw_context::run_copy_commands(std::vector<copy_command> const & commands, SDL_Rect box, SDL_Color color)
{
    for (auto const & c : commands)
    {
        set_texture_color_mod(c.texture, color);
        vec size = texture_dim(c.texture);
        SDL_Rect target { box.x + c.x_offset, box.y + c.y_offset, size.w, size.h };
        SDL_RenderCopy(_renderer, c.texture, nullptr, &target);
    }
}

void draw_context::draw_button_text(std::string const & text, SDL_Rect abs_rect)
{
    auto result = _fwc.text(text);
    vec const & size = std::get<0>(result);

    // center text in abs_rect
    SDL_Rect target_rect { abs_rect.x + (abs_rect.w - size.w) / 2, abs_rect.y + (abs_rect.h - size.h) / 2, size.w, size.h };

    //blit(text_surf_ptr.get(), nullptr, &target_rect);
    run_copy_commands(std::get<1>(result), target_rect, _theme.button_fg_color);
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
    // TODO rework
    int const clipped_width = abs_rect.w - texture_x_offset;
    int const clipped_height = abs_rect.h - texture_y_offset;

    if (clipped_width >= 0 && clipped_height >= 0)
    {
        auto result = _fwc.text(text);
        vec const & size = std::get<0>(result);

        int const avail_width = size.w - texture_x_offset;
        int const avail_height = size.h - texture_y_offset;

        // don't overstep texture boundaries
        SDL_Rect source_rect { texture_x_offset, texture_y_offset, std::min(avail_width, clipped_width), std::min(avail_height, clipped_height) };

        // avoid automatic stretch of blitting
        SDL_Rect target_rect { abs_rect.x, abs_rect.y, source_rect.w, source_rect.h };

        // TODO entry text color
        //SDL_SetSurfaceColorMod(text_surf_ptr.get(), 0, 0, 0);
        //blit(text_surf_ptr.get(), &source_rect, &target_rect);
        run_copy_commands(std::get<1>(result), target_rect, { 0, 0, 0 });
    }
}

int draw_context::draw_label_text(SDL_Rect box, std::string text)
{
    auto result = _fwc.text(text, box.w);
    vec const & size = std::get<0>(result);

    int width = std::min(box.w, size.w);
    int height = std::min(box.h, size.h);
    //SDL_Rect source { 0, 0, width, height };
    SDL_Rect target { box.x, box.y, width, height };

    // TODO fade effect when clipped?

    set_color({0, 0, 0});
    draw_rect_filled(box);

    //blit(text_surf_ptr.get(), &source, &target);
    run_copy_commands(std::get<1>(result), target, { 255, 255, 255});
    return size.h;
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
