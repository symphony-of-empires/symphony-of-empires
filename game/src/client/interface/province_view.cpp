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
#include "action.hpp"
#include "client/client_network.hpp"

using namespace Interface;

void ProvincePopulationTab::update_piecharts() {
    // Obtain population information
    std::vector<UI::ChartData> languages_data, religions_data, pop_types_data;
    for(const auto& language : gs.world->languages) {
        const auto size = province.languages[language.get_id()] * province.total_pops();
        languages_data.push_back(UI::ChartData(size, language.name.get_string(), Eng3D::Color::rgba32(language.color)));
    }
    languages_pie->set_data(languages_data);

    for(const auto& religion : gs.world->religions) {
        const auto size = province.religions[religion.get_id()] * province.total_pops();
        religions_data.push_back(UI::ChartData(size, religion.name.get_string(), Eng3D::Color::rgba32(religion.color)));
    }
    religions_pie->set_data(religions_data);

    std::vector<size_t> pop_type_sizes(gs.world->pop_types.size(), 0);
    for(const auto& pop : province.pops)
        pop_type_sizes[pop.type_id] += pop.size;
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

    this->landscape_img = new UI::Image(0, 0, this->width, 128 + 64 + 16, gs.tex_man.load(gs.package_man.get_unique("gfx/terraintype/" + gs.world->terrain_types[province.terrain_type_id].ref_name + ".png")), this);

    if(Nation::is_valid(province.owner_id)) {
        this->owner_flag = new UI::AspectImage(0, 0, 96, 48, gs.get_nation_flag(gs.world->nations[this->province.owner_id]), this);
        this->owner_flag->set_on_click([this](UI::Widget&) {
            new Interface::NationView(this->gs, gs.world->nations[this->province.owner_id]);
        });
        this->owner_flag->set_tooltip(gs.world->nations[this->province.owner_id].name + " owns this province");
        new UI::Image(this->owner_flag->x, this->owner_flag->y, this->owner_flag->width, this->owner_flag->height, gs.tex_man.load(gs.package_man.get_unique("gfx/flag_rug.png")), this);
    }

    // Display all the nuclei
    auto* nuclei_flex_row = new UI::Div(0, this->landscape_img->height - 24, this->width, 24, this);
    nuclei_flex_row->flex = UI::Flex::ROW;
    for(const auto& nucleus_id : province.nuclei) {
        auto& nucleus = g_world.nations[nucleus_id];
        this->owner_flag = new UI::AspectImage(0, 0, 32, 24, gs.get_nation_flag(nucleus), nuclei_flex_row);
        this->owner_flag->set_on_click([this, &nucleus](UI::Widget&) {
            new Interface::NationView(this->gs, nucleus);
        });
        this->owner_flag->set_tooltip(nucleus.name + " has nuclei on this province");
    }

    auto* languages_lab = new UI::Label(0, 0, "Languages", this);
    languages_lab->below_of(*this->landscape_img);
    this->languages_pie = new UI::PieChart(0, 0, 96, 96, this);
    this->languages_pie->below_of(*languages_lab);

    auto* religions_lab = new UI::Label(0, 0, "Religions", this);
    religions_lab->below_of(*this->landscape_img);
    religions_lab->right_side_of(*this->languages_pie);
    this->religions_pie = new UI::PieChart(0, 0, 96, 96, this);
    this->religions_pie->below_of(*religions_lab);
    this->religions_pie->right_side_of(*this->languages_pie);

    auto* pop_types_lab = new UI::Label(0, 0, "Proffesions", this);
    pop_types_lab->below_of(*this->landscape_img);
    pop_types_lab->right_side_of(*this->religions_pie);
    this->pop_types_pie = new UI::PieChart(0, 0, 96, 96, this);
    this->pop_types_pie->below_of(*pop_types_lab);
    this->pop_types_pie->right_side_of(*this->religions_pie);

    std::vector<int> sizes{ 64, 96 };
    std::vector<std::string> header{ "Size", "Budget" };
    if(gs.editor) {
        sizes.push_back(32);
        header.push_back(" ");
    }
    auto* table = new UI::Table<uint32_t>(0, 256 + 96, 0, 500, 30, sizes, header, this);
    table->reserve(this->province.pops.size());
    table->set_on_each_tick([this, table](UI::Widget&) {
        for(size_t i = 0; i < this->province.pops.size(); i++) {
            auto& pop = this->province.pops[i];
            uint32_t id = pop.get_type_id();
            auto* row = table->get_row(id);
            size_t row_index = 0;

            auto* size = row->get_element(row_index++);
            auto size_str = Eng3D::string_format("%.0f", pop.size);
            size->text(size_str);
            size->set_key(pop.size);

            auto* budget = row->get_element(row_index++);
            auto budget_str = Eng3D::string_format("%.0f", pop.budget / pop.size);
            budget->text(budget_str);
            auto budget_tip = Eng3D::string_format(_("Total budget: %.2f"), pop.budget);
            budget->set_tooltip(budget_tip);
            budget->set_key(pop.budget / pop.size);

            if(this->gs.editor) {
                auto* remove_btn = row->get_element(row_index++);
                auto remove_btn_str = "X";
                remove_btn->text(remove_btn_str);
                remove_btn->set_key(remove_btn_str);
                remove_btn->set_on_click([this, i, id, table](UI::Widget&) {
                    table->remove_row(id);
                    const_cast<Province&>(this->province).remove_pop(i);
                    table->on_each_tick(*table);
                });
            }
        }
    });
    table->on_each_tick(*table);

    this->set_on_each_tick([this](UI::Widget&) {
        this->update_piecharts();
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

    this->set_on_each_tick([](UI::Widget& w) {
        auto& o = static_cast<ProvinceEconomyTab&>(w);

        // Obtain demand, supply and other information about the goods
        std::vector<UI::ChartData> goods_data;
        int i = 0;
        for(const auto& good : o.gs.world->goods) {
            const auto good_col = Eng3D::Color::rgb8((uint8_t)(i * 12), (uint8_t)(i * 31), (uint8_t)(i * 97));
            const auto& product = o.province.products[o.gs.world->get_id(good)];
            goods_data.push_back(UI::ChartData(std::max<float>(product.demand, 0.1f), good.name.get_string(), good_col));
            i++;
        }
        o.products_pie->set_data(goods_data);
    });
    this->on_each_tick(*this);

    // Initial product info
    auto* flex_column = new UI::Div(0, 128, this->width, this->height - 128, this);
    flex_column->flex = UI::Flex::COLUMN;
    for(auto& good : gs.world->goods) {
        auto* info = new ProductInfo(this->gs, 0, 0, province, good, flex_column);
        product_infos.push_back(info);
    }
}

ProvinceBuildingTab::ProvinceBuildingTab(GameState& _gs, int x, int y, Province& _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province }
{
    std::vector<int> sizes{ 96, 64, 64, 32, 48, 32 };
    std::vector<std::string> header{ "Name", "Workers", "Inputs", "Output", "Scale", " " };
    auto* table = new UI::Table<uint32_t>(0, 0, 0, this->height, 32, sizes, header, this);
    table->reserve(this->province.buildings.size());
    table->set_on_each_tick([this, table](UI::Widget&) {
        for(size_t i = 0; i < this->province.buildings.size(); i++) {
            const auto& building = this->province.buildings[i];
            const auto& type = this->gs.world->building_types[i];
            auto* row = table->get_row(i);
            size_t row_index = 0;

            auto* name = row->get_element(row_index++);
            auto name_str = Eng3D::string_format("%s", type.name.get_string().c_str());
            name->text(name_str);
            name->set_key(name_str);

            auto* workers = row->get_element(row_index++);
            workers->text(Eng3D::string_format("%.0f", building.workers));
            workers->set_key(building.workers);

            auto* inputs = row->get_element(row_index++);
            inputs->set_key(type.inputs.size());
            inputs->kill_children();
            inputs->flex = UI::Flex::ROW;
            for(auto good : type.inputs) {
                auto* input_good_image = new UI::Image(0, 0, 32, 32, "gfx/good/" + good->ref_name + ".png", true, inputs);
                input_good_image->set_tooltip(good->name.get_string());
            }

            auto* outputs = row->get_element(row_index++);
            outputs->set_key(type.output != nullptr ? 1 : 0);
            outputs->kill_children();
            outputs->flex = UI::Flex::ROW;
            if(type.output != nullptr) {
                auto* output_good_image = new UI::Image(0, 0, 32, 32, "gfx/good/" + type.output->ref_name + ".png", true, outputs);
                output_good_image->set_tooltip(type.output->name.get_string());
            }

            auto* scale = row->get_element(row_index++);
            auto scale_str = Eng3D::string_format("%.0f", building.production_scale * building.level);
            scale->text(scale_str);
            scale->set_key(building.production_scale * building.level);
            scale->set_tooltip(Eng3D::string_format(_("Allowed production scale, (scale * level) = (%.0f * %.0f) = %.0f"), building.production_scale, building.level, building.production_scale * building.level));

            auto* upgrade = row->get_element(row_index++);
            upgrade->text("+");
            upgrade->set_tooltip(_("Upgrade building"));
            upgrade->set_key(0);
            upgrade->set_on_click([this, type](UI::Widget&) {
                this->gs.client->send(Action::BuildingAdd::form_packet(this->province, type));
            });
        }
    });
    table->on_each_tick(*table);
}

ProvinceEditLanguageTab::ProvinceEditLanguageTab(GameState& _gs, int x, int y, Province& _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province },
    language{ _gs.world->languages[0] },
    religion{ _gs.world->religions[0] }
{
    this->is_scroll = false;
    auto* language_flex_column = new UI::Div(0, 0, this->width / 2, this->height, this);
    language_flex_column->flex = UI::Flex::COLUMN;
    language_flex_column->is_scroll = true;
    for(auto& language : gs.world->languages) {
        auto* btn = new UI::Button(0, 0, 128, 24, language_flex_column);
        btn->text(language.name.get_string());
        btn->set_on_click([this, &language](UI::Widget&) {
            const_cast<Province&>(this->province).languages[language.get_id()] = 1.f;
            this->gs.map->update_mapmode();
            this->gs.input.selected_language = &language;
        });
    }

    auto* religion_flex_column = new UI::Div(this->width / 2, 0, this->width - (this->width / 2), this->height, this);
    religion_flex_column->flex = UI::Flex::COLUMN;
    religion_flex_column->is_scroll = true;
    for(auto& religion : gs.world->religions) {
        auto* btn = new UI::Button(0, 0, 128, 24, religion_flex_column);
        btn->text(religion.name.get_string());
        btn->set_on_click([this, &religion](UI::Widget&) {
            const_cast<Province&>(this->province).religions[religion.get_id()] = 1.f;
            this->gs.map->update_mapmode();
            this->gs.input.selected_religion = &religion;
        });
    }
}

ProvinceEditTerrainTab::ProvinceEditTerrainTab(GameState& _gs, int x, int y, Province& _province, UI::Widget* _parent)
    : UI::Group(x, y, _parent->width - x, _parent->height - y, _parent),
    gs{ _gs },
    province{ _province },
    terrain_type{ _gs.world->terrain_types[0] }
{
    std::vector<int> sizes{ 96, 128 };
    std::vector<std::string> header{ "Landscape", "Name" };
    auto table = new UI::Table<uint32_t>(0, 0, 0, this->height, 30, sizes, header, this);
    table->reserve(gs.world->terrain_types.size());
    table->set_on_each_tick([this, table](Widget&) {
        for(auto& terrain_type : this->gs.world->terrain_types) {
            auto* row = table->get_row(terrain_type.get_id());
            size_t row_index = 0;

            auto landscape = row->get_element(row_index++);
            auto landscape_icon = this->gs.tex_man.load(gs.package_man.get_unique("gfx/terraintype/" + terrain_type.ref_name + ".png"));
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
                auto& nc_province = const_cast<Province&>(this->province);
                nc_province.terrain_type_id = terrain_type.get_id();
                if(terrain_type.is_water_body) {
                    nc_province.pops.clear();
                    nc_province.nuclei.clear();
                    nc_province.controller_id = Nation::invalid();
                    nc_province.owner_id = Nation::invalid();
                    for(auto& building : nc_province.buildings)
                        building.level = 0;
                }

                this->gs.map->update_mapmode();
            });
        }
    });
    table->on_each_tick(*table);
}

