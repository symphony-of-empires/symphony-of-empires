#include "descision.hpp"

#include "../../event.hpp"
#include "../ui.hpp"
#include "../../serializer.hpp"
#include "../../actions.hpp"
#include "../../io_impl.hpp"

DescisionWindow::DescisionWindow(GameState& gs, Event& msg) : UI::Window(128, 128, 320, 570) {
    // TODO: Allow titles in events
    text(msg.title.c_str());

    // Separate the text line by line
    const char* buf = msg.text.c_str();
    size_t y = 38;
    // TODO FIX
    char* tmpbuf = new char[512];
    while (strlen(buf)) {
        size_t t_len = std::min<size_t>(strlen(buf), 18);
        memcpy(tmpbuf, buf, t_len);
        tmpbuf[t_len] = '\0';
        new UI::Label(8, y, tmpbuf, this);
        y += 24;
        buf += t_len;
    }

    // Buttons for descisions
    const DescisionButton* last = nullptr;
    for (const auto& descision : msg.descisions) {
        DescisionButton* decide_btn = new DescisionButton(this, gs, descision, msg);
        if (last != nullptr) {
            decide_btn->above_of((*last));
        }
        last = decide_btn;
    }
}

DescisionButton::DescisionButton(UI::Window* parent, GameState& _gs, const Descision& _descision, Event& msg)
    : gs{_gs}, descision{_descision}, event{msg}, UI::Button(9, 558 - 38, 303, 38, parent) {
    text(descision.name.c_str());
    user_data = (void*)&descision;
    on_click = [](UI::Widget& w, void* data) {
        DescisionButton& state = dynamic_cast<DescisionButton&>(w);

        Archive ar = Archive();
        ActionType action = ActionType::NATION_TAKE_DESCISION;
        ::serialize(ar, &action);
        ::serialize(ar, &state.event.ref_name);
        ::serialize(ar, &state.descision.ref_name);
        state.gs.send_command(ar);

        delete w.parent;
    };
}