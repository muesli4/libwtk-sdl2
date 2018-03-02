#include <algorithm>
#include <iterator>

#include "box.hpp"

// TODO default setting singleton
box::box(orientation o, box::children_type children)
    : box(o, children, 5, false)
{
}

box::box(orientation o, box::children_type children, int children_spacing)
    : box(o, children, children_spacing, false)
{
}

box::box(orientation o, box::children_type children, bool children_homogeneous)
    : box(o, children, 5, children_homogeneous)
{
}

box::box(orientation o, box::children_type children, int children_spacing, bool children_homogeneous)
    : _children(children)
    , _children_spacing(children_spacing)
    , _children_homogeneous(children_homogeneous)
    , _o(o)
{
    init_children();
}

#include <iostream>
void box::apply_layout_to_children()
{
    std::size_t const n = _children.size();

    if (n > 0)
    {
        if (_children_homogeneous)
        {
            // TODO use minimum width to compute minimum
            if (_o == orientation::HORIZONTAL)
            {
                // evenly split box
                int const avail_width = get_box().w - (n - 1) * _children_spacing;
                int const child_width_div = avail_width / n;
                int const child_width_rem = avail_width % n;

                int xoffset = get_box().x;

                for (std::size_t k = 0; k < n; ++k)
                {
                    int const extra_width = k >= (n - child_width_rem) ? 1 : 0;
                    int const child_width = child_width_div + extra_width;

                    _children[k].wptr->apply_layout({ xoffset, get_box().y, child_width, get_box().h });
                    xoffset += child_width + _children_spacing;
                }
            }
            else
            {
                int const avail_height = get_box().h - (n - 1) * _children_spacing;
                int const child_height_div = avail_height / n;
                int const child_height_rem = avail_height % n;

                int yoffset = get_box().y;

                for (std::size_t k = 0; k < n; ++k)
                {
                    int const extra_height = k >= (n - child_height_rem) ? 1 : 0;
                    int const child_height = child_height_div + extra_height;

                    _children[k].wptr->apply_layout({ get_box().x, yoffset, get_box().w, child_height });
                    yoffset += child_height + _children_spacing;
                }
            }
        }
        else
        {
            // Considerations:
            // - height-for-width is similar to the natural size of the widget.

            // Algorithm outline:
            // 1. Ask for minimum and natural size of each widget
            //    (height-for-width instead of natural size if supported).
            // 2. Calculate difference with available space for both minimal and
            //    natural size.
            // 3. If natural size will fit for every widget use natural size,
            //    otherwise use minimum size.
            // 4. Distribute left-over space to widgets with expand property.
            using namespace std;

            int min_sum = 0;
            int nat_sum = 0;
            int num_expand = 0;
            vector<size_hint> size_hints;
            size_hints.reserve(n);

            for (auto & c : _children)
            {
                auto & wptr = c.wptr;

                if (_o == orientation::VERTICAL)
                {
                    size_hints.push_back(wptr->get_size_hint(get_box().w, -1));
                    min_sum += size_hints.back().minimal.h;
                    nat_sum += size_hints.back().natural.h;
                }
                else
                {
                    size_hints.push_back(wptr->get_size_hint(-1, get_box().h));
                    min_sum += size_hints.back().minimal.w;
                    nat_sum += size_hints.back().natural.w;
                }

                if (c.expand)
                    num_expand += 1;
            }

            int const max_size = _o == orientation::HORIZONTAL ? get_box().w : get_box().h;
            int const avail_after_spacing = max<int>(0, (max_size - (n - 1) * _children_spacing));

            // the number of widgets receiving extra space
            int const num_receiving = num_expand == 0 ? n : num_expand;

            int const remaining_space_with_min_size = avail_after_spacing - min_sum;

            bool const use_natural_size = nat_sum <= avail_after_spacing;
            bool const fill_to_natural = remaining_space_with_min_size > 0;

            // The amount that is used to partially fill the child to the
            // natural size.
            vector<int> partial_nat_size_incs;
            int extra_length = 0;
            int extra_length_rem = 0;
            if (use_natural_size)
            {
                // every widget fits with natural size
                int const remaining_space = avail_after_spacing - nat_sum;
                extra_length = remaining_space / num_receiving;
                extra_length_rem = remaining_space % num_receiving;
            }
            else if (fill_to_natural)
            {
                // 1. Sort differences between natural size (or use min heap).
                // 2. While there is the minimum difference times the remaining
                //    widgets space left:
                //    Fill all remaining widgets equally by the minimum
                //    difference. Subtract that from every remaining difference.
                //    TODO add option to fill all widgets to a percentage
                //    TODO filling wfh/hfw widgets this way is not optimal

                int remaining_space = remaining_space_with_min_size;

                // This is used to avoid subtracting from all elements of the heap.
                int allocated = 0;

                // Differences and the corresponding index.
                auto cmp_first = [](auto a, auto b){ return get<0>(a) > get<0>(b); };
                vector<pair<int, size_t>> diff_heap;
                diff_heap.reserve(n);
                for (size_t k = 0; k < n; ++k)
                {
                    if (_children[k].wptr->can_use_intermediate_size())
                    {
                        auto const & sh = size_hints[k];
                        // TODO limit to 0 ?
                        int const nat_inc = _o == orientation::HORIZONTAL ? sh.natural.w - sh.minimal.w : sh.natural.h - sh.minimal.h;
                        diff_heap.emplace_back(nat_inc, k);
                    }
                }

                make_heap(begin(diff_heap), end(diff_heap), cmp_first);

                vector<int> tmp_partial_nat_size_incs(n, 0);
                do
                {
                    pop_heap(begin(diff_heap), end(diff_heap), cmp_first);
                    int min_idx;
                    int min_diff;
                    tie(min_diff, min_idx) = diff_heap.back();
                    {
                        int const new_allocated = min_diff;
                        min_diff -= allocated;
                        allocated = new_allocated;
                    }

                    int const next_allocation = min_diff * diff_heap.size();
                    // can we allocate the current size for each?
                    if (next_allocation <= remaining_space)
                    {
                        // allocate min diff for every widget (including the popped one)
                        for (auto const & p : diff_heap)
                        {
                            tmp_partial_nat_size_incs[get<1>(p)] += min_diff;
                        }

                        diff_heap.pop_back();
                    }
                    else
                    {
                        // allocate otherwise remaining
                        int const extra_space_div = remaining_space / diff_heap.size();
                        int const extra_space_rem = remaining_space % diff_heap.size();

                        for (std::size_t k = 0; k < diff_heap.size(); ++k)
                        {
                            tmp_partial_nat_size_incs[get<1>(diff_heap[k])] += extra_space_div + (k >= diff_heap.size() - extra_space_rem ? 1 : 0);
                        }
                        break;
                    }

                    remaining_space -= next_allocation;
                }
                // Protect against violated pre-conditions. E.g., when there is
                // not enough space for the widgets.
                // TODO figure out when this happens
                while (!diff_heap.empty());
                partial_nat_size_incs.swap(tmp_partial_nat_size_incs);
            }

            int offset = _o == orientation::HORIZONTAL ? get_box().x : get_box().y;

            for (size_t k = 0; k < n; k++)
            {
                auto & c = _children[k];

                // TODO evenly distribute remainder?
                // The space expanding widgets receive after every widget is
                // able to get its natural size.
                int const expand_width = c.expand ? extra_length + (k >= (n - extra_length_rem) ? 1 : 0) : 0;
                int const extra_length = expand_width + (!use_natural_size && fill_to_natural ? partial_nat_size_incs[k] : 0);

                auto const & sh = size_hints[k];
                vec const & size = use_natural_size ? sh.natural : sh.minimal;

                if (_o == orientation::HORIZONTAL)
                {
                    // TODO should both lengths be limited?
                    int w = min(size.w + extra_length, get_box().w);
                    c.wptr->apply_layout({ offset, get_box().y, w, get_box().h });
                    offset += w + _children_spacing;
                }
                else
                {
                    int h = min(size.h + extra_length, get_box().h);
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

size_hint box::get_size_hint(int width, int height) const
{
    int const spacing = _children.empty() ? 0 : (_children.size() - 1) * _children_spacing;
    vec minimal = { 0, 0 };
    vec natural = { 0, 0 };

    int hom_minimal_max = 0;
    int hom_natural_max = 0;
    for (auto & c : _children)
    {
        if (_o == orientation::HORIZONTAL)
        {
            auto sh = c.wptr->get_size_hint(-1, height);

            minimal.h = std::max(sh.minimal.h, minimal.h);
            natural.h = std::max(sh.natural.h, natural.h);
            if (_children_homogeneous)
            {
                hom_minimal_max = std::max(sh.minimal.w, hom_minimal_max);
                hom_natural_max = std::max(sh.natural.w, hom_natural_max);
            }
            else
            {
                minimal.w += sh.minimal.w;
                natural.w += sh.natural.w;
            }
        }
        else
        {
            auto sh = c.wptr->get_size_hint(width, -1);

            minimal.w = std::max(sh.minimal.w, minimal.w);
            natural.w = std::max(sh.natural.w, natural.w);
            if (_children_homogeneous)
            {
                hom_minimal_max = std::max(sh.minimal.h, hom_minimal_max);
                hom_natural_max = std::max(sh.natural.h, hom_natural_max);
            }
            else
            {
                minimal.h += sh.minimal.h;
                natural.h += sh.natural.h;
            }
        }
    }

    if (_children_homogeneous)
    {
        if (_o == orientation::HORIZONTAL)
        {
            minimal.w += hom_minimal_max * _children.size() + spacing;
            natural.w += hom_natural_max * _children.size() + spacing;
        }
        else
        {
            minimal.h += hom_minimal_max * _children.size() + spacing;
            natural.h += hom_natural_max * _children.size() + spacing;
        }
    }

    return size_hint(minimal, natural);
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

widget_ptr hbox(box::children_type children)
{
    return std::make_shared<box>(box::orientation::HORIZONTAL, children);
}

widget_ptr vbox(box::children_type children)
{
    return std::make_shared<box>(box::orientation::VERTICAL, children);
}


widget_ptr hbox(box::children_type children, int children_spacing)
{
    return std::make_shared<box>(box::orientation::HORIZONTAL, children, children_spacing);
}

widget_ptr vbox(box::children_type children, int children_spacing)
{
    return std::make_shared<box>(box::orientation::VERTICAL, children, children_spacing);
}


widget_ptr hbox(box::children_type children, bool children_homogeneous)
{
    return std::make_shared<box>(box::orientation::HORIZONTAL, children, children_homogeneous);
}

widget_ptr vbox(box::children_type children, bool children_homogeneous)
{
    return std::make_shared<box>(box::orientation::VERTICAL, children, children_homogeneous);
}

widget_ptr hbox(box::children_type children, int children_spacing, bool children_homogeneous)
{
    return std::make_shared<box>(box::orientation::HORIZONTAL, children, children_spacing, children_homogeneous);
}

widget_ptr vbox(box::children_type children, int children_spacing, bool children_homogeneous)
{
    return std::make_shared<box>(box::orientation::VERTICAL, children, children_spacing, children_homogeneous);
}

