#include <algorithm>
#include <iterator>

#include "box.hpp"

box::box(orientation o, std::vector<box::child> children, int children_spacing, bool children_homogeneous)
    : _children(children)
    , _children_spacing(children_spacing)
    , _children_homogeneous(children_homogeneous)
    , _o(o)
{
    init_children();
}

void box::apply_layout_to_children()
{
    std::size_t const n = _children.size();

    if (n > 0)
    {
        if (_children_homogeneous)
        {
            if (_o == orientation::HORIZONTAL)
            {
                // evenly split box
                int const child_width = (get_box().w - (n - 1) * _children_spacing) / n;
                int xoffset = get_box().x;

                for (auto c : _children)
                {
                    c.wptr->apply_layout({ xoffset, get_box().y, child_width, get_box().h });
                    xoffset += child_width + _children_spacing;
                }
            }
            else
            {
                int const child_height = (get_box().h - (n - 1) * _children_spacing) / n;
                int yoffset = get_box().y;

                for (auto c : _children)
                {
                    c.wptr->apply_layout({ get_box().x, yoffset, get_box().w, child_height });
                    yoffset += child_height + _children_spacing;
                }
            }
        }
        else
        {

            // 1. Ask for minimum size of each widget
            // 2. Calculate difference with available space
            // 3. Distribute left-over space to widgets with expand property
            // TODO give expand weight for each widget instead
            // TODO natural size property, width-for-height, etc.
            using namespace std;

            int sum = 0;
            int num_expand = 0;
            vector<vec> min_sizes;
            for (auto & c : _children)
            {
                int hfw = _o == orientation::VERTICAL ? c.wptr->height_for_width_hint(get_box().w) : -1;
                vec size;
                if (hfw  == -1)
                {
                    size = c.wptr->min_size_hint();
                }
                else
                {
                    size = { get_box().w, hfw };
                }

                min_sizes.push_back(size);
                if (c.expand)
                    num_expand += 1;

                if (_o == orientation::HORIZONTAL)
                {
                    sum = sum + size.w;
                }
                else
                {
                    sum = sum + size.h;
                }

            }

            int const avail = std::max<int>(0, ((_o == orientation::HORIZONTAL ? get_box().w : get_box().h) - sum - (n - 1) * _children_spacing));
            int const extra_width = avail / (num_expand == 0 ? n : num_expand);
            int offset = _o == orientation::HORIZONTAL ? get_box().x : get_box().y;

            for (std::size_t k = 0; k < n; k++)
            {
                auto & c = _children[k];

                int expand_width = (c.expand || num_expand == 0) ? extra_width : 0;

                if (_o == orientation::HORIZONTAL)
                {
                    // TODO should both lengths be limited?
                    int w = std::min(min_sizes[k].w + expand_width, get_box().w);
                    c.wptr->apply_layout({ offset, get_box().y, w, get_box().h });
                    offset += w + _children_spacing;
                }
                else
                {
                    int h = std::min(min_sizes[k].h + expand_width, get_box().h);
                    c.wptr->apply_layout({ get_box().x, offset, get_box().w, h });
                    offset += h + _children_spacing;
                }
            }
        }
    }
}

widget * box::find_selectable(navigation_type nt, point center)
{
    // When the navigation request is orthogonal, pick the widget with the
    // closest distance on that particular dimension.
    if (is_orthogonal(nt))
    {

        widget * min_w;
        int min_distance;

        std::size_t k = 0;

        // Find a selectable widget for a start.
        for (; k < _children.size(); ++k)
        {
            min_w = _children[k].wptr->find_selectable(nt, center);

            if (min_w != nullptr)
            {
                auto found_widget_center = rect_center(min_w->get_box());
                min_distance = relevant_distance(center, found_widget_center);
                break;
            }
        }

        if (min_w == nullptr)
            return nullptr;
        else
        {
            // Refine by finding a selectable widget with a smaller distance.
            for (++k; k < _children.size(); ++k)
            {
                auto new_w = _children[k].wptr->find_selectable(nt, center);

                if (new_w != nullptr)
                {
                    auto new_center = rect_center(new_w->get_box());
                    int new_distance = relevant_distance(center, new_center);

                    if (new_distance < min_distance)
                    {
                        min_w = new_w;
                        min_distance = new_distance;
                    }
                    else
                    {
                        // Assumption: Parallel widget dimension increases with
                        // order in container.
                        break;
                    }
                }
            }

            return min_w;
        }
    }
    // Otherwise just pick the first selectable one in the direction of the
    // request.
    else
    {
        if (is_forward(nt))
        {
            for (auto c : _children)
            {
                auto w = c.wptr->find_selectable(nt, center);
                if (w != nullptr)
                {
                    return w;
                }
            }
        }
        else
        {
            for (auto it = _children.rbegin(); it != _children.rend(); ++it)
            {
                auto w = it->wptr->find_selectable(nt, center);
                if (w != nullptr)
                {
                    return w;
                }
            }
        }
    }
    return nullptr;
}

