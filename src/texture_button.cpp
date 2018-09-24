#include "texture_button.hpp"

texture_button::texture_button(shared_texture_ptr texture, std::function<void()> callback)
    : button(callback)
    , _texture(texture)
{
}

texture_button::~texture_button()
{
}

void texture_button::set_texture(shared_texture_ptr texture)
{
    _texture = texture;
}

void texture_button::draw_drawable(draw_context & dc, rect box) const
{
    // TODO cache ?
    rect tex_box = center_vec_within_rect(texture_dim(_texture.get()), get_box());
    // TODO allow nullptr? how to prevent it?
    dc.copy_texture(_texture.get(), tex_box);
}

vec texture_button::get_drawable_size() const
{
    return texture_dim(_texture.get());
}

