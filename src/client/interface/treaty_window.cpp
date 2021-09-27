#include "treaty_window.hpp"

#include "../../diplomacy.hpp"
#include "../command.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

TreatySelectNation::TreatySelectNation(TreatyWindow* _treaty_win) : treaty_win{_treaty_win}, UI::Window(0, 0, 320, 425) {
    text("Select nation");

    int y = 0;
    for (const auto& nation : treaty_win->gs.world->nations) {
        UI::Button* select_receiver_btn = new UI::Button(9, y, 303, 38, this);
        select_receiver_btn->text(nation->name.c_str());
        select_receiver_btn->user_data = (void*)nation;
        select_receiver_btn->on_click = [](UI::Widget& w, void* data) {
            TreatySelectNation* state = (TreatySelectNation*)w.parent;
            state->treaty_win->recv_nation = (Nation*)data;
            state->treaty_win->select_win = nullptr;
            state->kill();
        };
        y += 38 + 2;
    }
}

TreatyPerClauseWindow::TreatyPerClauseWindow(TreatyWindow* treaty_window) : treaty_win{treaty_window}, UI::Window(0, 0, 320, 425) {
    text("Add new clause");

    int another_y = 0;
    for (const auto& province : treaty_win->recv_nation->owned_provinces) {
        if (province == nullptr)
            continue;

        UI::Button* list_btn = new UI::Button(9, another_y, 303, 38, this);
        list_btn->text(province->name.c_str());
        list_btn->user_data = (void*)province;
        list_btn->on_click = [](UI::Widget& w, void* data) {
            TreatyPerClauseWindow* state = (TreatyPerClauseWindow*)w.parent;
            // std::lock_guard<std::mutex> lock(g_treaty_draft_mutex);

            TreatyClause::AnexxProvince* clause = new TreatyClause::AnexxProvince();
            clause->type = TreatyClauseType::ANEXX_PROVINCES;
            clause->provinces.push_back((Province*)w.user_data);
            // TODO FIX
            // clause->sender = curr_nation;
            clause->receiver = state->treaty_win->recv_nation;
            clause->days_duration = 0;
            state->treaty_win->treaty_draft.clauses.push_back(clause);
        };
        another_y += 38 + 4;
    }
}

TreatyClauseWindow::TreatyClauseWindow(TreatyWindow* _treaty_win) : treaty_win{_treaty_win}, UI::Window(0, 0, 320, 425) {
    text("Add new clause");

    UI::Button* list_btn;
    int y = 0;

    y += 24;
    list_btn = new UI::Button(9, y, 303, 38, this);
    list_btn->text("War reparations");
    list_btn->on_click = [](UI::Widget& w, void* data) {
        TreatyClauseWindow* state = (TreatyClauseWindow*)w.parent;
        // std::lock_guard<std::mutex> lock(state->treaty_win->treaty_draft_mutex);
        TreatyClause::WarReparations* clause = new TreatyClause::WarReparations();
        clause->type = TreatyClauseType::WAR_REPARATIONS;
        // TODO FIX
        // clause->sender = curr_nation;
        clause->receiver = state->treaty_win->recv_nation;
        clause->amount = 5000.f;
        clause->days_duration = 365;
        state->treaty_win->treaty_draft.clauses.push_back(clause);
    };
    y += 38 + 2;

    list_btn = new UI::Button(9, y, 303, 38, this);
    list_btn->text("Humiliate");
    list_btn->on_click = [](UI::Widget&, void* data) {

    };
    y += 38 + 2;

    list_btn = new UI::Button(9, y, 303, 38, this);
    list_btn->text("Liberate nation");
    list_btn->on_click = [](UI::Widget&, void* data) {

    };
    y += 38 + 2;

    list_btn = new UI::Button(9, y, 303, 38, this);
    list_btn->text("Impose policies");
    list_btn->on_click = [](UI::Widget&, void* data) {

    };
    y += 38 + 2;

    list_btn = new UI::Button(9, y, 303, 38, this);
    list_btn->text("Annex provinces");
    list_btn->on_click = [](UI::Widget& w, void* data) {
        TreatyClauseWindow* state = (TreatyClauseWindow*)w.parent;
        if (state->treaty_win->recv_nation->owned_provinces.empty())
            return;

        new TreatyPerClauseWindow(state->treaty_win);
    };
    y += 38 + 2;

    list_btn = new UI::Button(9, y, 303, 38, this);
    list_btn->text("Ceasefire");
    list_btn->on_click = [](UI::Widget&, void* data) {

    };
    y += 38 + 2;

    UI::CloseButton* ok_btn = new UI::CloseButton(9, y, 303, 38, this);
    ok_btn->text("OK");
}

