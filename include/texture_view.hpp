#ifndef LIBWTK_SDL2_TEXTURE_VIEW_HPP
#define LIBWTK_SDL2_TEXTURE_VIEW_HPP

#include <string>

#include <SDL2/SDL.h>

#include "geometry.hpp"
#include "sdl_util.hpp"
#include "widget.hpp"

/**
 * A widget that displays a texture with the correct aspect ratio. The widget
 * takes ownership of the texture.
 *
 * Currently the widget centers the texture within its box. As that behavior is
 * inconsistent with that of other widgets it may change in the future.
 */
struct texture_view : widget
{
    texture_view();
    
    /**
     * Create a texture view by moving a texture pointer.
     *
     * One may give the minimum and natural width. The minimum width will
     * default to half of the width of the texture. The natural width will
     * default to the textures width.
     */
    texture_view(unique_texture_ptr p, int min_width = -1, int nat_width = -1);

    ~texture_view() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_box_allocated() override;

    size_hint get_size_hint(int width, int height) const override;

    /**
     * @name Texture View Interface
     * @{
     */

    void set_texture(unique_texture_ptr p, int min_width = -1, int nat_width = -1);

    /** @} */

    private:

    void refresh_target();
    vec fit_to_width(int width) const;
    int decode_min_width_param(int min_width) const;
    int decode_nat_width_param(int nat_width) const;

    unique_texture_ptr _p;
    vec _size;
    int _min_width;
    int _nat_width;
    rect _target;
};

#endif