ProvinceView::ProvinceView(GameState& _gs, Province& _province)
    : UI::Window(-400, 0, 400, _gs.height),
    gs{ _gs },
    province{ _province }
{
    if(this->gs.right_side_panel != nullptr)
        this->gs.right_side_panel->kill();
    this->gs.right_side_panel = this;
    this->set_close_btn_function([this](Widget&) {
        this->kill();
        this->gs.right_side_panel = nullptr;
        this->gs.map->set_selected_province(false, 0);
    });
    this->gs.map->set_selected_province(true, this->province.get_id());

    this->origin = UI::Origin::UPPER_RIGHT_SCREEN;
    this->is_scroll = false;
    this->text(province.name.get_string());

    auto* flex_row = new UI::Div(0, 0, this->width, 32, this);
    flex_row->flex = UI::Flex::ROW;

    this->pop_tab = new ProvincePopulationTab(gs, 0, 32, province, this);
    this->pop_tab->is_render = true;
    auto* pop_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/pv_1.png")), flex_row);
    pop_ibtn->set_on_click([this](UI::Widget&) {
        this->pop_tab->is_render = true;
        this->econ_tab->is_render = false;
        this->build_tab->is_render = false;
        if(gs.editor) {
            this->edit_language_tab->is_render = false;
            this->edit_terrain_tab->is_render = false;
        }
    });
    pop_ibtn->set_tooltip(_("Population"));

    this->econ_tab = new ProvinceEconomyTab(gs, 0, 32, province, this);
    this->econ_tab->is_render = false;
    auto* econ_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/money.png")), flex_row);
    econ_ibtn->set_on_click([this](UI::Widget&) {
        this->pop_tab->is_render = false;
        this->econ_tab->is_render = true;
        this->build_tab->is_render = false;
        if(gs.editor) {
            this->edit_language_tab->is_render = false;
            this->edit_terrain_tab->is_render = false;
        }
    });
    econ_ibtn->set_tooltip(_("Economy"));

    this->build_tab = new ProvinceBuildingTab(gs, 0, 32, province, this);
    this->build_tab->is_render = false;
    auto* build_ibtn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/pv_0.png")), flex_row);
    build_ibtn->set_on_click([this](UI::Widget&) {
        this->pop_tab->is_render = false;
        this->econ_tab->is_render = false;
        this->build_tab->is_render = true;
        if(gs.editor) {
            this->edit_language_tab->is_render = false;
            this->edit_terrain_tab->is_render = false;
        }
    });
    build_ibtn->set_tooltip(_("Buildings"));

    if(gs.editor) {
        auto* fill_pops_btn = new UI::Button(0, this->height - 64, 32, 32, flex_row);
        fill_pops_btn->set_on_click([this](UI::Widget&) {
            // Get max sv
            auto max_sv = 1.f;
            for(const auto& pop_type : this->gs.world->pop_types) {
                if(pop_type.social_value > max_sv)
                    max_sv = pop_type.social_value;
            }

            if(this->gs.input.selected_language == nullptr)
                this->gs.input.selected_language = &this->gs.world->languages[0];
            if(this->gs.input.selected_religion == nullptr)
                this->gs.input.selected_religion = &this->gs.world->religions[0];

            for(auto& pop_type : this->gs.world->pop_types) {
                Pop pop;
                pop.type_id = pop_type.get_id();
                pop.size = 1000.f / std::max<float>(0.01f, pop_type.social_value);
                pop.literacy = max_sv / std::max<float>(0.01f, pop_type.social_value);
                pop.budget = pop.size * 100.f * max_sv;
                const_cast<Province&>(this->province).pops.push_back(pop);
            }
            this->gs.map->update_mapmode();
        });
        fill_pops_btn->set_tooltip("Add POPs (will add " + std::to_string(gs.world->pop_types.size()) + "POPs)");

        this->edit_language_tab = new ProvinceEditLanguageTab(gs, 0, 32, province, this);
        this->edit_language_tab->is_render = false;
        auto* edit_language_btn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/money.png")), flex_row);
        edit_language_btn->set_on_click([this](UI::Widget&) {
            this->pop_tab->is_render = false;
            this->econ_tab->is_render = false;
            this->build_tab->is_render = false;
            this->edit_language_tab->is_render = true;
            this->edit_terrain_tab->is_render = false;
        });
        edit_language_btn->set_tooltip(_("Edit primary language and religion"));

        this->edit_terrain_tab = new ProvinceEditTerrainTab(gs, 0, 32, province, this);
        this->edit_terrain_tab->is_render = false;
        auto* edit_terrain_btn = new UI::Image(0, 0, 32, 32, gs.tex_man.load(gs.package_man.get_unique("gfx/money.png")), flex_row);
        edit_terrain_btn->set_on_click([this](UI::Widget&) {
            this->pop_tab->is_render = false;
            this->econ_tab->is_render = false;
            this->build_tab->is_render = false;
            this->edit_language_tab->is_render = false;
            this->edit_terrain_tab->is_render = true;
        });
        edit_terrain_btn->set_tooltip(_("Edit terrain"));

        rename_inp = new UI::Input(0, 0, 128, 24, flex_row);
        rename_inp->set_buffer(province.name.get_string());
        auto* xchg_name_btn = new UI::Button(0, 0, 32, 32, flex_row);
        xchg_name_btn->set_on_click([this](UI::Widget&) {
            const_cast<Province&>(this->province).name = this->rename_inp->get_buffer();
            this->gs.map->create_labels();
        });
        xchg_name_btn->set_tooltip("Rename province");

        density_sld = new UI::Slider(0, 0, 128, 24, 0.1f, 2.f, flex_row);
        density_sld->set_value(0.f);
        density_sld->set_on_click([this](UI::Widget& w) {
            w.text(std::to_string(((UI::Slider&)w).get_value()));
            const float den = this->density_sld->value;
            for(auto& pop : const_cast<Province&>(this->province).pops)
                pop.size *= den;
            this->gs.map->update_mapmode();
        });
    }
}

ProvinceView::~ProvinceView() {
    gs.map->set_selected_province(true, 0);
}