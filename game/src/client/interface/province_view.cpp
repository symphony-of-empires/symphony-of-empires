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
//      client/interface/province_view.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "province_view.hpp"
#include "nation_view.hpp"

#include "client/game_state.hpp"
#include "client/map.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "building.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/ui/piechart.hpp"
#include "unified_render/ui/label.hpp"
#include "unified_render/ui/image.hpp"
#include "unified_render/ui/tooltip.hpp"
#include "unified_render/ui/button.hpp"
#include "unified_render/ui/close_button.hpp"

using namespace Interface;

void ProvincePopulationTab::update_piecharts() {
    // Obtain population information
    std::vector<size_t> culture_sizes(gs.world->cultures.size(), 0);
    std::vector<size_t> religion_sizes(gs.world->religions.size(), 0);
    std::vector<size_t> pop_type_sizes(gs.world->pop_types.size(), 0);
    for(const auto& pop : province->pops) {
        culture_sizes[gs.world->get_id(pop.culture)] += pop.size;
        religion_sizes[gs.world->get_id(pop.religion)] += pop.size;
        pop_type_sizes[gs.world->get_id(pop.type)] += pop.size;
    }

    std::vector<UI::ChartData> cultures_data, religions_data, pop_types_data;
    for(const auto& culture : gs.world->cultures) {
        cultures_data.push_back(UI::ChartData(culture_sizes[gs.world->get_id(culture)], culture->name, UnifiedRender::Color::rgba32(culture->color)));
    }
    cultures_pie->set_data(cultures_data);

    for(const auto& religion : gs.world->religions) {
        religions_data.push_back(UI::ChartData(religion_sizes[gs.world->get_id(religion)], religion->name, UnifiedRender::Color::rgba32(religion->color)));
    }
    religions_pie->set_data(religions_data);

    for(const auto& pop_type : gs.world->pop_types) {
        const auto color = UnifiedRender::Color(
            (uint8_t)((gs.world->get_id(pop_type) * 12) % 256),
            (uint8_t)((gs.world->get_id(pop_type) * 31) % 256),
            (uint8_t)((gs.world->get_id(pop_type) * 97) % 256)
        );
        pop_types_data.push_back(UI::ChartData(pop_type_sizes[gs.world->get_id(pop_type)], pop_type->name, color));
    }
    pop_types_pie->set_data(pop_types_data);

    if(pop_infos.size() < province->pops.size()) {
        for(size_t i = pop_infos.size(); i < province->pops.size(); i++) {
            PopInfo* info = new PopInfo(gs, 0, (i * 24) + 128, province, i, this);
            pop_infos.push_back(info);
        }
    } else if(pop_infos.size() > province->pops.size()) {
        for(size_t i = province->pops.size(); i < pop_infos.size(); i++) {
            pop_infos[i]->kill();
        }
        pop_infos.erase(pop_infos.begin() + province->pops.size(), pop_infos.end());
    }
}

ProvincePopulationTab::ProvincePopulationTab(GameState& _gs, int x, int y, Province* _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province }
{
    this->text(province->name);

    this->cultures_pie = new UI::PieChart(0, 0, 128, 128, this);
    new UI::Label(0, 0, "Cultures", this);

    this->religions_pie = new UI::PieChart(0, 0, 128, 128, this);
    this->religions_pie->right_side_of(*this->cultures_pie);
    new UI::Label(this->religions_pie->x, this->religions_pie->y, "Religions", this);

    this->pop_types_pie = new UI::PieChart(0, 0, 128, 128, this);
    this->pop_types_pie->right_side_of(*this->religions_pie);
    new UI::Label(this->pop_types_pie->x, this->pop_types_pie->y, "Proffesions", this);

    update_piecharts();
    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvincePopulationTab&>(w);
        if(o.gs.world->time % o.gs.world->ticks_per_month) {
            return;
        }
        
        o.update_piecharts();
    });
}

