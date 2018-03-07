#include <algorithm>

#include "region.hpp"

void region::apply_layout(SDL_Rect box)
{
    _box = box;

    // If we can't assing enough space for a region make at least a sane box.
    _box.w = std::max(0, _box.w);
    _box.h = std::max(0, _box.h);

    on_box_allocated();
}

void region::on_box_allocated()
{
}

bool region::can_use_intermediate_size() const
{
    return true;
}

SDL_Rect const & region::get_box() const
{
    return _box;
}