TreatyWindow::TreatyWindow(GameState& _gs, UI::Window* top_win) : gs{_gs}, UI::Window(0, 0, 320, 425) {
    treaty_draft.clauses.clear();

    text("Draft Treaty");
    below_of((*top_win));

    UI::Button* select_receiver_btn = new UI::Button(9, 43, 303, 38, this);
    select_receiver_btn->text("Select nation");
    select_receiver_btn->on_click = [](UI::Widget& w, void*) {
        TreatyWindow* state = (TreatyWindow*)w.parent;
        state->select_win = new TreatySelectNation(state);
    };

    recv_nation_lab = new UI::Label(64, 44, "...", this);
    recv_nation_lab->below_of((*select_receiver_btn));
    recv_nation_lab->on_update = [](UI::Widget& w, void*) {
        TreatyWindow* state = (TreatyWindow*)w.parent;
        if (state->recv_nation == nullptr)
            return;
        w.text(state->recv_nation->name.c_str());
    };

    recv_nation_flag = new UI::Image(0, recv_nation_lab->y, 58, 24, nullptr, this);
    recv_nation_flag->on_update = [](UI::Widget& w, void*) {
        TreatyWindow* state = (TreatyWindow*)w.parent;
        if (state->recv_nation == nullptr)
            return;
        w.current_texture = &state->gs.get_nation_flag(*state->recv_nation);
    };

    UI::Button* new_clause = new UI::Button(9, 0, 303, 38, this);
    new_clause->below_of((*recv_nation_lab));
    new_clause->text("Add new clause");
    new_clause->on_click = [](UI::Widget& w, void*) {
        TreatyWindow* state = (TreatyWindow*)w.parent;
        new TreatyClauseWindow(state);
    };

    UI::Button* reset_btn = new UI::Button(9, 0, 303, 38, this);
    reset_btn->text("Reset");
    reset_btn->below_of(*new_clause);
    reset_btn->on_click = [](UI::Widget& w, void* data) {
        TreatyWindow* state = (TreatyWindow*)w.parent;
        state->treaty_draft.clauses.clear();
    };

    UI::Button* ok_btn = new UI::Button(9, 0, 303, 38, this);
    ok_btn->text("OK");
    ok_btn->below_of(*reset_btn);
    ok_btn->on_click = [](UI::Widget& w, void* data) {
        TreatyWindow* state = (TreatyWindow*)w.parent;
        // Name the treaty somethign random
        // TODO: Let the user rename treaties and name treaties after cities
        state->treaty_draft.name = "Treaty of ";
        state->treaty_draft.name += state->gs.curr_nation->name.c_str();
    
        Command* command = new TreatySendCommand(&state->treaty_draft, state->gs.curr_nation);
        state->gs.add_command(command);

        // Clear everything
        state->treaty_draft.clauses.clear();
    };

    UI::Label* treaty_desc_lab = new UI::Label(9, 0, "...", this);
    treaty_desc_lab->below_of(*ok_btn);
    treaty_desc_lab->on_update = [](UI::Widget& w, void*) {
        TreatyWindow* state = (TreatyWindow*)w.parent;
        w.text(treaty_to_text(state->treaty_draft).c_str());
    };
}

TreatyPopup::TreatyPopup(GameState& _gs, Treaty* _treaty) : gs{_gs}, treaty{_treaty}, UI::Window(128, 128, 320, 570) {
    // TODO: Allow titles in events
    text("Treaty offer");

    // Separate the text line by line
    new UI::Label(8, 32, treaty_to_text(*treaty).c_str(), this);

    UI::Button* approve_btn = new UI::Button(9, 64, 303, 38, this);
    approve_btn->text("Approve");
    approve_btn->user_data = this;
    approve_btn->on_click = [](UI::Widget& w, void* data) {
        TreatyPopup* state = (TreatyPopup*)data;

        Command* command = new TreatyAcceptCommand(state->treaty, true);
        state->gs.add_command(command);
        delete w.parent;
    };

    UI::Button* deny_btn = new UI::Button(9, 0, 303, 38, this);
    deny_btn->text("Reject");
    deny_btn->user_data = (void*)&treaty;
    deny_btn->below_of(*approve_btn);
    deny_btn->on_click = [](UI::Widget& w, void* data) {
        TreatyPopup* state = (TreatyPopup*)data;

        Command* command = new TreatyAcceptCommand(state->treaty, false);
        state->gs.add_command(command);
        delete w.parent;
    };
}

std::string treaty_to_text(const Treaty& treaty) {
    std::string str;

    str = "";
    str += "Treaty";
    str += treaty.name.c_str();
    for (const auto& clause : treaty.clauses) {
        switch (clause->type) {
            case TreatyClauseType::WAR_REPARATIONS:
                str += "war reparations from ";
                str += clause->receiver->name.c_str();
                break;
            case TreatyClauseType::HUMILIATE:
                str += "humiliate ";
                str += clause->receiver->name.c_str();
                break;
            case TreatyClauseType::LIBERATE_NATION:
                str += "liberate ";
                str += ((TreatyClause::LiberateNation*)clause)->liberated->name.c_str();
                break;
            case TreatyClauseType::IMPOSE_POLICIES:
                str += "impose policies ";
                break;
            case TreatyClauseType::ANEXX_PROVINCES:
                str += "anexx province ";
                for (const auto& province : ((TreatyClause::AnexxProvince*)clause)->provinces) {
                    str += province->name.c_str();
                    str += ", ";
                }
                str += "from ";
                str += clause->receiver->name.c_str();
                break;
            case TreatyClauseType::CEASEFIRE:
                str += "generous ceasefire to ";
                str += clause->receiver->name.c_str();
                break;
            default:
                str += "unknown";
                break;
        }
        str += ", ";
    }
    return str;
}