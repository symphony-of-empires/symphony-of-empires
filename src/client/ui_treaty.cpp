#include "render/texture.hpp"
#include "ui.hpp"
#include "../path.hpp"

extern UI::Window* top_win;
extern std::pair<int, int> mouse_pos;

#include "../nation.hpp"
extern Nation* curr_nation;

#include "../serializer.hpp"
#include "../io_impl.hpp"
#include "client_network.hpp"

UI::Label* recv_nation_lab;
UI::Image* recv_nation_flag;
UI::Window* select_win;
Nation* recv_nation = nullptr;

extern const UnifiedRender::Texture& get_nation_flag(Nation& nation);
enum TreatyClauseType g_clause_type;

#include <mutex>
Treaty g_treaty_draft;
std::mutex g_treaty_draft_mutex;


void ui_treaty(UI::Window* top_win) {}

// void ui_treaty(UI::Window* top_win) {
//     g_treaty_draft.clauses.clear();

//     UI::Window* treaty_win = new UI::Window(0, 0, 320, 425);
//     treaty_win->text("Draft Treaty");
//     treaty_win->below_of((*top_win));

//     UI::Button* select_receiver_btn = new UI::Button(9, 43, 303, 38, treaty_win);
//     select_receiver_btn->text("Select nation");
//     select_receiver_btn->on_click = [](UI::Widget&, void*) {
//         select_win = new UI::Window(0, 0, 320, 425);
//         select_win->text("Select nation");

//         int y = 0;
//         for(const auto& nation: g_world->nations) {
//             UI::Button* select_receiver_btn = new UI::Button(9, y, 303, 38, select_win);
//             select_receiver_btn->text(nation->name.c_str());
//             select_receiver_btn->user_data = (void*)nation;
//             select_receiver_btn->on_click = [](UI::Widget&, void* data) {
//                 recv_nation = (Nation*)data;
//                 delete select_win;
//             };
//             y += 38 + 2;
//         }
//     };

//     recv_nation_lab = new UI::Label(64, 44, "...", treaty_win);
//     recv_nation_lab->below_of((*select_receiver_btn));
//     recv_nation_lab->on_update = [](UI::Widget& w, void*) {
//         if(recv_nation == nullptr)
//             return;
//         w.text(recv_nation->name.c_str());
//     };

//     recv_nation_flag = new UI::Image(0, recv_nation_lab->y, 58, 24, nullptr, treaty_win);
//     recv_nation_flag->on_update = [](UI::Widget& w, void*) {
//         if(recv_nation == nullptr)
//             return;
//         w.current_texture = &get_nation_flag(*recv_nation);
//     };

//     UI::Button* new_clause = new UI::Button(9, 0, 303, 38, treaty_win);
//     new_clause->below_of((*recv_nation_lab));
//     new_clause->text("Add new clause");
//     new_clause->on_click = [](UI::Widget&, void*) {
//         UI::Window* new_clause_win = new UI::Window(0, 0, 320, 425);
//         new_clause_win->text("Add new clause");

//         UI::Button* list_btn;
//         int y = 0;
        
//         y += 24;
//         list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
//         list_btn->text("War reparations");
//         list_btn->on_click = [](UI::Widget&, void* data) {
//             std::lock_guard<std::mutex> lock(g_treaty_draft_mutex);
//             TreatyClause::WarReparations* clause = new TreatyClause::WarReparations();
//             clause->type = TREATY_CLAUSE_WAR_REPARATIONS;
//             // TODO FIX
//             // clause->sender = curr_nation;
//             clause->receiver = recv_nation;
//             clause->amount = 5000.f;
//             clause->days_duration = 365;
//             g_treaty_draft.clauses.push_back(clause);
//         };
//         y += 38 + 2;

//         list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
//         list_btn->text("Humiliate");
//         list_btn->on_click = [](UI::Widget&, void* data) {
            
//         };
//         y += 38 + 2;

