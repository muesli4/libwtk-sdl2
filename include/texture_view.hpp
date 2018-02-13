#ifndef TEXTURE_VIEW_HPP
#define TEXTURE_VIEW_HPP

#include <string>

#include <SDL2/SDL.h>

#include "widget.hpp"
#include "sdl_util.hpp"

// A widget that displays a texture with the correct aspect ratio. The widget
// takes ownership of the texture.
struct texture_view : widget
{
    texture_view(unique_texture_ptr p, int min_width = 100); //texture_dim(p.get()).w / 2);
    ~texture_view() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    vec min_size_hint() const override;

    void apply_layout_to_children() override;

    int height_for_width_hint(int width) const override;

    // texture_view interface

    void set_texture(unique_texture_ptr p, int min_width = 100);

    private:

    unique_texture_ptr _p;
    vec _size;
    int _min_width;
    SDL_Rect _target;
};

#endif

