#include "client/interface/policies.hpp"
#include "client/client_network.hpp"
#include "unified_render/serializer.hpp"
#include "io_impl.hpp"

using namespace Interface;

#define POLICY_CHECKBOX(x, title, body)\
    auto* x## _chk = new UI::Checkbox(0, 0, 128, 24, this);\
    x## _chk->text(title);\
    x## _chk->on_click = ([](UI::Widget& w, void*) {\
        auto& o = static_cast<PoliciesScreen&>(*w.parent);\
        o.new_policy.x = !(o.new_policy).x;\
        ((UI::Checkbox&)w).value = (o.new_policy).x;\
    });\
    x## _chk->value = (new_policy).x;\
    x## _chk->tooltip = new UI::Tooltip(x## _chk, 512, 24);\
    x## _chk->tooltip->text(body);

PoliciesScreen::PoliciesScreen(GameState& _gs)
    : UI::Window(0, 0, 512, 400, nullptr),
    gs{ _gs }
{
    this->new_policy = gs.curr_nation->current_policy;

    this->ideology_pie = new UI::PieChart(0, 0, 128, 128, this);
    auto* ideology_pie_lab = new UI::Label(0, 0, "Ideologies", this);

    this->militancy_chart = new UI::Chart(0, 0, 128, 128, this);
    this->militancy_chart->text("Militancy");
    this->militancy_chart->right_side_of(*ideology_pie);

    this->con_chart = new UI::Chart(0, 0, 128, 128, this);
    this->con_chart->text("Conciousness");
    this->con_chart->right_side_of(*militancy_chart);

    // Social
    POLICY_CHECKBOX(slavery, "Slavery", "Allows to put the burden of work to slaves for free");
    slavery_chk->below_of(*con_chart);

    POLICY_CHECKBOX(secular_education, "Secular education", "Educates people in a way that it's bound to religion");
    secular_education_chk->below_of(*con_chart);
    secular_education_chk->right_side_of(*slavery_chk);

    POLICY_CHECKBOX(private_property, "Private property", "Allow the right to own property");
    private_property_chk->below_of(*con_chart);
    private_property_chk->right_side_of(*secular_education_chk);

    POLICY_CHECKBOX(public_education, "Public education", "(paid by the goverment)");
    public_education_chk->below_of(*private_property_chk);

    POLICY_CHECKBOX(public_healthcare, "Public healthcare", "(paid by the goverment)");
    public_healthcare_chk->below_of(*private_property_chk);
    public_healthcare_chk->right_side_of(*public_education_chk);

    POLICY_CHECKBOX(social_security, "Social security", "Help people in need with goverment support");
    social_security_chk->below_of(*private_property_chk);
    social_security_chk->right_side_of(*public_healthcare_chk);

    auto* poor_tax_sld = new UI::Slider(0, 0, 512, 24, -10.f, 100.f, this);
    poor_tax_sld->text(std::to_string(new_policy.poor_flat_tax));
    poor_tax_sld->value = new_policy.poor_flat_tax;
    poor_tax_sld->below_of(*social_security_chk);
    poor_tax_sld->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(*w.parent);
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
        auto& o = static_cast<PoliciesScreen&>(*w.parent);
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
        auto& o = static_cast<PoliciesScreen&>(*w.parent);
        o.new_policy.rich_flat_tax = ((UI::Slider&)w).value;
        w.text(std::to_string(o.new_policy.rich_flat_tax));
    });
    rich_tax_sld->tooltip = new UI::Tooltip(rich_tax_sld, 512, 24);
    rich_tax_sld->tooltip->text("The taxing done to the high class (flat %)");

    // Goverment structure
    POLICY_CHECKBOX(legislative_parliament, "Legislative parliament", "The legislative parliament allows to have a democratic vote on laws");
    legislative_parliament_chk->below_of(*rich_tax_sld);

    POLICY_CHECKBOX(executive_parliament, "Executive parliament", "The executive parliament will approve/disapprove acts of diplomacy and war");
    executive_parliament_chk->below_of(*rich_tax_sld);
    executive_parliament_chk->right_side_of(*legislative_parliament_chk);

    POLICY_CHECKBOX(constitutional, "Constitutional", "The constitution will limit the governing power of the goverment");
    constitutional_chk->below_of(*rich_tax_sld);
    constitutional_chk->right_side_of(*executive_parliament_chk);

    // Foreing policies
    POLICY_CHECKBOX(foreign_trade, "Foreign Trade", "Controls foreing trade with other countries");
    foreign_trade_chk->below_of(*constitutional_chk);

    auto* enact_btn = new UI::Button(0, 0, 128, 24, this);
    enact_btn->text("Enact policy");
    enact_btn->below_of(*foreign_trade_chk);
    enact_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(*w.parent);
        Packet packet = Packet();
        Archive ar = Archive();
        ActionType action = ActionType::NATION_ENACT_POLICY;
        ::serialize(ar, &action);
        ::serialize(ar, &o.new_policy); // PoliciesObj
        packet.data(ar.get_buffer(), ar.size());
        std::scoped_lock lock(g_client->pending_packets_mutex);
        g_client->pending_packets.push_back(packet);

        o.gs.ui_ctx->prompt("Policy", "New policy enacted!");
    });

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->text("Close");
    close_btn->below_of(*foreign_trade_chk);
    close_btn->right_side_of(*enact_btn);

    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PoliciesScreen&>(w);
        const World& world = *(o.gs.world);

        std::vector<UI::ChartData> ideology_data;
        for(const auto& ideology : world.ideologies) {
            ideology_data.push_back(UI::ChartData(1.f, ideology->name, ideology->color));
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
