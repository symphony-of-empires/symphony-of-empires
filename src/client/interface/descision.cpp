#include "descision.hpp"

#include "../../event.hpp"
#include "../../serializer.hpp"
#include "../../actions.hpp"
#include "../../io_impl.hpp"

using namespace Interface;

DescisionWindow::DescisionWindow(GameState& gs, Event& msg): UI::Window(128, 128, 320, 570) {
    // Title of the event
    this->text(msg.title.c_str());

    // Body of the event text
    UI::Text* txt = new UI::Text(0, y, 18 * 24, 24, this);
    txt->text(msg.text);

    // Buttons for descisions for the event
    const DescisionButton* last = nullptr;
    for(const auto& descision : msg.descisions) {
        DescisionButton* decide_btn = new DescisionButton(this, gs, descision, msg);
        if(last != nullptr) {
            decide_btn->above_of((*last));
        }
        last = decide_btn;
    }
}

DescisionButton::DescisionButton(UI::Window* parent, GameState& _gs, const Descision& _descision, Event& msg)
    : gs{ _gs },
    descision{ _descision },
    event{ msg },
    UI::Button(0, parent->height - 24, parent->width, 24, parent)
{
    this->text(descision.name);
    this->user_data = this;
    this->on_click = [](UI::Widget& w, void* data) {
        auto& state = static_cast<DescisionButton&>(w);

        Archive ar = Archive();
        ActionType action = ActionType::NATION_TAKE_DESCISION;
        ::serialize(ar, &action);
        ::serialize(ar, &state.event.ref_name);
        ::serialize(ar, &state.descision.ref_name);
        state.gs.send_command(ar);

        delete w.parent;
    };
}