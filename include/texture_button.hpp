#ifndef LIBWTK_SDL2_TEXTURE_BUTTON_HPP
#define LIBWTK_SDL2_TEXTURE_BUTTON_HPP

#include "button.hpp"

struct texture_button : button
{
    texture_button(shared_texture_ptr texture, std::function<void()> callback);

    ~texture_button() override;

    /**
     * @name Texture Button Interface
     * @{
     */

    void set_texture(shared_texture_ptr texture);

    /** @} */

    private:

    void draw_drawable(draw_context & dc, rect box) const override;
    vec get_drawable_size() const override;

    shared_texture_ptr _texture;
};

#endif