//         list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
//         list_btn->text("Liberate nation");
//         list_btn->on_click = [](UI::Widget&, void* data) {
            
//         };
//         y += 38 + 2;

//         list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
//         list_btn->text("Impose policies");
//         list_btn->on_click = [](UI::Widget&, void* data) {
            
//         };
//         y += 38 + 2;

//         list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
//         list_btn->text("Annex provinces");
//         list_btn->on_click = [](UI::Widget& w, void* data) {
//             if(recv_nation->owned_provinces.empty())
//                 return;
            
//             UI::Window* per_clause_win = new UI::Window(0, 0, 320, 425);
//             per_clause_win->text("Add new clause");

//             int another_y = 0;
//             for(const auto& province: recv_nation->owned_provinces) {
//                 if(province == nullptr)
//                     continue;
                
//                 UI::Button* list_btn = new UI::Button(9, another_y, 303, 38, per_clause_win);
//                 list_btn->text(province->name.c_str());
//                 list_btn->user_data = (void*)province;
//                 list_btn->on_click = [](UI::Widget& w, void* data) {
//                     std::lock_guard<std::mutex> lock(g_treaty_draft_mutex);

//                     TreatyClause::AnexxProvince* clause = new TreatyClause::AnexxProvince();
//                     clause->type = TREATY_CLAUSE_ANEXX_PROVINCES;
//                     clause->provinces.push_back((Province*)w.user_data);
//                     // TODO FIX
//                     // clause->sender = curr_nation;
//                     clause->receiver = recv_nation;
//                     clause->days_duration = 0;
//                     g_treaty_draft.clauses.push_back(clause);
//                 };
//                 another_y += 38 + 4;
//             }
//         };
//         y += 38 + 2;

//         list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
//         list_btn->text("Ceasefire");
//         list_btn->on_click = [](UI::Widget&, void* data) {
            
//         };
//         y += 38 + 2;

//         UI::CloseButton* ok_btn = new UI::CloseButton(9, y, 303, 38, new_clause_win);
//         ok_btn->text("OK");
//     };
    
//     UI::Button* reset_btn = new UI::Button(9, 0, 303, 38, treaty_win);
//     reset_btn->text("Reset");
//     reset_btn->below_of(*new_clause);
//     reset_btn->on_click = [](UI::Widget&, void* data) {
//         g_treaty_draft.clauses.clear();
//     };
    
//     UI::Button* ok_btn = new UI::Button(9, 0, 303, 38, treaty_win);
//     ok_btn->text("OK");
//     ok_btn->below_of(*reset_btn);
//     ok_btn->on_click = [](UI::Widget&, void* data) {
//         // Name the treaty somethign random
//         // TODO: Let the user rename treaties and name treaties after cities
//         g_treaty_draft.name = "Treaty of ";
//         // TODO FIX
//         // g_treaty_draft.name += curr_nation->name.c_str();

//         // Send draft to server
//         // TODO FIX
//         // g_client->packet_mutex.lock();
//         // Packet packet = Packet(g_client->get_fd());
//         // Archive ar = Archive();
//         // enum ActionType action = ACTION_DRAFT_TREATY;
//         // ::serialize(ar, &action);
//         // ::serialize(ar, &g_treaty_draft.clauses); // ClausesRefList
//         // ::serialize(ar, &g_treaty_draft.name); // StringObj
//         // ::serialize(ar, &curr_nation); // Sender - NationRef
//         // packet.data(ar.get_buffer(), ar.size());
//         // g_client->packet_queue.push_back(packet);
//         // g_client->packet_mutex.unlock();
        
//         // Clear everything
//         g_treaty_draft.clauses.clear();
//     };
    
//     UI::Label* treaty_desc_lab = new UI::Label(9, 0, "...", treaty_win);
//     treaty_desc_lab->below_of(*ok_btn);
//     treaty_desc_lab->on_update = [](UI::Widget& w, void*) {
//         w.text(treaty_to_text(g_treaty_draft).c_str());
//     };
// }
