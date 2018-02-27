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
            int nat_inc_sum = 0;
            int num_expand = 0;
            vector<vec> min_sizes;
            vector<vec> nat_size_incs;
            min_sizes.reserve(n);
            nat_size_incs.reserve(n);

            for (auto & c : _children)
            {
                vec min_size = c.wptr->min_size_hint();
                min_sizes.push_back(min_size);

                if (_o == orientation::VERTICAL)
                {
                    min_sum += min_size.h;
                    int hfw = c.wptr->height_for_width_hint(get_box().w);

                    if (hfw == -1)
                    {
                        vec nat_size_inc = c.wptr->nat_size_inc_hint();
                        nat_size_incs.push_back(nat_size_inc);
                        nat_inc_sum += nat_size_inc.h;
                    }
                    else
                    {
                        int const nat_height = hfw - min_size.h;
                        nat_size_incs.push_back(
                            { max(0, get_box().w - min_size.w)
                            , max(0, nat_height)
                            });
                        nat_inc_sum += nat_height;
                    }
                }
                else
                {
                    min_sum += min_size.w;
                    vec nat_size_inc = c.wptr->nat_size_inc_hint();
                    nat_size_incs.push_back(nat_size_inc);
                    nat_inc_sum += nat_size_inc.w;
                }

                if (c.expand)
                    num_expand += 1;
            }

            int const max_size = _o == orientation::HORIZONTAL ? get_box().w : get_box().h;
            int const avail_after_spacing = max<int>(0, (max_size - (n - 1) * _children_spacing));

            // the number of widgets receiving extra space
            int const num_receiving = num_expand == 0 ? n : num_expand;

            int const remaining_space_with_min_size = avail_after_spacing - min_sum;

            bool const use_natural_size = min_sum + nat_inc_sum <= avail_after_spacing;
            bool const fill_to_natural = remaining_space_with_min_size > 0;

            // The amount that is used to partially fill the child to the
            // natural size.
            vector<int> partial_nat_size_incs;
            int extra_length = 0;
            int extra_length_rem = 0;
            if (use_natural_size)
            {
                // every widget fits with natural size
                int const remaining_space = avail_after_spacing - (min_sum + nat_inc_sum);
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

                int remaining_space = remaining_space_with_min_size;

                // This is used to avoid subtracting from all elements of the heap.
                int allocated = 0;

                // Differences and the corresponding index.
                auto cmp_first = [](auto a, auto b){ return get<0>(a) > get<0>(b); };
                vector<pair<int, size_t>> diff_heap;
                diff_heap.reserve(n);
                for (size_t k = 0; k < n; ++k)
                {
                    int size = _o == orientation::HORIZONTAL ? nat_size_incs[k].w : nat_size_incs[k].h;
                    diff_heap.emplace_back(size, k);
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
                        int const extra_space = remaining_space / diff_heap.size();
                        // TODO allocate int remainder
                        for (auto const & p : diff_heap)
                        {
                            tmp_partial_nat_size_incs[get<1>(p)] += extra_space;
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
                int const expand_width = c.expand ? extra_length + (extra_length_rem < k ? 1 : 0) : 0;


                vec const nat_expand = use_natural_size
                                     ? nat_size_incs[k]
                                     : ( fill_to_natural
                                       ? (_o == orientation::HORIZONTAL
                                         ? vec{ partial_nat_size_incs[k], 0 }
                                         : vec{ 0, partial_nat_size_incs[k] }
                                         )
                                       : vec { 0, 0 });

                vec const size = min_sizes[k] + nat_expand;

                if (_o == orientation::HORIZONTAL)
                {
                    // TODO should both lengths be limited?
                    int w = min(size.w + expand_width, get_box().w);
                    c.wptr->apply_layout({ offset, get_box().y, w, get_box().h });
                    offset += w + _children_spacing;
                }
                else
                {
                    int h = min(size.h + expand_width, get_box().h);
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
    int const spacing = _children.empty() ? 0 : (_children.size() - 1) * _children_spacing;
    vec result = { 0, 0 };

    if (_o == orientation::HORIZONTAL)
    {
        result.w = spacing;
    }
    else
    {
        result.h = spacing;
    }

    int homogeneous_max = 0;
    for (auto & c : _children)
    {
        vec size = c.wptr->min_size_hint();
        if (_o == orientation::HORIZONTAL)
        {
            result.h = std::max(size.h, result.h);
            if (_children_homogeneous)
                homogeneous_max = std::max(size.w, homogeneous_max);
            else
                result.w += size.w;

        }
        else
        {
            if (_children_homogeneous)
                homogeneous_max = std::max(size.h, homogeneous_max);
            else
                result.h += size.h;
            result.w = std::max(size.w, result.w);
        }
    }

    if (_children_homogeneous)
    {
        if (_o == orientation::HORIZONTAL)
        {
            result.w += homogeneous_max * _children.size();
        }
        else
        {
            result.h += homogeneous_max * _children.size();
        }
    }

    return result;
}

vec box::nat_size_inc_hint() const
{
    vec result = { 0, 0 };

    std::vector<std::size_t> hfw_indices;

    for (std::size_t k = 0; k < _children.size(); ++k)
    {
        auto const & c = _children[k];
        vec const size = c.wptr->nat_size_inc_hint();
        if (_o == orientation::HORIZONTAL)
        {
            result.h = std::max(size.h, result.h);
            result.w += size.w;
        }
        else
        {
            // TODO not optimal since we can't use it properly
            int const hfw = c.wptr->height_for_width_hint(get_box().w);
            if (hfw == -1)
            {
                result.h += size.h;
                result.w = std::max(size.w, result.w);
            }
            else
            {
                hfw_indices.push_back(k);
            }
        }
    }

    // Rescale height-for-width widgets with maximum width.
    for (auto hfw_idx : hfw_indices)
    {
        int const hfw = _children[hfw_idx].wptr->height_for_width_hint(result.w);
        vec const min_size = _children[hfw_idx].wptr->min_size_hint();
        result.h += hfw - min_size.h;
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

