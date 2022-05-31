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

#include "eng3d/path.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/locale.hpp"
#include "eng3d/string_format.hpp"
#include "eng3d/ui/piechart.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/table.hpp"
#include "eng3d/ui/close_button.hpp"

#include "province_view.hpp"
#include "nation_view.hpp"
#include "client/game_state.hpp"
#include "client/map.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "building.hpp"

using namespace Interface;

void ProvincePopulationTab::update_piecharts() {
    // Obtain population information
    std::vector<size_t> culture_sizes(gs.world->cultures.size(), 0);
    std::vector<size_t> religion_sizes(gs.world->religions.size(), 0);
    std::vector<size_t> pop_type_sizes(gs.world->pop_types.size(), 0);
    for(const auto& pop : province.pops) {
        culture_sizes[gs.world->get_id(*pop.culture)] += pop.size;
        religion_sizes[gs.world->get_id(*pop.religion)] += pop.size;
        pop_type_sizes[gs.world->get_id(*pop.type)] += pop.size;
    }

    std::vector<UI::ChartData> cultures_data, religions_data, pop_types_data;
    for(const auto& culture : gs.world->cultures) {
        cultures_data.push_back(UI::ChartData(culture_sizes[gs.world->get_id(culture)], culture.name.get_string(), Eng3D::Color::rgba32(culture.color)));
    }
    cultures_pie->set_data(cultures_data);

    for(const auto& religion : gs.world->religions) {
        religions_data.push_back(UI::ChartData(religion_sizes[gs.world->get_id(religion)], religion.name.get_string(), Eng3D::Color::rgba32(religion.color)));
    }
    religions_pie->set_data(religions_data);

    for(const auto& pop_type : gs.world->pop_types) {
        const auto color = Eng3D::Color(
            (uint8_t)((gs.world->get_id(pop_type) * 12) % 256),
            (uint8_t)((gs.world->get_id(pop_type) * 31) % 256),
            (uint8_t)((gs.world->get_id(pop_type) * 97) % 256)
        );
        pop_types_data.push_back(UI::ChartData(pop_type_sizes[gs.world->get_id(pop_type)], pop_type.name.get_string(), color));
    }
    pop_types_pie->set_data(pop_types_data);
}

