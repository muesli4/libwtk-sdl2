#include <algorithm>

#include "radio_button.hpp"
#include "box.hpp"

radio_button::group::group(radio_button::callback_type && callback)
    : _callback(callback)
    , _active_radio_button(nullptr)
{
}

void radio_button::group::add(radio_button & rb)
{
    auto const end = std::end(_radio_buttons);
    auto it = std::find(std::begin(_radio_buttons), end, &rb);
    if (it == end)
    {
        _radio_buttons.push_back(&rb);
    }
}

void radio_button::group::remove(radio_button const & rb)
{
    auto const end = std::end(_radio_buttons);
    auto it = std::find(std::begin(_radio_buttons), end, &rb);
    if (it != end)
    {
        _radio_buttons.erase(it);
        if (_active_radio_button == &rb)
        {
            _active_radio_button = nullptr;
        }
    }
}

void radio_button::group::set_active(radio_button const & rb)
{
    if (!is_active(rb))
    {
        auto const end = std::end(_radio_buttons);
        auto const begin = std::begin(_radio_buttons);
        auto it = std::find(begin, end, &rb);
        if (it != end)
        {
            if (_active_radio_button != nullptr)
            {
                _active_radio_button->mark_dirty();
            }
            _active_radio_button = *it;
            _active_radio_button->mark_dirty();
            _callback(std::distance(begin, it));
        }
    }
}

bool radio_button::group::is_active(radio_button const & rb)
{
    return &rb == _active_radio_button;
}

radio_button::radio_button(std::string label, radio_button::callback_type callback)
    : radio_button(label, std::make_shared<group>(std::move(callback)))
{
}

radio_button::radio_button(std::string label, radio_button & other)
    : radio_button(label, other._group)
{
}

radio_button::radio_button(std::string label, std::shared_ptr<group> g)
    : _label(label)
    , _group(std::move(g))
{
    _group->add(*this);
    mark_dirty();
}

radio_button::~radio_button()
{
    _group->remove(*this);
}

void radio_button::select()
{
    _group->set_active(*this);
}

void radio_button::on_draw(draw_context & dc, selection_context const & sc) const
{
    // draw a radio box on the left
    auto box = get_box();
    auto box_len = get_context_info().font_line_skip();
    auto box_spacing = box_len / 3;

    rect radio_box_box = { box.x, box.y, box_len, box_len };
    dc.draw_radio_entry(radio_box_box, _group->is_active(*this), sc.is_selected_widget(this));
    // draw the label on the right
    // TODO make spacing depend on font size ?
    rect text_box = { box.x + box_len + box_spacing
                    , box.y
                    , box.w - box_len - box_spacing
                    , box.h
                    };
    dc.draw_label_text(text_box, _label, true);
}

void radio_button::on_activate()
{
    select();
}

void radio_button::on_mouse_up_event(mouse_up_event const & e)
{
    if (within_rect(e.position, get_box()))
    {
        bool const went_down = e.opt_movement.has_value();
        if (!went_down || (went_down && within_rect(e.opt_movement.value().origin, get_box())))
        {
            select();
        }
    }
}

void radio_button::on_key_event(key_event const & e)
{
}

size_hint radio_button::get_size_hint(int width, int height) const
{
    auto min_len = get_context_info().font_line_skip();
    vec tsize = get_context_info().text_size(_label, width);
    return {{ min_len + 5 + tsize.w, std::max(tsize.h, min_len) }};
}

widget_ptr radio_box_from_labels(std::vector<std::string> labels, radio_button::callback_type callback)
{
    std::shared_ptr<radio_button> last_radio_button;
    box::children_type cs;

    if (!labels.empty())
    {
        last_radio_button = std::make_shared<radio_button>(labels[0], callback);
        for (std::size_t k = 1; k < labels.size(); ++k)
        {
            cs.push_back({false, last_radio_button});
            last_radio_button = std::make_shared<radio_button>(labels[k], *last_radio_button);

        }
        cs.push_back({false, last_radio_button});
    }

    return vbox(cs);
}
