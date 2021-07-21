#include "texture.hpp"
#include "ui.hpp"
#include "path.hpp"

extern TextureManager* g_texture_manager;
extern UI::Window* top_win;
extern std::pair<int, int> mouse_pos;

#include "nation.hpp"
extern Nation* curr_nation;

#include "serializer.hpp"
#include "io_impl.hpp"
#include "network.hpp"

UI::Label* recv_nation_lab;
UI::Image* recv_nation_flag;
UI::Window* select_win;
Nation* recv_nation = nullptr;

extern const Texture& get_nation_flag(Nation& nation);

enum TreatyClauseType g_clause_type;

#include <mutex>
Treaty g_treaty_draft;
std::mutex g_treaty_draft_mutex;

std::string treaty_to_text(Treaty& treaty) {
    std::lock_guard<std::mutex> lock(g_treaty_draft_mutex);
    std::string str;
    
    str = "";
    str += "Treaty";
    str += treaty.name.c_str();
    for(const auto& clause: treaty.clauses) {
        switch(clause.type) {
        case TREATY_CLAUSE_WAR_REPARATIONS:
            str += "war reparations from ";
            str += clause.receiver->name.c_str();
            break;
        case TREATY_CLAUSE_HUMILIATE:
            str += "humiliate ";
            str += clause.receiver->name.c_str();
            break;
        case TREATY_CLAUSE_LIBERATE_NATION:
            str += "liberate ";
            str += clause.liberated->name.c_str();
            break;
        case TREATY_CLAUSE_IMPOSE_POLICIES:
            str += "impose policies ";
            break;
        case TREATY_CLAUSE_ANEXX_PROVINCES:
            str += "anexx province ";
            for(const auto& province: clause.provinces) {
                str += province->name.c_str();
                str += ", ";
            }
            str += "from ";
            str += clause.receiver->name.c_str();
            break;
        case TREATY_CLAUSE_CEASEFIRE:
            str += "generous ceasefire to ";
            str += clause.receiver->name.c_str();
            break;
        default:
            str += "unknown";
            break;
        }
        str += ", ";
    }
    return str;
}