ProvincePopulationTab::ProvincePopulationTab(GameState& _gs, int x, int y, Province& _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province }
{
    this->is_scroll = true;
    this->text(province.name.get_string());

    this->landscape_img = new UI::Image(0, 0, this->width, 128 + 64 + 16, gs.tex_man->load(Path::get("gfx/terraintype/" + province.terrain_type->ref_name + ".png")), this);

    if(province.owner != nullptr) {
        this->owner_flag = new UI::AspectImage(0, 0, 96, 48, gs.get_nation_flag(*this->province.owner), this);
        this->owner_flag->set_on_click([this](UI::Widget&) {
            new Interface::NationView(this->gs, *this->province.owner);
        });
        this->owner_flag->set_tooltip(this->province.owner->name + " owns this province");
        new UI::Image(this->owner_flag->x, this->owner_flag->y, this->owner_flag->width, this->owner_flag->height, gs.tex_man->load(Path::get("gfx/flag_rug.png")), this);
    }

    // Display all the nuclei
    int dx = 0;
    for(const auto& nation : province.nuclei) {
        this->owner_flag = new UI::AspectImage(dx, this->landscape_img->height - 24, 32, 24, gs.get_nation_flag(*nation), this);
        this->owner_flag->set_on_click([this, nation](UI::Widget&) {
            new Interface::NationView(this->gs, *nation);
        });
        this->owner_flag->set_tooltip(nation->name + " has nuclei on this province");
        new UI::Image(this->owner_flag->x, this->owner_flag->y, this->owner_flag->width, this->owner_flag->height, gs.tex_man->load(Path::get("gfx/flag_rug.png")), this);
        dx += this->owner_flag->width;
    }

    auto* cultures_lab = new UI::Label(0, 0, "Cultures", this);
    cultures_lab->below_of(*this->landscape_img);
    this->cultures_pie = new UI::PieChart(0, 0, 96, 96, this);
    this->cultures_pie->below_of(*cultures_lab);

    auto* religions_lab = new UI::Label(0, 0, "Religions", this);
    religions_lab->below_of(*this->landscape_img);
    religions_lab->right_side_of(*this->cultures_pie);
    this->religions_pie = new UI::PieChart(0, 0, 96, 96, this);
    this->religions_pie->below_of(*religions_lab);
    this->religions_pie->right_side_of(*this->cultures_pie);

    auto* pop_types_lab = new UI::Label(0, 0, "Proffesions", this);
    pop_types_lab->below_of(*this->landscape_img);
    pop_types_lab->right_side_of(*this->religions_pie);
    this->pop_types_pie = new UI::PieChart(0, 0, 96, 96, this);
    this->pop_types_pie->below_of(*pop_types_lab);
    this->pop_types_pie->right_side_of(*this->religions_pie);

    std::vector<int> sizes{ 96, 128, 24, 128 };
    std::vector<std::string> header{ "Size", "Budget", "Religion", "Culture" };
    auto table = new UI::Table<uint32_t>(0, 256 + 96, 0, 500, 30, sizes, header, this);
    auto& pops = this->province.pops;
    table->reserve(pops.size());
    table->on_each_tick = [pops, table](Widget&) {
        for(size_t i = 0; i < pops.size(); i++) {
            auto& pop = pops[i];
            uint32_t id = pop.get_type_id();
            auto* row = table->get_row(id);
            size_t row_index = 0;

            auto tex_man = Eng3D::State::get_instance().tex_man;

            auto size = row->get_element(row_index++);
            auto size_str = Eng3D::string_format("%.0f", pop.size);
            size->text(size_str);
            size->set_key(pop.size);

            auto budget = row->get_element(row_index++);
            auto budget_str = Eng3D::string_format("%.0f", pop.budget / pop.size);
            budget->text(budget_str);
            auto budget_tip = Eng3D::Locale::translate("A total budget of") + " " + Eng3D::string_format("%.0f", pop.budget);
            budget->set_tooltip(budget_tip);
            budget->set_key(pop.budget / pop.size);

            auto religion = row->get_element(row_index++);
            auto religion_icon = tex_man->load(Path::get("gfx/religion/" + pop.religion->ref_name + ".png"));
            religion->current_texture = religion_icon;
            auto religion_tip = Eng3D::Locale::translate(pop.religion->name.get_string());
            religion->set_tooltip(religion_tip);
            religion->set_key(religion_tip);

            auto culture = row->get_element(row_index++);
            auto culture_str = Eng3D::Locale::translate(pop.culture->name.get_string());
            culture->text(culture_str);
            culture->set_key(culture_str);
        }
    };
    table->on_each_tick(*table);

    this->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<ProvincePopulationTab&>(w);
        o.update_piecharts();
    });
    this->on_each_tick(*this);
}

ProvinceEconomyTab::ProvinceEconomyTab(GameState& _gs, int x, int y, Province& _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province }
{
    this->text(province.name.get_string());
    this->is_scroll = true;

    this->products_pie = new UI::PieChart(0, 0, 128, 128, this);
    new UI::Label(0, 0, "Products", this);

    this->on_each_tick = ([](UI::Widget& w) {
        auto& o = static_cast<ProvinceEconomyTab&>(w);

        // Obtain demand, supply and other information about the goods
        std::vector<UI::ChartData> goods_data;
        int i = 0;
        for(const auto& good : o.gs.world->goods) {
            const auto good_col = Eng3D::Color(i * 12, i * 31, i * 97);
            const Product& product = o.province.products[o.gs.world->get_id(good)];
            goods_data.push_back(UI::ChartData(product.demand, good.name.get_string(), good_col));
            i++;
        }
        o.products_pie->set_data(goods_data);
    });
    this->on_each_tick(*this);

    // Initial product info
    int i = 0;
    for(auto& good : gs.world->goods) {
        auto* info = new ProductInfo(this->gs, 0, (i * 24) + 128, province, good, this);
        product_infos.push_back(info);
        i++;
    }
}