bool box::is_forward(navigation_type nt)
{
    return
        nt == navigation_type::NEXT
        || nt == navigation_type::NEXT_X
        || nt == navigation_type::NEXT_Y;
}

widget * box::navigate_selectable_from_children(navigation_type nt, widget * w, point center)
{
    // When navigating in a linear container like box, we don't have to consider
    // the center hint, as we're either orthogonal with the navigation request,
    // then the parent will handle it, or we're parallel to it, then we just
    // need to pick a consecutive widget.

    if (is_orthogonal(nt))
    {
        return navigate_selectable_parent(nt, center);
    }
    else
    {
        auto const begin = _children.begin();
        auto const end = _children.end();

        // locate widget in container
        auto it = std::find_if(begin, end, [w](child const & c){ return c.wptr.get() == w; });

        // find first selectable widget of all navigationally consecutive widgets
        while (true)
        {
            if (is_forward(nt))
            {
                it++;
            }
            else
            {
                it--;
            }

            if (it == end || it < begin)
            {
                return navigate_selectable_parent(nt, center);
            }
            else
            {
                auto result = it->wptr->find_selectable(nt, center);
                if (result != nullptr)
                    return result;
            }
        }
    }
}

vec box::min_size_hint() const
{
    vec result = { 0, 0 };

    for (auto & c : _children)
    {
        vec size = c.wptr->min_size_hint();
        if (_o == orientation::HORIZONTAL)
        {
            result.h = std::max(size.h, result.h);
            result.w += size.w;
        }
        else
        {
            result.h += size.h;
            result.w = std::max(size.w, result.w);
        }
    }
    return result;
}

vec box::nat_size_inc_hint() const
{
    // TODO refactor
    vec result = { 0, 0 };

    for (auto & c : _children)
    {
        vec size = c.wptr->nat_size_inc_hint();
        if (_o == orientation::HORIZONTAL)
        {
            result.h = std::max(size.h, result.h);
            result.w += size.w;
        }
        else
        {
            result.h += size.h;
            result.w = std::max(size.w, result.w);
        }
    }
    return result;
}

box::~box()
{
}

int box::relevant_distance(point old_center, point new_center) const
{
    return std::abs(_o == orientation::HORIZONTAL ? old_center.x - new_center.x : old_center.y - new_center.y);
}

bool box::is_orthogonal(navigation_type nt) const
{
    return
        ((nt == navigation_type::NEXT_X || nt == navigation_type::PREV_X)
            && _o == orientation::VERTICAL
        )
        ||
        ((nt == navigation_type::NEXT_Y || nt == navigation_type::PREV_Y)
            && _o == orientation::HORIZONTAL
        );
}


std::vector<widget *> box::get_children()
{
    std::vector<widget *> result;
    std::for_each(std::begin(_children), std::end(_children), [&](auto c){ result.push_back(c.wptr.get()); });
    return result;
}

std::vector<widget const *> box::get_children() const
{
    std::vector<widget const *> result;
    std::for_each(std::begin(_children), std::end(_children), [&](auto c){ result.push_back(c.wptr.get()); });
    return result;
}

widget_ptr hbox(std::vector<box::child> children, int children_spacing, bool children_homogeneous)
{
    return std::make_shared<box>(box::orientation::HORIZONTAL, children, children_spacing, children_homogeneous);
}

widget_ptr vbox(std::vector<box::child> children, int children_spacing, bool children_homogeneous)
{
    return std::make_shared<box>(box::orientation::VERTICAL, children, children_spacing, children_homogeneous);
}

