#include "draw_context.hpp"

#include "sdl_util.hpp"

color_theme::color_theme()
    : button_bg_color{25, 25, 25}
    , button_fg_color{235, 235, 235}
    , button_frame_color{105, 105, 105}
    , button_pressed_bg_color{105, 55, 55}
    , button_selected_bg_color{55, 55, 105}

    , entry_selected_bg_color{250, 200, 200}
    , entry_highlight_bg_color{240, 240, 240}

    , entry_box_bg_color{255, 255, 255}
    , entry_box_frame_color{100, 100, 100}
    , entry_box_selected_frame_color{155, 155, 205}

    , bg_color{0, 0, 0}
    , active_color{230, 230, 255}
    , hightlight_color{210, 210, 210}
{}

draw_context::draw_context(SDL_Renderer * renderer, font_manager & fm)
    : _renderer(renderer)
    , _fm(fm)
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

void draw_context::run_copy_commands(std::vector<copy_command> const & commands, point origin, SDL_Color color)
{
    for (auto const & c : commands)
    {
        set_texture_color_mod(c.texture, color);
        vec size = texture_dim(c.texture);
        SDL_Rect target { origin.x + c.x_offset, origin.y + c.y_offset, size.w, size.h };
        SDL_RenderCopy(_renderer, c.texture, &c.source, &target);
    }
}

void draw_context::copy_texture(SDL_Texture * t, SDL_Rect src, SDL_Rect dst)
{
    SDL_RenderCopy(_renderer, t, &src, &dst);
}

void draw_context::copy_texture(SDL_Texture * t, SDL_Rect dst)
{
    SDL_RenderCopy(_renderer, t, nullptr, &dst);
}

void draw_context::draw_button_text(std::string const & text, SDL_Rect abs_rect)
{
    auto result = _fm.text(text);
    vec const & size = std::get<0>(result);

    // center text in abs_rect
    point origin { abs_rect.x + (abs_rect.w - size.w) / 2, abs_rect.y + (abs_rect.h - size.h) / 2 };

    //blit(text_surf_ptr.get(), nullptr, &target_rect);
    run_copy_commands(std::get<1>(result), origin, _theme.button_fg_color);
}

void draw_context::draw_entry_box(SDL_Rect box, bool selected)
{
    set_color(_theme.entry_box_bg_color);
    SDL_RenderFillRect(_renderer, &box);
    set_color(selected ? _theme.entry_box_selected_frame_color : _theme.entry_box_frame_color);
    SDL_RenderDrawRect(_renderer, &box);
}

void draw_context::draw_entry_text(std::string text, SDL_Rect abs_rect, int texture_x_offset, int texture_y_offset)
{
    int const remaining_w = abs_rect.w - std::max(0, texture_x_offset);
    int const remaining_h = abs_rect.h - std::max(0, texture_y_offset);

    // Ignore if there is no intersection anymore.
    // Probably only useful to avoid text rendering.
    if (remaining_w >= 0 && remaining_h >= 0)
    {
        auto result = _fm.text(text);

        // Set clipping such that nothing gets drawn outside of the specified box.
        SDL_RenderSetClipRect(_renderer, &abs_rect);
        // Use a viewport to translate relative texture coordinates to the box.
        SDL_RenderSetViewport(_renderer, &abs_rect);
        run_copy_commands(std::get<1>(result), { texture_x_offset, texture_y_offset }, { 0, 0, 0 });
        SDL_RenderSetViewport(_renderer, nullptr);
        SDL_RenderSetClipRect(_renderer, nullptr);
    }
}

int draw_context::draw_label_text(SDL_Rect box, std::string text, int font_idx)
{
    auto result = _fm.text(text, box.w, font_idx);
    vec const & size = std::get<0>(result);

    point origin { box.x, box.y };

    // TODO fade effect when clipped?

    set_color({0, 0, 0});
    draw_rect_filled(box);

    SDL_RenderSetClipRect(_renderer, &box);
    run_copy_commands(std::get<1>(result), origin, { 255, 255, 255});
    SDL_RenderSetClipRect(_renderer, nullptr);
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

void draw_context::draw_entry_hightlighted_background(SDL_Rect box)
{
    set_color(_theme.entry_highlight_bg_color);
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
