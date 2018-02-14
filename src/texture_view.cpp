#include "texture_view.hpp"

#include <SDL2/SDL_image.h>

texture_view::texture_view(unique_texture_ptr p, int min_width)
    : _p(std::move(p))
    , _size(texture_dim(_p.get()))
    , _min_width(std::min(min_width, _size.w))
    , _target { 0, 0, 0, 0 }
{
}

texture_view::texture_view()
    : _p()
    , _size { 1, 1 }
    , _min_width(0)
    , _target { 0, 0, 0, 0 }
{
}

texture_view::~texture_view()
{
}


void texture_view::on_draw(draw_context & dc, selection_context const & sc) const
{
    if (_p)
        dc.copy_texture(_p.get(), _target);
    else
        dc.draw_background(get_box());
}

void texture_view::apply_layout_to_children()
{
    if (_p)
        std::tie(_target, std::ignore, std::ignore) = scale_preserve_ar(texture_dim(_p.get()), get_box());
}

vec texture_view::min_size_hint() const
{
    double ratio = static_cast<double>(_min_width) / _size.w;
    return { _min_width, static_cast<int>(_size.h * ratio) };
}

int texture_view::height_for_width_hint(int width) const
{
    double ratio = static_cast<double>(width) / _size.w;
    return _size.h * ratio;
}

void texture_view::set_texture(unique_texture_ptr p, int min_width)
{
    bool was_nullptr = _p.operator bool();

    _p = std::move(p);
    if (_p)
    {
        _size = texture_dim(_p.get());
        _min_width = std::min(min_width, _size.w);
        mark_dirty();
    }
    else
    {
        _min_width = 0;
        if (!was_nullptr)
            mark_dirty();
    }
    _target = { 0, 0, 0, 0 };
}
