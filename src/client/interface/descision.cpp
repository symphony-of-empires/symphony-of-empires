#include "client/interface/descision.hpp"

#include "event.hpp"
#include "unified_render/serializer.hpp"
#include "action.hpp"
#include "io_impl.hpp"

using namespace Interface;

DescisionWindow::DescisionWindow(GameState& gs, Event& event)
    : UI::Window(128, 128, 320, 570)
{
    // Title of the event
    this->text(event.title.c_str());

    // Body of the event text
    auto* txt = new UI::Text(0, 0, 18 * 24, 24, this);
    txt->text(event.text);

    this->height = txt->height + (event.descisions.size() * 24);

    // Buttons for descisions for the event
    const DescisionButton* last = nullptr;
    for(const auto& descision : event.descisions) {
        DescisionButton* decide_btn = new DescisionButton(this, gs, descision, event);
        if(last != nullptr) {
            decide_btn->above_of(*last);
        }
        last = decide_btn;
    }
}

DescisionButton::DescisionButton(UI::Window* parent, GameState& _gs, const Descision& _descision, Event& _event)
    : UI::Button(0, parent->height - 24, parent->width, 24, parent),
    gs{ _gs },
    descision{ _descision },
    event{ _event }
{
    this->text(descision.name);
    this->user_data = this;
    this->on_click = [](UI::Widget& w, void* data) {
        auto& o = static_cast<DescisionButton&>(w);

        Archive ar = Archive();
        ActionType action = ActionType::NATION_TAKE_DESCISION;
        ::serialize(ar, &action);
        ::serialize(ar, &o.event.ref_name);
        ::serialize(ar, &o.descision.ref_name);
        o.gs.send_command(ar);

        w.parent->kill();
    };
}