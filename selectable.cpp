#include "selectable.hpp"

widget * selectable::find_selectable(navigation_type nt)
{
    return this;
}

widget * selectable::navigate_selectable(navigation_type nt)
{
    return navigate_selectable_parent(nt, rect_center(_box));
}

selectable::~selectable()
{
}

