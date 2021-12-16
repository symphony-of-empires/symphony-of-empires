#include "client/interface/nation_view.hpp"
#include "client/client_network.hpp"
#include "client/interface/common.hpp"
#include "io_impl.hpp"

using namespace Interface;

NationMarketView::NationMarketView(GameState& _gs, Nation* _nation)
    : gs{ _gs },
    nation{ _nation },
    UI::Window(0, 0, 700, 600)
{
    //this->is_scroll = false;
    this->text("Market information");

    unsigned int i = 0;
    for(const auto& product : gs.world->products) {
        if(product->building == nullptr || product->building->get_owner() != nation) continue;
        ProductInfo* info = new ProductInfo(gs, 0, (i * 24) + 128, product, this);
        i++;
    }

    auto* close_btn = new UI::CloseButton(0, 24, this->width, 24, this);
    close_btn->text("OK");
}

NationView::NationView(GameState& _gs, Nation* _nation)
    : gs{ _gs },
    nation{ _nation },
    UI::Window(0, 0, 256, 512)
{
    this->is_scroll = false;
    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(w);
        if(o.gs.world->time % o.gs.world->ticks_per_month) return;

        w.text(o.nation->get_client_hint().alt_name);
    });

    this->flag_img = new UI::Image(0, 24, 128, 96, nullptr, this);
    this->flag_img->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) return;

        w.current_texture = &o.gs.get_nation_flag(*o.nation);
    });
    flag_img->tooltip = new UI::Tooltip(flag_img, 512, 24);
    flag_img->tooltip->text("The flag which represents the country");

    this->name_lab = new UI::Label(0, 0, "?", this);
    this->name_lab->below_of(*this->flag_img);
    this->name_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) return;

        w.text(o.nation->get_client_hint().alt_name);
    });
    name_lab->tooltip = new UI::Tooltip(name_lab, 512, 24);
    name_lab->tooltip->text("The official name");

    this->ideology_lab = new UI::Label(0, 0, "?", this);
    this->ideology_lab->below_of(*this->name_lab);
    this->ideology_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) return;

        w.text(o.nation->get_client_hint().ideology->name);
    });
    ideology_lab->tooltip = new UI::Tooltip(ideology_lab, 512, 24);
    ideology_lab->tooltip->text("The ideology according to their policies");

    if(gs.curr_nation != nation) {
        this->rel_lab = new UI::Label(0, 0, "?", this);
        this->rel_lab->below_of(*this->ideology_lab);
        this->rel_lab->on_each_tick = ([](UI::Widget& w, void*) {
            auto& o = static_cast<NationView&>(*w.parent);
            if(o.gs.world->time % o.gs.world->ticks_per_month) return;

            w.text(std::to_string(o.gs.curr_nation->relations[o.gs.world->get_id(o.nation)].relation));
        });
        rel_lab->tooltip = new UI::Tooltip(rel_lab, 512, 24);
        rel_lab->tooltip->text("Our diplomatic relations with them");

        this->interest_lab = new UI::Label(0, 0, "?", this);
        this->interest_lab->below_of(*this->rel_lab);
        this->interest_lab->on_each_tick = ([](UI::Widget& w, void*) {
            auto& o = static_cast<NationView&>(*w.parent);
            if(o.gs.world->time % o.gs.world->ticks_per_month) return;

            w.text(std::to_string(o.gs.curr_nation->relations[o.gs.world->get_id(o.nation)].interest));
        });
        interest_lab->tooltip = new UI::Tooltip(interest_lab, 512, 24);
        interest_lab->tooltip->text("Interest/Tolerance towards them");
    }

    auto* market_btn = new UI::Button(0, 0, this->width, 24, this);
    if(gs.curr_nation != nation) {
        market_btn->below_of(*this->interest_lab);
    } else {
        market_btn->below_of(*this->ideology_lab);
    }
    market_btn->text("Examine market");
    market_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        new NationMarketView(o.gs, o.nation);
    });
    market_btn->tooltip = new UI::Tooltip(market_btn, 512, 24);
    market_btn->tooltip->text("View market information");

    this->inc_btn = new UI::Button(0, 0, this->width, 24, this);
    this->inc_btn->below_of(*market_btn);
    this->inc_btn->text("Increment relations");
    this->inc_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
		
        Packet packet = Packet();
        Archive ar = Archive();
        ActionType action = ActionType::DIPLO_INC_RELATIONS;
        ::serialize(ar, &action);
        ::serialize(ar, &o.gs.curr_nation); // Sender
        ::serialize(ar, &o.nation); // Target
        packet.data(ar.get_buffer(), ar.size());
        std::scoped_lock lock(g_client->pending_packets_mutex);
        g_client->pending_packets.push_back(packet);
    });

    this->dec_btn = new UI::Button(0, 0, this->width, 24, this);
    this->dec_btn->below_of(*this->inc_btn);
    this->dec_btn->text("Decrement relations");
    this->dec_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
		
        Packet packet = Packet();
        Archive ar = Archive();
        ActionType action = ActionType::DIPLO_DEC_RELATIONS;
        ::serialize(ar, &action);
        ::serialize(ar, &o.gs.curr_nation); // Sender
        ::serialize(ar, &o.nation); // Target
        packet.data(ar.get_buffer(), ar.size());
        std::scoped_lock lock(g_client->pending_packets_mutex);
        g_client->pending_packets.push_back(packet);
    });

    this->dow_btn = new UI::Button(0, 0, this->width, 24, this);
    this->dow_btn->below_of(*this->dec_btn);
    this->dow_btn->text("Declare war");
    this->dow_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
		
        Packet packet = Packet();
        Archive ar = Archive();
        ActionType action = ActionType::DIPLO_DECLARE_WAR;
        ::serialize(ar, &action);
        ::serialize(ar, &o.gs.curr_nation); // Sender
        ::serialize(ar, &o.nation); // Target
        packet.data(ar.get_buffer(), ar.size());
		std::scoped_lock lock(g_client->pending_packets_mutex);
        g_client->pending_packets.push_back(packet);
    });

    this->ally_btn = new UI::Button(0, 0, this->width, 24, this);
    this->ally_btn->below_of(*this->dow_btn);
    this->ally_btn->text("Offer alliance");

    this->defensive_pact_btn = new UI::Button(0, 0, this->width, 24, this);
    this->defensive_pact_btn->below_of(*this->ally_btn);
    this->defensive_pact_btn->text("Defensive pact");

    this->embargo_btn = new UI::Button(0, 0, this->width, 24, this);
    this->embargo_btn->below_of(*this->defensive_pact_btn);
    this->embargo_btn->text("Embargo");

    this->allow_market_access_btn = new UI::Button(0, 0, this->width, 24, this);
    this->allow_market_access_btn->below_of(*this->embargo_btn);
    this->allow_market_access_btn->text("Allow market access");

    this->allow_military_access_btn = new UI::Button(0, 0, this->width, 24, this);
    this->allow_military_access_btn->below_of(*this->allow_market_access_btn);
    this->allow_military_access_btn->text("Allow military access");

    this->propose_truce_btn = new UI::Button(0, 0, this->width, 24, this);
    this->propose_truce_btn->below_of(*this->allow_military_access_btn);
    this->propose_truce_btn->text("Propose truce");

    auto* close_btn = new UI::CloseButton(0, 0, this->width, 24, this);
    close_btn->below_of(*this->propose_truce_btn);
    close_btn->text("OK");
}