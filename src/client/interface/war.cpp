#include "client/interface/war.hpp"
#include "client/client_network.hpp"
#include "io_impl.hpp"
#include "action.hpp"

using namespace Interface;

WarDeclarePrompt::WarDeclarePrompt(GameState& _gs, Nation* _nation)
    : UI::Window(0, 0, 256, 512),
    gs{ _gs },
    nation{ _nation }
{
    this->is_scroll = false;

    this->body_txt = new UI::Text(0, 0, this->width, 24, this);
    this->body_txt->text("TODO: add cassus bellis");

    this->approve_btn = new UI::Button(0, 24, this->width, 24, this);
    this->approve_btn->text("Declare war");
    this->approve_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<WarDeclarePrompt&>(*w.parent);
		o.gs.client->send(Action::DiploDeclareWar::form_packet(o.nation));
        w.parent->kill();
    });

    this->deny_btn = new UI::Button(0, 48, this->width, 24, this);
    this->deny_btn->text("Nevermind");
    this->deny_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<WarDeclarePrompt&>(*w.parent);
        w.parent->kill();
    });
}