ProvinceBuildingTab::ProvinceBuildingTab(GameState& _gs, int x, int y, Province& _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province }
{
    this->text(province.name.get_string());

    // Initial product info
    unsigned int dy = 0;

    auto* build_btn = new UI::Button(0, 0, 128, 24, this);
    build_btn->text("Build new");
    build_btn->set_on_click([this](UI::Widget&) {
        new BuildingBuildView(this->gs, 0, 0, false, this->province);
    });
    dy += build_btn->height;

    for(unsigned int i = 0; i < gs.world->building_types.size(); i++) {
        if(province.buildings[i].level) {
            auto* info = new BuildingInfo(this->gs, 0, dy, province, i, this);
            this->building_infos.push_back(info);
            dy += info->height;
        }
    }
}

ProvinceEditCultureTab::ProvinceEditCultureTab(GameState& _gs, int x, int y, Province& _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province },
    culture{ _gs.world->cultures[0] }
{
    this->text(province.name.get_string());
    this->is_scroll = true;

    // Initial product info
    unsigned int dy = 0;
    for(auto& culture : gs.world->cultures) {
        auto* btn = new UI::Button(0, dy, 128, 24, this);
        btn->text(culture.name.get_string());
        btn->set_on_click([this, &culture](UI::Widget&) {
            for(auto& pop : const_cast<Province&>(this->province).pops) {
                pop.culture = &culture;
            }
            this->gs.map->update_mapmode();
            this->gs.input.selected_culture = &culture;
        });
        dy += btn->height;
    }
}

ProvinceEditReligionTab::ProvinceEditReligionTab(GameState& _gs, int x, int y, Province& _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province },
    religion{ _gs.world->religions[0] }
{
    this->text(province.name.get_string());
    this->is_scroll = true;

    // Initial product info
    unsigned int dy = 0;
    for(auto& religion : gs.world->religions) {
        auto* btn = new UI::Button(0, dy, 128, 24, this);
        btn->text(religion.name.get_string());
        btn->set_on_click([this, &religion](UI::Widget&) {
            for(auto& pop : const_cast<Province&>(this->province).pops) {
                pop.religion = &religion;
            }
            this->gs.map->update_mapmode();
            this->gs.input.selected_religion = &religion;
        });
        dy += btn->height;
    }
}

ProvinceEditTerrainTab::ProvinceEditTerrainTab(GameState& _gs, int x, int y, Province& _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province },
    terrain_type{ _gs.world->terrain_types[0] }
{
    this->text(province.name.get_string());

    std::vector<int> sizes{ 96, 128 };
    std::vector<std::string> header{ "Landscape", "Name" };
    auto table = new UI::Table<uint32_t>(0, 0, 0, this->height, 30, sizes, header, this);
    table->reserve(gs.world->terrain_types.size());
    table->on_each_tick = [this, table](Widget&) {
        for(auto& terrain_type : this->gs.world->terrain_types) {
            auto* row = table->get_row(this->gs.world->get_id(terrain_type));
            size_t row_index = 0;

            auto tex_man = Eng3D::State::get_instance().tex_man;

            auto landscape = row->get_element(row_index++);
            auto landscape_icon = tex_man->load(Path::get("gfx/terraintype/" + terrain_type.ref_name + ".png"));
            landscape->current_texture = landscape_icon;
            auto landscape_tip = Eng3D::Locale::translate(terrain_type.name.get_string());
            landscape->set_tooltip(landscape_tip);
            landscape->set_key(landscape_tip);

            auto name = row->get_element(row_index++);
            auto name_str = Eng3D::Locale::translate(terrain_type.name.get_string());
            name->text(name_str);
            name->set_tooltip(name_str);
            name->set_key(name_str);
            name->set_on_click([this, &terrain_type](UI::Widget&) {
                const_cast<Province&>(this->province).terrain_type = const_cast<TerrainType*>(&terrain_type);
                this->gs.map->update_mapmode();
            });
        }
    };
    table->on_each_tick(*table);
}