void ui_treaty(void) {
    g_treaty_draft.clauses.clear();

    UI::Window* treaty_win = new UI::Window(0, 0, 320, 425);
    treaty_win->text("Draft Treaty");
    treaty_win->below_of((*top_win));

    UI::Button* select_receiver_btn = new UI::Button(9, 43, 303, 38, treaty_win);
    select_receiver_btn->text("Select nation");
    select_receiver_btn->on_click = [](UI::Widget&, void*) {
        select_win = new UI::Window(0, 0, 320, 425);
        select_win->text("Select nation");

        int y = 0;
        for(const auto& nation: g_world->nations) {
            UI::Button* select_receiver_btn = new UI::Button(9, y, 303, 38, select_win);
            select_receiver_btn->text(nation->name.c_str());
            select_receiver_btn->user_data = (void*)nation;
            select_receiver_btn->on_click = [](UI::Widget&, void* data) {
                recv_nation = (Nation*)data;
                delete select_win;
            };
            y += 38 + 2;
        }
    };

    recv_nation_lab = new UI::Label(64, 44, "...", treaty_win);
    recv_nation_lab->below_of((*select_receiver_btn));
    recv_nation_lab->on_update = [](UI::Widget& w, void*) {
        if(recv_nation == nullptr)
            return;
        w.text(recv_nation->name.c_str());
    };

    recv_nation_flag = new UI::Image(0, recv_nation_lab->y, 58, 24, nullptr, treaty_win);
    recv_nation_flag->on_update = [](UI::Widget& w, void*) {
        if(recv_nation == nullptr)
            return;
        w.current_texture = &get_nation_flag(*recv_nation);
    };

    UI::Button* new_clause = new UI::Button(9, 0, 303, 38, treaty_win);
    new_clause->below_of((*recv_nation_lab));
    new_clause->text("Add new clause");
    new_clause->on_click = [](UI::Widget&, void*) {
        UI::Window* new_clause_win = new UI::Window(0, 0, 320, 425);
        new_clause_win->text("Add new clause");

        UI::Button* list_btn;
        int y = 0;
        
        y += 24;
        list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
        list_btn->text("TREATY_CLAUSE_WAR_REPARATIONS");
        list_btn->on_click = [](UI::Widget&, void* data) {
            std::lock_guard<std::mutex> lock(g_treaty_draft_mutex);
            TreatyClause::BaseClause clause;
            clause.type = TREATY_CLAUSE_WAR_REPARATIONS;
            clause.sender = curr_nation;
            clause.receiver = recv_nation;
            clause.amount = 5000.f;
            clause.days_duration = 365;
            g_treaty_draft.clauses.push_back(clause);
        };
        y += 38 + 2;

        list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
        list_btn->text("TREATY_CLAUSE_HUMILIATE");
        list_btn->on_click = [](UI::Widget&, void* data) {
            
        };
        y += 38 + 2;

        list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
        list_btn->text("TREATY_CLAUSE_LIBERATE_NATION");
        list_btn->on_click = [](UI::Widget&, void* data) {
            
        };
        y += 38 + 2;

        list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
        list_btn->text("TREATY_CLAUSE_IMPOSE_POLICIES");
        list_btn->on_click = [](UI::Widget&, void* data) {
            
        };
        y += 38 + 2;

        list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
        list_btn->text("TREATY_CLAUSE_ANEXX_PROVINCES");
        list_btn->on_click = [](UI::Widget& w, void* data) {
            if(recv_nation->owned_provinces.empty()) {
                return;
            }
            
            UI::Window* per_clause_win = new UI::Window(0, 0, 320, 425);
            per_clause_win->text("Add new clause");

            int another_y = 0;
            for(const auto& province: recv_nation->owned_provinces) {
                if(province == nullptr)
                    continue;
                
                UI::Button* list_btn = new UI::Button(9, another_y, 303, 38, per_clause_win);
                list_btn->text(province->name.c_str());
                list_btn->user_data = (void*)province;
                list_btn->on_click = [](UI::Widget& w, void* data) {
                    TreatyClause::BaseClause clause;
                    clause.type = TREATY_CLAUSE_ANEXX_PROVINCES;
                    clause.provinces.push_back((Province*)w.user_data);
                    clause.sender = curr_nation;
                    clause.receiver = recv_nation;
                    clause.days_duration = 0;
                    g_treaty_draft.clauses.push_back(clause);
                };
                another_y += 38 + 4;
            }
        };
        y += 38 + 2;

        list_btn = new UI::Button(9, y, 303, 38, new_clause_win);
        list_btn->text("TREATY_CLAUSE_CEASEFIRE");
        list_btn->on_click = [](UI::Widget&, void* data) {
            
        };
        y += 38 + 2;

        UI::CloseButton* ok_btn = new UI::CloseButton(9, y, 303, 38, new_clause_win);
        ok_btn->text("OK");
    };
    
    UI::Button* reset_btn = new UI::Button(9, 0, 303, 38, treaty_win);
    reset_btn->text("Reset");
    reset_btn->below_of(*new_clause);
    reset_btn->on_click = [](UI::Widget&, void* data) {
        g_treaty_draft.clauses.clear();
    };
    
    UI::Button* ok_btn = new UI::Button(9, 0, 303, 38, treaty_win);
    ok_btn->text("OK");
    ok_btn->below_of(*reset_btn);
    ok_btn->on_click = [](UI::Widget&, void* data) {
        // Send draft to server
        g_client->packet_mutex.lock();
        Packet packet = Packet(g_client->get_fd());
        Archive ar = Archive();
        enum ActionType action = ACTION_DRAFT_TREATY;
        ::serialize(ar, &action);
        ::serialize(ar, &g_treaty_draft.clauses);
        ::serialize(ar, &g_treaty_draft.name);
        ::serialize(ar, &g_treaty_draft.receiver);
        ::serialize(ar, &g_treaty_draft.sender);
        packet.data(ar.get_buffer(), ar.size());
        g_client->packet_queue.push_back(packet);
        g_client->packet_mutex.unlock();
        
        // Clear everything
        g_treaty_draft.clauses.clear();
    };
    
    UI::Label* treaty_desc_lab = new UI::Label(9, 0, "...", treaty_win);
    treaty_desc_lab->below_of(*ok_btn);
    treaty_desc_lab->on_update = [](UI::Widget& w, void* data) {
        w.text(treaty_to_text(g_treaty_draft).c_str());
    };
}
