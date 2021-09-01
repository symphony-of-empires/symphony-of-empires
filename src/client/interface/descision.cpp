#include "descision.hpp"
#include "../../event.hpp"
#include "../ui.hpp"

void create_descision(UI::Context* ui_ctx, Event& msg, std::vector<Event>& displayed_events) {
    UI::Window* popup_win = new UI::Window(128, 128, 320, 570);

    // TODO: Allow titles in events
    popup_win->text(msg.title.c_str());

    // Separate the text line by line
    const char* buf = msg.text.c_str();
    size_t y = 38;
    // TODO FIX
    char* tmpbuf = new char[512];
    while (strlen(buf)) {
        size_t t_len = std::min<size_t>(strlen(buf), 18);
        memcpy(tmpbuf, buf, t_len);
        tmpbuf[t_len] = '\0';
        new UI::Label(8, y, tmpbuf, popup_win);
        y += 24;
        buf += t_len;
    }

    // TODO FIX
    // Buttons for descisions
    // const UI::Button* last = nullptr;
    // for (const auto& descision : msg.descisions) {
    //     UI::Button* decide_btn = new UI::Button(9, 558 - 38, 303, 38, popup_win);
    //     decide_btn->text(descision.name.c_str());
    //     decide_btn->user_data = (void*)&descision;
    //     int i = 5;
    //     decide_btn->on_click = [](UI::Widget& w, void* data) {
    //         delete w.parent;

    //         Descision* descision = (Descision*)data;
    //         Event* decide_event = nullptr;
    //         // TODO fix
    //         // for (auto& e_event : displayed_events) {
    //         //     for (const auto& e_descision : e_event.descisions) {
    //         //         if (e_descision.ref_name == descision->ref_name) {
    //         //             decide_event = &e_event;
    //         //             break;
    //         //         }
    //         //     }

    //         //     if (decide_event != nullptr)
    //         //         break;
    //         // }

    //         // Event not found
    //         if (decide_event == nullptr) {
    //             print_error("Event not found, we tried finding by descision %s", descision->name.c_str());
    //             return;
    //         }

    //         g_client->packet_mutex.lock();
    //         Packet packet = Packet(g_client->get_fd());
    //         Archive ar = Archive();
    //         enum ActionType action = ACTION_NATION_TAKE_DESCISION;
    //         ::serialize(ar, &action);
    //         ::serialize(ar, &decide_event->ref_name);
    //         ::serialize(ar, &descision->ref_name);
    //         packet.data(ar.get_buffer(), ar.size());
    //         g_client->packet_queue.push_back(packet);
    //         g_client->packet_mutex.unlock();
    //         return;
    //     };

    //     if (last != nullptr) {
    //         decide_btn->above_of((*last));
    //     }
    //     last = decide_btn;
    // }

}