ProvinceView::ProvinceView(GameState& _gs, Province& _province)
    : UI::Window(-400, 0, 400, _gs.height),
    gs{ _gs },
    province{ _province }
{
    if(gs.right_side_panel != nullptr)
        gs.right_side_panel->kill();
    gs.right_side_panel = this;
    gs.map->set_selected_province(true, _gs.world->get_id(_province));

    this->set_close_btn_function([this](Widget&) {
        this->kill();
        this->gs.right_side_panel = nullptr;
        gs.map->set_selected_province(false, 0);
    });

    this->origin = UI::Origin::UPPER_RIGHT_SCREEN;
    this->is_scroll = false;
    this->text(province.name.get_string());

    this->pop_tab = new ProvincePopulationTab(gs, 0, 32, province, this);
    this->pop_tab->is_render = true;
    auto* pop_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man->load(Path::get("gfx/pv_1.png")), this);
    pop_ibtn->set_on_click([this](UI::Widget&) {
        this->pop_tab->is_render = true;
        this->econ_tab->is_render = false;
        this->build_tab->is_render = false;
    });
    pop_ibtn->tooltip = new UI::Tooltip(pop_ibtn, 512, 24);
    pop_ibtn->tooltip->text("Population");

    this->econ_tab = new ProvinceEconomyTab(gs, 0, 32, province, this);
    this->econ_tab->is_render = false;
    auto* econ_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man->load(Path::get("gfx/money.png")), this);
    econ_ibtn->right_side_of(*pop_ibtn);
    econ_ibtn->set_on_click([this](UI::Widget&) {
        this->pop_tab->is_render = false;
        this->econ_tab->is_render = true;
        this->build_tab->is_render = false;
    });
    econ_ibtn->tooltip = new UI::Tooltip(econ_ibtn, 512, 24);
    econ_ibtn->tooltip->text("Economy");

    this->build_tab = new ProvinceBuildingTab(gs, 0, 32, province, this);
    this->build_tab->is_render = false;
    auto* build_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man->load(Path::get("gfx/pv_0.png")), this);
    build_ibtn->right_side_of(*econ_ibtn);
    build_ibtn->set_on_click([this](UI::Widget&) {
        this->pop_tab->is_render = false;
        this->econ_tab->is_render = false;
        this->build_tab->is_render = true;
    });
    build_ibtn->tooltip = new UI::Tooltip(build_ibtn, 512, 24);
    build_ibtn->tooltip->text("Buildings");

    if(gs.editor) {
        rename_inp = new UI::Input(0, this->height - (64 + 24), 128, 24, this);
        rename_inp->set_buffer(province.name.get_string());

        auto* xchg_name_btn = new UI::Button(0, this->height - (64 + 24), 32, 32, this);
        xchg_name_btn->right_side_of(*rename_inp);
        xchg_name_btn->set_on_click([this](UI::Widget&) {
            const_cast<Province&>(this->province).name = this->rename_inp->get_buffer();
            this->gs.map->create_labels();
            this->gs.ui_ctx->prompt("Update", "Updated name of province to \"" + this->province.name + "\"!");
        });
        xchg_name_btn->set_tooltip("Rename province");

        density_sld = new UI::Slider(0, this->height - (64 + 24), 128, 24, 0.1f, 2.f, this);
        density_sld->right_side_of(*xchg_name_btn);
        density_sld->set_value(0.f);
        density_sld->set_on_click([this](UI::Widget& w) {
            w.text(std::to_string(((UI::Slider&)w).get_value()));
            const float den = this->density_sld->value;
            for(auto& pop : const_cast<Province&>(this->province).pops)
                pop.size *= den;
            this->gs.map->update_mapmode();
        });

        auto* fill_pops_btn = new UI::Button(0, this->height - 64, 32, 32, this);
        fill_pops_btn->below_of(*density_sld);
        fill_pops_btn->set_on_click([this](UI::Widget&) {
            // Get max sv
            float max_sv = 1.f;
            for(const auto& pop_type : this->gs.world->pop_types) {
                if(pop_type.social_value > max_sv)
                    max_sv = pop_type.social_value;
            }

            if(this->gs.input.selected_culture == nullptr)
                this->gs.input.selected_culture = &this->gs.world->cultures[0];
            if(this->gs.input.selected_religion == nullptr)
                this->gs.input.selected_religion = &this->gs.world->religions[0];

            for(auto& pop_type : this->gs.world->pop_types) {
                Pop pop;
                pop.type = &pop_type;
                pop.culture = this->gs.input.selected_culture;
                pop.religion = this->gs.input.selected_religion;
                pop.size = 1000.f / std::max<Eng3D::Decimal>(0.01f, pop_type.social_value);
                pop.literacy = max_sv / std::max<Eng3D::Decimal>(0.01f, pop_type.social_value);
                pop.budget = 100.f * max_sv;
                const_cast<Province&>(this->province).pops.push_back(pop);
            }
            this->gs.map->update_mapmode();
        });
        fill_pops_btn->set_tooltip("Add POPs (will add " + std::to_string(gs.world->pop_types.size()) + "POPs)");

        auto* clear_pops_btn = new UI::Button(0, this->height - 64, 32, 32, this);
        clear_pops_btn->below_of(*density_sld);
        clear_pops_btn->right_side_of(*fill_pops_btn);
        clear_pops_btn->set_on_click([this](UI::Widget&) {
            const_cast<Province&>(this->province).pops.clear();
            this->gs.map->update_mapmode();
        });
        clear_pops_btn->set_tooltip("Removes all POPs from this province ;)");

        this->edit_culture_tab = new ProvinceEditCultureTab(gs, 0, 32, province, this);
        this->edit_culture_tab->is_render = false;

        auto* edit_culture_btn = new UI::Button(0, this->height - 64, 32, 32, this);
        edit_culture_btn->below_of(*density_sld);
        edit_culture_btn->right_side_of(*clear_pops_btn);
        edit_culture_btn->set_on_click([this](UI::Widget&) {
            this->pop_tab->is_render = false;
            this->econ_tab->is_render = false;
            this->build_tab->is_render = false;
            this->edit_culture_tab->is_render = true;
            this->edit_religion_tab->is_render = false;
            this->edit_terrain_tab->is_render = false;
        });
        edit_culture_btn->set_tooltip("Edit culture");

        this->edit_religion_tab = new ProvinceEditReligionTab(gs, 0, 32, province, this);
        this->edit_religion_tab->is_render = false;

        auto* edit_religion_btn = new UI::Button(0, this->height - 64, 32, 32, this);
        edit_religion_btn->below_of(*density_sld);
        edit_religion_btn->right_side_of(*edit_culture_btn);
        edit_religion_btn->set_on_click([this](UI::Widget&) {
            this->pop_tab->is_render = false;
            this->econ_tab->is_render = false;
            this->build_tab->is_render = false;
            this->edit_culture_tab->is_render = false;
            this->edit_religion_tab->is_render = true;
            this->edit_terrain_tab->is_render = false;
        });
        edit_religion_btn->set_tooltip("Edit religion");

        this->edit_terrain_tab = new ProvinceEditTerrainTab(gs, 0, 32, province, this);
        this->edit_terrain_tab->is_render = false;

        auto* edit_terrain_btn = new UI::Button(0, this->height - 64, 32, 32, this);
        edit_terrain_btn->below_of(*density_sld);
        edit_terrain_btn->right_side_of(*edit_religion_btn);
        edit_terrain_btn->set_on_click([this](UI::Widget&) {
            this->pop_tab->is_render = false;
            this->econ_tab->is_render = false;
            this->build_tab->is_render = false;
            this->edit_culture_tab->is_render = false;
            this->edit_religion_tab->is_render = false;
            this->edit_terrain_tab->is_render = true;
        });
        edit_terrain_btn->set_tooltip("Edit terrain");
    }
}

ProvinceView::~ProvinceView() {
    gs.map->set_selected_province(true, 0);
}