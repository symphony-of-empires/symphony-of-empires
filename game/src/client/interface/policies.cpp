// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      client/interface/policies.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "client/interface/policies.hpp"
#include "client/client_network.hpp"
#include "unified_render/serializer.hpp"
#include "io_impl.hpp"
#include "unified_render/ui/components.hpp"

#include "unified_render/texture.hpp"
#include "unified_render/path.hpp"

using namespace Interface;

#define POLICY_CHECKBOX(x, title, body)\
    auto* x## _chk = new UI::Checkbox(0, 0, 128, 24, reform_grp);\
    x## _chk->text(title);\
    x## _chk->set_on_click([](UI::Widget& w) {\
        auto& o = static_cast<PoliciesScreen&>(*w.parent->parent);\
        o.new_policy.x = !(o.new_policy).x;\
        ((UI::Checkbox&)w).set_value((o.new_policy).x);\
    });\
    x## _chk->set_value(new_policy.x);\
    x## _chk->tooltip = new UI::Tooltip(x## _chk, 512, 24);\
    x## _chk->tooltip->text(body);

PoliciesScreen::PoliciesScreen(GameState& _gs)
    : UI::Window(0, 0, 512, 400, nullptr),
    gs{ _gs }
{
    this->padding.x = 0;
    this->padding.y = 48;

    this->current_texture = &gs.tex_man->load(Path::get("gfx/policies_screen.png"));
    this->width = this->current_texture->width;
    this->height = this->current_texture->height;
    this->text("Laws and goverment");
    this->is_scroll = false;
    this->new_policy = gs.curr_nation->current_policy;

    auto* gov_lab = new UI::Label(0, 0, "Goverment", this);
    auto* ideology_lab = new UI::Label(6, 38, "IDEOLOGY", this);
    ideology_lab->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<PoliciesScreen&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        // TODO: More dynamic names
        if(o.gs.curr_nation->ideology != nullptr) {
            w.text(o.gs.curr_nation->ideology->name);
        }
    });
    ideology_lab->on_each_tick(*ideology_lab);

    auto* ideology_pie_lab = new UI::Label(0, 82, "House", this);
    auto* ideology_pie = new UI::PieChart(0, 0, 128, 128, this);
    ideology_pie->below_of(*ideology_pie_lab);
    ideology_pie->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<PoliciesScreen&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        std::vector<UI::ChartData> ideology_data;
        for(const auto& ideology : o.gs.world->ideologies) {
            ideology_data.push_back(UI::ChartData(1.f, ideology->name, ideology->color));
        }
        ((UI::PieChart&)w).set_data(ideology_data);
    });
    ideology_pie->on_each_tick(*ideology_pie);

    auto* militancy_lab = new UI::Label(0, 290, " ", this);
    militancy_lab->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<PoliciesScreen&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        float num = 0.f;
        if(!o.gs.curr_nation->owned_provinces.empty()) {
            for(const auto& province : o.gs.curr_nation->owned_provinces) {
                for(const auto& pop : province->pops) {
                    num += pop.militancy;
                }
            }
            num /= o.gs.curr_nation->owned_provinces.size();
        }
        w.text("Militancy: " + std::to_string(num));
    });
    militancy_lab->on_each_tick(*militancy_lab);

    auto* con_lab = new UI::Label(0, 290, " ", this);
    con_lab->below_of(*militancy_lab);
    con_lab->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<PoliciesScreen&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }

        float num = 0.f;
        if(!o.gs.curr_nation->owned_provinces.empty()) {
            for(const auto& province : o.gs.curr_nation->owned_provinces) {
                for(const auto& pop : province->pops) {
                    num += pop.con;
                }
            }
            num /= o.gs.curr_nation->owned_provinces.size();
        }
        w.text("Consciousness: " + std::to_string(num));
    });
    con_lab->on_each_tick(*con_lab);

    auto* reform_grp = new UI::Group(207, 38, this->width - 207, this->height - (this->padding.y + 38 + 48), this);
    reform_grp->is_scroll = true;

    // Social
    POLICY_CHECKBOX(slavery, "Slavery", "Allows to put the burden of work to slaves for free");

    POLICY_CHECKBOX(secular_education, "Secular education", "Educates people in a way that it's bound to religion");
    secular_education_chk->below_of(*slavery_chk);

    POLICY_CHECKBOX(private_property, "Private property", "Allow the right to own property");
    private_property_chk->below_of(*secular_education_chk);

    POLICY_CHECKBOX(public_education, "Public education", "(paid by the goverment)");
    public_education_chk->below_of(*private_property_chk);

    POLICY_CHECKBOX(public_healthcare, "Public healthcare", "(paid by the goverment)");
    public_healthcare_chk->below_of(*public_education_chk);

    POLICY_CHECKBOX(social_security, "Social security", "Help people in need with goverment support");
    social_security_chk->below_of(*public_healthcare_chk);

    auto* poor_tax_sld = new UI::Slider(0, 0, 128, 24, -10.f, 100.f, reform_grp);
    poor_tax_sld->below_of(*social_security_chk);
    poor_tax_sld->text(std::to_string(new_policy.poor_flat_tax));
    poor_tax_sld->value = new_policy.poor_flat_tax;
    poor_tax_sld->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<PoliciesScreen&>(*w.parent->parent);
        o.new_policy.poor_flat_tax = ((UI::Slider&)w).value;
        w.text(std::to_string(o.new_policy.poor_flat_tax));
    });
    poor_tax_sld->tooltip = new UI::Tooltip(poor_tax_sld, 512, 24);
    poor_tax_sld->tooltip->text("The taxing done to the low class (flat %)");

    auto* med_tax_sld = new UI::Slider(0, 0, 128, 24, -10.f, 100.f, reform_grp);
    med_tax_sld->below_of(*poor_tax_sld);
    med_tax_sld->text(std::to_string(new_policy.poor_flat_tax));
    med_tax_sld->value = new_policy.med_flat_tax;
    med_tax_sld->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<PoliciesScreen&>(*w.parent->parent);
        o.new_policy.med_flat_tax = ((UI::Slider&)w).value;
        w.text(std::to_string(o.new_policy.med_flat_tax));
    });
    med_tax_sld->tooltip = new UI::Tooltip(med_tax_sld, 512, 24);
    med_tax_sld->tooltip->text("The taxing done to the medium class (flat %)");

    auto* rich_tax_sld = new UI::Slider(0, 0, 128, 24, -10.f, 100.f, reform_grp);
    rich_tax_sld->below_of(*med_tax_sld);
    rich_tax_sld->text(std::to_string(new_policy.poor_flat_tax));
    rich_tax_sld->value = new_policy.rich_flat_tax;
    rich_tax_sld->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<PoliciesScreen&>(*w.parent->parent);
        o.new_policy.rich_flat_tax = ((UI::Slider&)w).value;
        w.text(std::to_string(o.new_policy.rich_flat_tax));
    });
    rich_tax_sld->tooltip = new UI::Tooltip(rich_tax_sld, 512, 24);
    rich_tax_sld->tooltip->text("The taxing done to the high class (flat %)");

    // Goverment structure
    POLICY_CHECKBOX(legislative_parliament, "Legislative parliament", "The legislative parliament allows to have a democratic vote on laws");
    legislative_parliament_chk->below_of(*rich_tax_sld);

    POLICY_CHECKBOX(executive_parliament, "Executive parliament", "The executive parliament will approve/disapprove acts of diplomacy and war");
    executive_parliament_chk->below_of(*legislative_parliament_chk);

    POLICY_CHECKBOX(constitutional, "Constitutional", "The constitution will limit the governing power of the goverment");
    constitutional_chk->below_of(*executive_parliament_chk);

    // Foreing policies
    POLICY_CHECKBOX(foreign_trade, "Foreign Trade", "Controls foreing trade with other countries");
    foreign_trade_chk->below_of(*constitutional_chk);

    auto* enact_btn = new UI::Button(207, 0, 128, 24, this);
    enact_btn->below_of(*reform_grp);
    enact_btn->text("Enact policy");
    enact_btn->on_click = ([](UI::Widget& w) {
        auto& o = static_cast<PoliciesScreen&>(*w.parent);
        UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
        Archive ar = Archive();
        ActionType action = ActionType::NATION_ENACT_POLICY;
        ::serialize(ar, &action);
        ::serialize(ar, &o.new_policy); // PoliciesObj
        packet.data(ar.get_buffer(), ar.size());
        std::scoped_lock lock(g_client->pending_packets_mutex);
        g_client->pending_packets.push_back(packet);

        o.gs.ui_ctx->prompt("Policy", "New policy enacted!");
    });

    auto* close_btn = new UI::CloseButton(207, 0, 128, 24, this);
    close_btn->below_of(*enact_btn);
    close_btn->text("Close");
}
