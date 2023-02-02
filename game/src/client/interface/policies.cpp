// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      client/interface/policies.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/serializer.hpp"
#include "eng3d/ui/components.hpp"
#include "eng3d/ui/table.hpp"
#include "eng3d/texture.hpp"
#include "client/interface/policies.hpp"
#include "client/client_network.hpp"
#include "action.hpp"

using namespace Interface;

PoliciesView::PoliciesView(GameState& _gs)
    : UI::Window(0, 0, 512, 400, nullptr),
    gs{ _gs }
{
    this->padding.x = 0;
    this->padding.y = 48;

    this->current_texture = this->gs.tex_man.load(this->gs.package_man.get_unique("gfx/policies_screen.png"));
    this->width = this->current_texture->width;
    this->height = this->current_texture->height;
    this->set_text("Laws and goverment");
    this->is_scroll = false;
    this->new_policy = gs.curr_nation->current_policy;
    this->set_close_btn_function([this](Widget&) {
        this->kill();
    });

    /// @todo More dynamic names
    auto& ideology_lab = this->make_widget<UI::Label>(0, 0, "");
    ideology_lab.set_on_each_tick([this](UI::Widget& w) {
        const auto& ideology = this->gs.world->ideologies[this->gs.curr_nation->ideology_id];
        const auto& subideology = ideology.subideologies[this->gs.curr_nation->subideology_id];
        w.set_text(translate_format("%s (%s)", subideology.name.data(), ideology.name.data()));
        w.set_tooltip(translate_format(
            "Distributism %.2f\nMercantilist %.2f\nCapitalism %.2f\nIndividualism %.2f\nState power %.2f\nEqualitarianism %.2f\nSecular %.2f\nPluralism %.2f\n", subideology.economic.distributism, subideology.economic.mercantilist, subideology.economic.capitalism, subideology.political.individualism, subideology.political.state_power, subideology.political.equalitarianism, subideology.political.secular, subideology.political.pluralism));
    });
    ideology_lab.on_each_tick(ideology_lab);

    auto& ideology_img = this->make_widget<UI::Image>(6, 38, 32, 32);
    ideology_img.set_on_each_tick([this](UI::Widget& w) {
        const auto& ideology = this->gs.world->ideologies[this->gs.curr_nation->ideology_id];
        w.current_texture = this->gs.tex_man.load(this->gs.package_man.get_unique(ideology.get_icon_path()));
    });
    ideology_img.on_each_tick(ideology_img);

    auto& ideology_pie_lab = this->make_widget<UI::Label>(0, 82, "House");
    auto& ideology_pie = this->make_widget<UI::PieChart>(0, 0, 128, 128);
    ideology_pie.below_of(ideology_pie_lab);
    ideology_pie.set_on_each_tick([this](UI::Widget& w) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;

        std::vector<UI::ChartData> ideology_data;
        for(const auto& ideology : this->gs.world->ideologies)
            ideology_data.emplace_back(1.f, ideology.name, ideology.color);
        
        /*
        for(const auto province_id : this->gs.curr_nation->owned_provinces)
            for(const auto& pop : this->gs.world->provinces[province_id].pops)
                for(const auto& ideology : this->gs.world->ideologies)
                    ideology_data[ideology].num += pop.ideology_approval[ideology] * pop.size;
        */
        static_cast<UI::PieChart&>(w).set_data(ideology_data);
    });
    ideology_pie.on_each_tick(ideology_pie);

    auto& militancy_lab = this->make_widget<UI::Label>(0, 290, " ");
    militancy_lab.set_on_each_tick([this](UI::Widget& w) {
        if(this->gs.world->time % this->gs.world->ticks_per_month) return;

        auto num = 0.f;
        for(const auto province_id : this->gs.curr_nation->owned_provinces)
            for(const auto& pop : this->gs.world->provinces[province_id].pops)
                num += pop.militancy;
        num /= this->gs.curr_nation->owned_provinces.size();
        w.set_text(translate_format("Militancy: %.2f%%", num * 100.f));
    });
    militancy_lab.on_each_tick(militancy_lab);

    //
    // Taxes
    //
    auto& tax_grp = this->make_widget<UI::Group>(207, 38, 196, this->height - (this->padding.y + 38 + 48));
    tax_grp.flex = UI::Flex::COLUMN;
    tax_grp.is_scroll = true;

    tax_grp.make_widget<UI::Label>(0, 0, "Burgeoise income tax");
    auto& burgeoise_tax_sld = tax_grp.make_widget<UI::Slider>(0, 0, 128, 24, -1.f, 1.f);
    burgeoise_tax_sld.set_value(this->new_policy.burgeoise_tax);
    burgeoise_tax_sld.set_on_click([this](UI::Widget& w) {
        this->new_policy.burgeoise_tax = static_cast<UI::Slider&>(w).get_value();
        w.set_text(string_format("%.2f%%", this->new_policy.burgeoise_tax * 100.f));
    });
    burgeoise_tax_sld.set_tooltip("% of tax taken from the income of the higher class population");
    burgeoise_tax_sld.on_click(burgeoise_tax_sld);

    tax_grp.make_widget<UI::Label>(0, 0, "Population income tax");
    auto& pop_tax_sld = tax_grp.make_widget<UI::Slider>(0, 0, 128, 24, -1.f, 1.f);
    pop_tax_sld.set_value(this->new_policy.pop_tax);
    pop_tax_sld.set_on_click([this](UI::Widget& w) {
        this->new_policy.pop_tax = static_cast<UI::Slider&>(w).get_value();
        w.set_text(string_format("%.2f%%", this->new_policy.pop_tax * 100.f));
    });
    pop_tax_sld.set_tooltip("% of tax taken from the income of the lower and middle class population");
    pop_tax_sld.on_click(pop_tax_sld);

    tax_grp.make_widget<UI::Label>(0, 0, "Minimum wage");
    auto& min_wage_sld = tax_grp.make_widget<UI::Slider>(0, 0, 128, 24, -1.f, 100.f);
    min_wage_sld.set_value(this->new_policy.min_wage);
    min_wage_sld.set_on_click([this](UI::Widget& w) {
        this->new_policy.min_wage = static_cast<UI::Slider&>(w).get_value();
        w.set_text(string_format("%.2f", this->new_policy.min_wage));
    });
    min_wage_sld.set_tooltip("Minimum wage (fixed) that factories are required to pay to workers");
    min_wage_sld.on_click(min_wage_sld);
    // TODO: Allow minimum wage to be a relative amount to the required price for buying
    // and satisfying all life needs

    tax_grp.make_widget<UI::Label>(0, 0, "Industry profit tax");
    auto& industry_profit_tax_sld = tax_grp.make_widget<UI::Slider>(0, 0, 128, 24, -1.f, 1.f);
    industry_profit_tax_sld.set_value(this->new_policy.industry_profit_tax);
    industry_profit_tax_sld.set_on_click([this](UI::Widget& w) {
        this->new_policy.industry_profit_tax = static_cast<UI::Slider&>(w).get_value();
        w.set_text(string_format("%.2f%%", this->new_policy.industry_profit_tax * 100.f));
    });
    industry_profit_tax_sld.set_tooltip("% of profits taken by the goverment from the industry's profits");
    industry_profit_tax_sld.on_click(industry_profit_tax_sld);

    //
    // Investments
    //
    auto& investment_grp = this->make_widget<UI::Group>(207 + 196, 38, 196, this->height - (this->padding.y + 38 + 48));
    investment_grp.flex = UI::Flex::COLUMN;
    investment_grp.is_scroll = true;

    investment_grp.make_widget<UI::Label>(0, 0, "Industry subsidies");
    auto& industry_subsidies_budget_tax_sld = investment_grp.make_widget<UI::Slider>(0, 0, 128, 24, -1.f, 1.f);
    industry_subsidies_budget_tax_sld.set_value(this->new_policy.industry_subsidies_budget);
    industry_subsidies_budget_tax_sld.set_on_click([this](UI::Widget& w) {
        this->new_policy.industry_subsidies_budget = static_cast<UI::Slider&>(w).get_value();
        w.set_text(string_format("%.2f%%", this->new_policy.industry_subsidies_budget * 100.f));
    });
    industry_subsidies_budget_tax_sld.set_tooltip("% of the national budget dedicated to subsidizing factories");
    industry_subsidies_budget_tax_sld.on_click(industry_subsidies_budget_tax_sld);

    investment_grp.make_widget<UI::Label>(0, 0, "Education budget");
    auto& education_budget_tax_sld = investment_grp.make_widget<UI::Slider>(0, 0, 128, 24, -1.f, 1.f);
    education_budget_tax_sld.set_value(this->new_policy.education_budget);
    education_budget_tax_sld.set_on_click([this](UI::Widget& w) {
        this->new_policy.education_budget = static_cast<UI::Slider&>(w).get_value();
        w.set_text(string_format("%.2f%%", this->new_policy.education_budget * 100.f));
    });
    education_budget_tax_sld.set_tooltip("% of the national budget dedicated to education");
    education_budget_tax_sld.on_click(education_budget_tax_sld);

    investment_grp.make_widget<UI::Label>(0, 0, "Private ownership");
    auto& private_ownership_sld = investment_grp.make_widget<UI::Slider>(0, 0, 128, 24, -1.f, 1.f);
    private_ownership_sld.set_value(this->new_policy.private_ownership);
    private_ownership_sld.set_on_click([this](UI::Widget& w) {
        this->new_policy.private_ownership = static_cast<UI::Slider&>(w).get_value();
        w.set_text(string_format("%.2f%%", this->new_policy.private_ownership * 100.f));
    });
    private_ownership_sld.set_tooltip("Maximum allowed private ownership stake % for factories\nAllows private ownership of factories, where the burgeoise would profit from it instead of the industry itself");
    private_ownership_sld.on_click(private_ownership_sld);

    investment_grp.make_widget<UI::Label>(0, 0, "Foreign ownership");
    auto& foreign_ownership_sld = investment_grp.make_widget<UI::Slider>(0, 0, 128, 24, -1.f, 1.f);
    foreign_ownership_sld.set_value(this->new_policy.foreign_ownership);
    foreign_ownership_sld.set_on_click([this](UI::Widget& w) {
        this->new_policy.foreign_ownership = static_cast<UI::Slider&>(w).get_value();
        w.set_text(string_format("%.2f%%", this->new_policy.foreign_ownership * 100.f));
    });
    foreign_ownership_sld.set_tooltip("Maximum allowed foreign ownership stake % for factories\nAllows foreign investment for the construction of factories");
    foreign_ownership_sld.on_click(foreign_ownership_sld);

    investment_grp.make_widget<UI::Label>(0, 0, "Collective ownership");
    auto& collective_ownership_sld = investment_grp.make_widget<UI::Slider>(0, 0, 128, 24, -1.f, 1.f);
    collective_ownership_sld.set_value(this->new_policy.collective_ownership);
    collective_ownership_sld.set_on_click([this](UI::Widget& w) {
        this->new_policy.collective_ownership = static_cast<UI::Slider&>(w).get_value();
        w.set_text(string_format("%.2f%%", this->new_policy.collective_ownership * 100.f));
    });
    collective_ownership_sld.set_tooltip("Maximum allowed collective ownership stake % for factories\nA collective represents the workers that work on the industry itself; allowing laborers to profit from the industry");
    collective_ownership_sld.on_click(collective_ownership_sld);

    investment_grp.make_widget<UI::Label>(0, 0, "Individual ownership");
    auto& individual_ownership_sld = investment_grp.make_widget<UI::Slider>(0, 0, 128, 24, -1.f, 1.f);
    individual_ownership_sld.set_value(this->new_policy.individual_ownership);
    individual_ownership_sld.set_on_click([this](UI::Widget& w) {
        this->new_policy.individual_ownership = static_cast<UI::Slider&>(w).get_value();
        w.set_text(string_format("%.2f%%", this->new_policy.individual_ownership * 100.f));
    });
    individual_ownership_sld.set_tooltip("Maximum allowed individual ownership stake % for factories\nIndividual ownership pertains to stakes of a industry owned by POPs, and allowing them to profit from factories with an incurred efficiency penalty");
    individual_ownership_sld.on_click(individual_ownership_sld);

    //
    // State economy
    //
    auto& state_economy_grp = this->make_widget<UI::Group>(207 + 196 + 196, 38, 196, this->height - (this->padding.y + 38 + 48));
    state_economy_grp.flex = UI::Flex::COLUMN;
    state_economy_grp.is_scroll = true;

    std::vector<int> sizes{ 120, 128 };
    std::vector<std::string> header{ "Commodity", "Scale" };
    auto& commodity_table = state_economy_grp.make_widget<UI::Table<uint32_t>>(0, 0, state_economy_grp.height, 32, sizes, header);
    commodity_table.reserve(1);
    this->commodity_production = this->gs.curr_nation->commodity_production;
    for(const auto& commodity : this->gs.world->commodities) {
        auto& row = commodity_table.get_row(commodity.get_id());
        auto* commodity_row = row.get_element(0);
        commodity_row->set_key(commodity.name.data());
        auto& commodity_img = commodity_row->make_widget<UI::Image>(0, 0, 35, 35, commodity.get_icon_path(), true);
        commodity_img.set_tooltip(commodity.name.data());

        auto* scale = row.get_element(1);
        scale->set_key(commodity.name.data());
        auto& scale_sld = scale->make_widget<UI::Slider>(0, 0, 128, 24, 0.f, 1.f);
        scale_sld.set_value(this->commodity_production[commodity]);
        scale_sld.set_on_click([this, &commodity](UI::Widget& w) {
            this->commodity_production[commodity] = static_cast<UI::Slider&>(w).get_value();
            w.set_text(string_format("%.2f%%", this->commodity_production[commodity] * 100.f));
            if(this->gs.curr_nation->can_directly_control_factories()) {
                w.set_tooltip("Scale the production of this product (only applies to factories with a state stake on them)");
            } else {
                w.set_tooltip("Scale the production of this product (applies to all factories regardless of stake)");
            }
        });
        scale_sld.on_click(scale_sld);
    }

    auto& enact_btn = this->make_widget<UI::Button>(207, 0, 128, 24);
    enact_btn.below_of(tax_grp);
    enact_btn.set_text("Enact policy");
    enact_btn.set_on_click([this](UI::Widget&) {
        Eng3D::Networking::Packet packet{};
        Eng3D::Deser::Archive ar{};
        Eng3D::Deser::serialize<ActionType>(ar, ActionType::NATION_ENACT_POLICY);
        Eng3D::Deser::serialize(ar, this->new_policy); // PoliciesObj
        Eng3D::Deser::serialize(ar, this->commodity_production); // VectorFloatObj
        packet.data(ar.get_buffer(), ar.size());
        if(this->gs.client)
            this->gs.client->send(packet);
        else {
            this->gs.curr_nation->set_policy(this->new_policy);
            this->gs.curr_nation->commodity_production = this->commodity_production;
        }
        this->gs.ui_ctx.prompt("Policy", "New policy enacted!");
    });
}