ProvinceEconomyTab::ProvinceEconomyTab(GameState& _gs, int x, int y, Province* _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province }
{
    this->text(province->name);

    this->products_pie = new UI::PieChart(0, 0, 128, 128, this);
    new UI::Label(0, 0, "Products", this);

    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceEconomyTab&>(w);

        // Obtain demand, supply and other information about the goods
        std::vector<UI::ChartData> goods_data, products_data;
        for(const auto& product : o.province->products) {
            //if(product->building == nullptr) {
            //    continue;
            //}

            const auto product_col = UnifiedRender::Color(
                o.gs.world->get_id(product) * 12,
                o.gs.world->get_id(product) * 31,
                o.gs.world->get_id(product) * 97
            );

            products_data.push_back(UI::ChartData(product->demand, product->good->name, product_col));
        }
        o.products_pie->set_data(products_data);
    });

    // Initial product info
    unsigned int i = 0;
    for(const auto& product : this->province->products) {
        //if(product->building == nullptr) {
        //    continue;
        //}

        auto* info = new ProductInfo(this->gs, 0, (i * 24) + 128, product, this);
        this->product_infos.push_back(info);
        i++;
    }
}

ProvinceBuildingTab::ProvinceBuildingTab(GameState& _gs, int x, int y, Province* _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province }
{
    this->text(province->name);

    // Initial product info
    unsigned int dy = 0;

    auto* build_btn = new UI::Button(0, 0, 128, 24, this);
    build_btn->text("Build new");
    build_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent->parent);
        new BuildingBuildView(o.gs, 0, 0, false, o.gs.curr_nation, o.province);
    });
    dy += build_btn->height;

    for(unsigned int i = 0; i < province->get_buildings().size(); i++) {
        auto* info = new BuildingInfo(this->gs, 0, dy, province, i, this);
        this->building_infos.push_back(info);
        dy += info->height;
    }
}

ProvinceEditCultureTab::ProvinceEditCultureTab(GameState& _gs, int x, int y, Province* _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province }
{
    this->text(province->name);

    // Initial product info
    unsigned int dy = 0;

    for(const auto& culture : gs.world->cultures) {
        auto* btn = new UI::Button(0, dy, 128, 24, this);
        btn->text(culture->name);
        btn->user_data = (void*)culture;
        btn->on_click = ([](UI::Widget& w, void* data) {
            auto& o = static_cast<ProvinceEditCultureTab&>(*w.parent);
            for(auto& pop : o.province->pops) {
                pop.culture = (Culture*)data;
            }
            o.gs.map->update_mapmode();
        });
        dy += btn->height;
    }
}

ProvinceEditTerrainTab::ProvinceEditTerrainTab(GameState& _gs, int x, int y, Province* _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province }
{
    this->text(province->name);

    // Initial product info
    unsigned int dy = 0;

    for(const auto& terrain_type : gs.world->terrain_types) {
        auto* btn = new UI::Button(0, dy, 128, 24, this);
        btn->text(terrain_type->name);
        btn->user_data = (void*)terrain_type;
        btn->on_click = ([](UI::Widget& w, void* data) {
            auto& o = static_cast<ProvinceEditTerrainTab&>(*w.parent);
            o.province->terrain_type = (TerrainType*)data;
            o.gs.map->update_mapmode();
        });
        dy += btn->height;
    }
}

