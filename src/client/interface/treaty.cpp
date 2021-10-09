#include "treaty.hpp"

#include "../../io_impl.hpp"

using namespace Interface;

TreatyChooseWindow::TreatyChooseWindow(GameState& _gs, Treaty* _treaty)
    : gs{ _gs },
    treaty{ _treaty },
    UI::Window(0, 0, 256, 512)
{
    this->body_txt = new UI::Text(0, 24, this->width, 24, this);
    this->body_txt->text("[insert the treaty description here]");

    this->approve_btn = new UI::Button(0, 24 * 2, this->width, 24, this);
    this->approve_btn->text("Approve");
    this->approve_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = dynamic_cast<TreatyChooseWindow&>(*w.parent);

        Archive ar = Archive();
        ActionType action = ActionType::CHANGE_TREATY_APPROVAL;
        ::serialize(ar, &action);
        ::serialize(ar, &o.treaty);

        TreatyApproval approval = TreatyApproval::ACCEPTED;
        ::serialize(ar, &approval);
        
        o.gs.send_command(ar);

        delete w.parent;
    });

    this->deny_btn = new UI::Button(0, 24 * 3, this->width, 24, this);
    this->deny_btn->text("Deny it!");
    this->deny_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = dynamic_cast<TreatyChooseWindow&>(*w.parent);

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