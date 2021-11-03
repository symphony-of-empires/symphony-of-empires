#include "client/interface/nation_view.hpp"
#include "io_impl.hpp"

using namespace Interface;

NationView::NationView(GameState& _gs, Nation* _nation)
    : gs{ _gs },
    nation{ _nation },
    UI::Window(0, 0, 256, 512)
{
    this->is_scroll = false;

    this->flag_img = new UI::Image(0, 24, 128, 96, nullptr, this);
    this->flag_img->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        w.current_texture = &o.gs.get_nation_flag(*o.nation);
    });

    this->name_lab = new UI::Label(0, 0, "?", this);
    this->name_lab->below_of(*this->flag_img);
    this->name_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        w.text(o.nation->get_client_hint().alt_name);
    });

    this->ideology_lab = new UI::Label(0, 0, "?", this);
    this->ideology_lab->below_of(*this->name_lab);
    this->ideology_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        w.text(o.nation->get_client_hint().ideology->name);
    });

    this->rel_lab = new UI::Label(0, 0, "?", this);
    this->rel_lab->below_of(*this->ideology_lab);
    this->rel_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        w.text(std::to_string(o.gs.curr_nation->relations[o.gs.world->get_id(o.nation)].relation));
    });

    this->interest_lab = new UI::Label(0, 0, "?", this);
    this->interest_lab->below_of(*this->rel_lab);
    this->interest_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        w.text(std::to_string(o.gs.curr_nation->relations[o.gs.world->get_id(o.nation)].interest));
    });

    this->inc_btn = new UI::Button(0, 0, 128, 24, this);
    this->inc_btn->below_of(*this->interest_lab);
    this->inc_btn->text("Increment relations");

    this->dec_btn = new UI::Button(0, 0, 128, 24, this);
    this->dec_btn->below_of(*this->inc_btn);
    this->dec_btn->text("Decrement relations");

    this->dow_btn = new UI::Button(0, 0, 128, 24, this);
    this->dow_btn->below_of(*this->dec_btn);
    this->dow_btn->text("Declare war");

    this->ally_btn = new UI::Button(0, 0, 128, 24, this);
    this->ally_btn->below_of(*this->dow_btn);
    this->ally_btn->text("Offer alliance");

    this->defensive_pact_btn = new UI::Button(0, 0, 128, 24, this);
    this->defensive_pact_btn->below_of(*this->ally_btn);
    this->defensive_pact_btn->text("Defensive pact");

    this->embargo_btn = new UI::Button(0, 0, 128, 24, this);
    this->embargo_btn->below_of(*this->defensive_pact_btn);
    this->embargo_btn->text("Embargo");

    this->allow_market_access_btn = new UI::Button(0, 0, 128, 24, this);
    this->allow_market_access_btn->below_of(*this->embargo_btn);
    this->allow_market_access_btn->text("Allow market access");

    this->allow_military_access_btn = new UI::Button(0, 0, 128, 24, this);
    this->allow_military_access_btn->below_of(*this->allow_market_access_btn);
    this->allow_military_access_btn->text("Allow military access");

    this->propose_truce_btn = new UI::Button(0, 0, 128, 24, this);
    this->propose_truce_btn->below_of(*this->allow_military_access_btn);
    this->propose_truce_btn->text("Propose truce");

    this->close_btn = new UI::CloseButton(0, 0, this->width, 24, this);
    this->close_btn->below_of(*this->propose_truce_btn);
    this->close_btn->text("Close");
}