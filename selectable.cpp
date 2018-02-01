#include "selectable.hpp"

widget * selectable::find_selectable()
{
    return this;
}

widget * selectable::navigate_selectable(navigation_type nt)
{
    if (_parent == nullptr)
        return nullptr;
    else
        return _parent->navigate_selectable_from_children(nt, this, rect_center(_box));
}

selectable::~selectable()
{
}

