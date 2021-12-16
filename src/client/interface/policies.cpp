#include "client/interface/policies.hpp"
#include "client/client_network.hpp"
#include "unified_render/serializer.hpp"
#include "io_impl.hpp"

using namespace Interface;

PoliciesScreen::PoliciesScreen(GameState& _gs)
    : gs{_gs},
    UI::Window(0, 0, 512, 512, nullptr)
{
    this->new_policy = gs.curr_nation->current_policy;

    this->ideology_pie = new UI::PieChart(0, 24, 128, 128, this);
    this->ideology_pie->text("Ideologies");

    this->militancy_chart = new UI::Chart(0, 0, 128, 128, this);
    this->militancy_chart->text("Militancy");
    this->militancy_chart->right_side_of(*ideology_pie);

    this->con_chart = new UI::Chart(0, 0, 128, 128, this);
    this->con_chart->text("Conciousness");
    this->con_chart->right_side_of(*militancy_chart);

    // Social
    auto* slavery_chk = new UI::Checkbox(0, 0, 128, 24, this);
    slavery_chk->text("Slavery");
    slavery_chk->below_of(*this->con_chart);
    slavery_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
        o.new_policy.slavery = !o.new_policy.slavery;
    });
    slavery_chk->tooltip = new UI::Tooltip(slavery_chk, 512, 24);
    slavery_chk->tooltip->text("Allows to transfer the work to slaves for free");

    auto* poor_tax_sld = new UI::Slider(0, 0, 512, 24, -10.f, 100.f, this);
    poor_tax_sld->text(std::to_string(new_policy.poor_flat_tax));
    poor_tax_sld->value = new_policy.poor_flat_tax;
    poor_tax_sld->below_of(*slavery_chk);
    poor_tax_sld->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
        o.new_policy.poor_flat_tax = ((UI::Slider&)w).value;
        w.text(std::to_string(o.new_policy.poor_flat_tax));
    });
    poor_tax_sld->tooltip = new UI::Tooltip(poor_tax_sld, 512, 24);
    poor_tax_sld->tooltip->text("The taxing done to the low class (flat %)");

    auto* med_tax_sld = new UI::Slider(0, 0, 512, 24, -10.f, 100.f, this);
    med_tax_sld->text(std::to_string(new_policy.poor_flat_tax));
    med_tax_sld->value = new_policy.med_flat_tax;
    med_tax_sld->below_of(*poor_tax_sld);
    med_tax_sld->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
        o.new_policy.med_flat_tax = ((UI::Slider&)w).value;
        w.text(std::to_string(o.new_policy.med_flat_tax));
    });
    med_tax_sld->tooltip = new UI::Tooltip(med_tax_sld, 512, 24);
    med_tax_sld->tooltip->text("The taxing done to the medium class (flat %)");

    auto* rich_tax_sld = new UI::Slider(0, 0, 512, 24, -10.f, 100.f, this);
    rich_tax_sld->text(std::to_string(new_policy.poor_flat_tax));
    rich_tax_sld->value = new_policy.rich_flat_tax;
    rich_tax_sld->below_of(*med_tax_sld);
    rich_tax_sld->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
        o.new_policy.rich_flat_tax = ((UI::Slider&)w).value;
        w.text(std::to_string(o.new_policy.rich_flat_tax));
    });
    rich_tax_sld->tooltip = new UI::Tooltip(rich_tax_sld, 512, 24);
    rich_tax_sld->tooltip->text("The taxing done to the high class (flat %)");

    // Goverment structure
    auto* lpar_chk = new UI::Checkbox(0, 0, 128, 24, this);
    lpar_chk->text("Legislative parliament");
    lpar_chk->below_of(*rich_tax_sld);
    lpar_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
        o.new_policy.legislative_parliament = !o.new_policy.legislative_parliament;
    });
    lpar_chk->tooltip = new UI::Tooltip(lpar_chk, 512, 24);
    lpar_chk->tooltip->text("The legislative parliament allows to have a democratic vote on laws");

    auto* epar_chk = new UI::Checkbox(0, 0, 128, 24, this);
    epar_chk->text("Executive parliament");
    epar_chk->below_of(*rich_tax_sld);
    epar_chk->right_side_of(*lpar_chk);
    epar_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
        o.new_policy.executive_parliament = !o.new_policy.executive_parliament;
    });
    epar_chk->tooltip = new UI::Tooltip(epar_chk, 512, 24);
    epar_chk->tooltip->text("The executive parliament will approve/disapprove acts of diplomacy and war");

    auto* constitutionalism_chk = new UI::Checkbox(0, 0, 128, 24, this);
    constitutionalism_chk->text("Constitutionalism");
    constitutionalism_chk->below_of(*rich_tax_sld);
    constitutionalism_chk->right_side_of(*epar_chk);
    constitutionalism_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
        o.new_policy.constitutional = !o.new_policy.constitutional;
    });
    constitutionalism_chk->tooltip = new UI::Tooltip(constitutionalism_chk, 512, 24);
    constitutionalism_chk->tooltip->text("The constitution will limit the governing power of the goverment");

    // Foreing policies
    auto* isolationism_chk = new UI::Checkbox(0, 0, 128, 24, this);
    isolationism_chk->text("Isolationism");
    isolationism_chk->below_of(*constitutionalism_chk);
    isolationism_chk->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
        o.new_policy.foreign_trade = !o.new_policy.foreign_trade;
    });
    isolationism_chk->tooltip = new UI::Tooltip(isolationism_chk, 512, 24);
    isolationism_chk->tooltip->text("Allows us to isolate ourselves from the rest of the world");

    this->enact_btn = new UI::Button(0, 0, 128, 24, this);
    this->enact_btn->below_of(*isolationism_chk);
    this->enact_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
		
        Packet packet = Packet();
        Archive ar = Archive();
        ActionType action = ActionType::NATION_ENACT_POLICY;
        ::serialize(ar, &action);
        ::serialize(ar, &o.new_policy); // PoliciesObj
        packet.data(ar.get_buffer(), ar.size());
        std::scoped_lock lock(g_client->pending_packets_mutex);
        g_client->pending_packets.push_back(packet);
    });

    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
        const World& world = *(o.gs.world);

        std::vector<UI::ChartData> ideology_data;
        for(const auto& ideology : world.ideologies) {
            ideology_data.push_back(UI::ChartData(
                1.f,
                ideology->name,
                UI::Color(rand() % 255, rand() % 255, rand() % 255)
            ));
        }
        o.ideology_pie->set_data(ideology_data);

        float militancy = 0.f, con = 0.f;
        for(const auto& province : o.gs.curr_nation->owned_provinces) {
            for(const auto& pop : province->pops) {
                ideology_data[world.get_id(pop.get_ideology())].num += pop.size;
                militancy += pop.militancy;
                con += pop.con;
            }
        }

        o.militancy_chart->data.push_back(militancy);
        if(o.militancy_chart->data.size() >= 30)
            o.militancy_chart->data.pop_back();
        
        o.con_chart->data.push_back(con);
        if(o.con_chart->data.size() >= 30)
            o.con_chart->data.pop_back();
    });
}
