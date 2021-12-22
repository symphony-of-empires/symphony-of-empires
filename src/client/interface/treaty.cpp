#include "client/interface/treaty.hpp"
#include "io_impl.hpp"

using namespace Interface;

TreatyDraftView::TreatyDraftView(GameState& _gs)
    : gs{ _gs },
    UI::Window(0, 0, 256, 512)
{
    this->is_scroll = false;
    this->text("Draft treaty");
    
    auto* draft_btn = new UI::Button(0, 24 * 2, this->width, 24, this);
    draft_btn->text("Draft");
}

TreatyChooseWindow::TreatyChooseWindow(GameState& _gs, Treaty* _treaty)
    : gs{ _gs },
    treaty{ _treaty },
    UI::Window(0, 0, 256, 512)
{
    this->is_scroll = false;
    this->text("Treaty proposal");
    
    this->body_txt = new UI::Text(0, 0, this->width, 24, this);
    this->body_txt->text("[insert the treaty description here]");

    auto* approve_btn = new UI::Button(0, 24 * 2, this->width, 24, this);
    approve_btn->text("Approve");
    approve_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TreatyChooseWindow&>(*w.parent);

        Archive ar = Archive();
        ActionType action = ActionType::CHANGE_TREATY_APPROVAL;
        ::serialize(ar, &action);
        ::serialize(ar, &o.treaty);

        TreatyApproval approval = TreatyApproval::ACCEPTED;
        ::serialize(ar, &approval);
        
        o.gs.send_command(ar);

        delete w.parent;
    });

    auto* deny_btn = new UI::Button(0, 24 * 3, this->width, 24, this);
    deny_btn->text("Deny it!");
    deny_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TreatyChooseWindow&>(*w.parent);

        Archive ar = Archive();
        ActionType action = ActionType::CHANGE_TREATY_APPROVAL;
        ::serialize(ar, &action);
        ::serialize(ar, &o.treaty);

        TreatyApproval approval = TreatyApproval::DENIED;
        ::serialize(ar, &approval);

        o.gs.send_command(ar);

        delete w.parent;
    });
}