ProvinceView::ProvinceView(GameState& _gs, Province* _province)
    : UI::Window(-400, 0, 400, _gs.height),
    gs{ _gs },
    province{ _province }
{
    if(gs.right_side_panel != nullptr) {
        gs.right_side_panel->kill();
    }
    gs.right_side_panel = this;

    this->origin = UI::Origin::UPPER_RIGHT_SCREEN;
    this->is_scroll = false;
    this->text(province->name);

    this->pop_tab = new ProvincePopulationTab(gs, 0, 32, province, this);
    this->pop_tab->is_render = true;
    auto* pop_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/pv_1.png")), this);
    pop_ibtn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);

        o.pop_tab->is_render = true;
        o.econ_tab->is_render = false;
        o.build_tab->is_render = false;
    });
    pop_ibtn->tooltip = new UI::Tooltip(pop_ibtn, 512, 24);
    pop_ibtn->tooltip->text("Population");

    this->econ_tab = new ProvinceEconomyTab(gs, 0, 32, province, this);
    this->econ_tab->is_render = false;
    auto* econ_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/pv_2.png")), this);
    econ_ibtn->right_side_of(*pop_ibtn);
    econ_ibtn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);

        o.pop_tab->is_render = false;
        o.econ_tab->is_render = true;
        o.build_tab->is_render = false;
    });
    econ_ibtn->tooltip = new UI::Tooltip(econ_ibtn, 512, 24);
    econ_ibtn->tooltip->text("Economy");

    this->build_tab = new ProvinceBuildingTab(gs, 0, 32, province, this);
    this->build_tab->is_render = false;
    auto* build_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/pv_0.png")), this);
    build_ibtn->right_side_of(*econ_ibtn);
    build_ibtn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);

        o.pop_tab->is_render = false;
        o.econ_tab->is_render = false;
        o.build_tab->is_render = true;
    });
    build_ibtn->tooltip = new UI::Tooltip(build_ibtn, 512, 24);
    build_ibtn->tooltip->text("Buildings");

    auto* nation_ibtn = new UI::Image(0, 0, 32, 32, &gs.tex_man->load(Path::get("gfx/pv_3.png")), this);
    nation_ibtn->right_side_of(*build_ibtn);
    nation_ibtn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);

        // View the nation info only if the province has a valid owner
        if(o.province->owner == nullptr) {
            return;
        }

        new NationView(o.gs, o.province->owner);
    });
    nation_ibtn->tooltip = new UI::Tooltip(nation_ibtn, 512, 24);
    nation_ibtn->tooltip->text("Nation");

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->right_side_of(*nation_ibtn);
    close_btn->text("Close");
    close_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);
        o.gs.right_side_panel->kill();
        o.gs.right_side_panel = nullptr;
    });

    if(gs.editor) {
        rename_inp = new UI::Input(0, this->height - (64 + 24), 128, 24, this);
        rename_inp->buffer = province->name;
        rename_inp->text(rename_inp->buffer);
        
        auto* xchg_name_btn = new UI::Image(0, this->height - (64 + 24), 32, 32, &gs.tex_man->load(Path::get("gfx/pv_0.png")), this);
        xchg_name_btn->right_side_of(*rename_inp);
        xchg_name_btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ProvinceView&>(*w.parent);
            o.province->name = o.rename_inp->buffer;
            o.gs.ui_ctx->prompt("Update", "Updated name of province to \"" + o.province->name + "\"!");
        });
        xchg_name_btn->tooltip = new UI::Tooltip(xchg_name_btn, 512, 24);
        xchg_name_btn->tooltip->text("Rename province");

        density_sld = new UI::Slider(0, this->height - (64 + 24), 128, 24, 0.1f, 2.f, this);
        density_sld->right_side_of(*xchg_name_btn);
        density_sld->value = 0.f;
        density_sld->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ProvinceView&>(*w.parent);
            w.text(std::to_string(((UI::Slider&)w).value));
        });

        auto* xchg_dens_btn = new UI::Image(0, this->height - (64 + 24), 32, 32, &gs.tex_man->load(Path::get("gfx/pv_0.png")), this);
        xchg_dens_btn->right_side_of(*density_sld);
        xchg_dens_btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ProvinceView&>(*w.parent);

            const float den = o.density_sld->value;
            for(auto& pop : o.province->pops) {
                pop.size *= den;
            }
            o.gs.map->update_mapmode();
            o.gs.ui_ctx->prompt("Update", "Updated POP density of province \"" + o.province->name + "\"!");
        });
        xchg_dens_btn->tooltip = new UI::Tooltip(xchg_dens_btn, 512, 24);
        xchg_dens_btn->tooltip->text("Update density");

        auto* fill_pops_btn = new UI::Image(0, this->height - 64, 32, 32, &gs.tex_man->load(Path::get("gfx/pv_0.png")), this);
        fill_pops_btn->below_of(*xchg_dens_btn);
        fill_pops_btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ProvinceView&>(*w.parent);

            // Get max sv
            float max_sv = 1.f;
            for(const auto& pop_type : o.gs.world->pop_types) {
                if(pop_type->social_value > max_sv) {
                    max_sv = pop_type->social_value;
                }
            }

            for(auto& pop_type : o.gs.world->pop_types) {
                Pop pop;
                pop.type = g_world->pop_types.at(0);
                pop.culture = g_world->cultures.at(0);
                pop.religion = g_world->religions.at(0);
                pop.size = 1000.f / std::max(0.01f, pop_type->social_value);
                pop.literacy = max_sv / std::max(0.01f, pop_type->social_value);
                pop.budget = 100.f * max_sv;
                o.province->pops.push_back(pop);
            }
            o.gs.map->update_mapmode();
            o.gs.ui_ctx->prompt("Update", "Added POPs to province \"" + o.province->name + "\"!");
        });
        fill_pops_btn->tooltip = new UI::Tooltip(fill_pops_btn, 512, 24);
        fill_pops_btn->tooltip->text("Add POPs (will add " + std::to_string(gs.world->pop_types.size()) + "POPs)");

        auto* clear_pops_btn = new UI::Image(0, this->height - 64, 32, 32, &gs.tex_man->load(Path::get("gfx/pv_0.png")), this);
        clear_pops_btn->below_of(*xchg_dens_btn);
        clear_pops_btn->right_side_of(*fill_pops_btn);
        clear_pops_btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ProvinceView&>(*w.parent);
            o.province->pops.clear();
            o.gs.map->update_mapmode();
            o.gs.ui_ctx->prompt("Update", "Cleared POPs of province \"" + o.province->name + "\"!");
        });
        clear_pops_btn->tooltip = new UI::Tooltip(clear_pops_btn, 512, 24);
        clear_pops_btn->tooltip->text("Removes all POPs from this province ;)");

        this->edit_culture_tab = new ProvinceEditCultureTab(gs, 0, 32, province, this);
        this->edit_culture_tab->is_render = false;
        auto* edit_culture_btn = new UI::Image(0, this->height - 64, 32, 32, &gs.tex_man->load(Path::get("gfx/pv_0.png")), this);
        edit_culture_btn->below_of(*xchg_dens_btn);
        edit_culture_btn->right_side_of(*clear_pops_btn);
        edit_culture_btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ProvinceView&>(*w.parent);
            o.pop_tab->is_render = false;
            o.econ_tab->is_render = false;
            o.build_tab->is_render = false;
            o.edit_culture_tab->is_render = true;
            o.edit_terrain_tab->is_render = false;
        });
        edit_culture_btn->tooltip = new UI::Tooltip(edit_culture_btn, 512, 24);
        edit_culture_btn->tooltip->text("Edit culture");

        this->edit_terrain_tab = new ProvinceEditTerrainTab(gs, 0, 32, province, this);
        this->edit_terrain_tab->is_render = false;
        auto* edit_terrain_btn = new UI::Image(0, this->height - 64, 32, 32, &gs.tex_man->load(Path::get("gfx/pv_0.png")), this);
        edit_terrain_btn->below_of(*xchg_dens_btn);
        edit_terrain_btn->right_side_of(*edit_culture_btn);
        edit_terrain_btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ProvinceView&>(*w.parent);
            o.pop_tab->is_render = false;
            o.econ_tab->is_render = false;
            o.build_tab->is_render = false;
            o.edit_culture_tab->is_render = false;
            o.edit_terrain_tab->is_render = true;
        });
        edit_terrain_btn->tooltip = new UI::Tooltip(edit_terrain_btn, 512, 24);
        edit_terrain_btn->tooltip->text("Edit terrain");

        auto* exbuild_btn = new UI::Image(0, this->height - 64, 32, 32, &gs.tex_man->load(Path::get("gfx/pv_0.png")), this);
        exbuild_btn->below_of(*xchg_dens_btn);
        exbuild_btn->right_side_of(*edit_terrain_btn);
        exbuild_btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ProvinceView&>(*w.parent);
            for(auto& building : o.province->get_buildings()) {
                building.owner = o.gs.curr_nation;
            }
        });
        exbuild_btn->tooltip = new UI::Tooltip(exbuild_btn, 512, 24);
        exbuild_btn->tooltip->text("Expropriates all buildings to the current owner");
    }
}