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
//      client/interface/province_view.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/texture.hpp"
#include "eng3d/string.hpp"
#include "eng3d/ui/piechart.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/table.hpp"
#include "eng3d/ui/close_button.hpp"

#include "client/interface/industry_window.hpp"
#include "client/interface/province_view.hpp"
#include "client/interface/nation_view.hpp"
#include "client/game_state.hpp"
#include "client/map.hpp"
#include "client/map_render.hpp"
#include "world.hpp"
#include "world.hpp"
#include "world.hpp"
#include "action.hpp"
#include "client/client_network.hpp"

using namespace Interface;

void ProvincePopulationTab::update_piecharts() {
    // Obtain population information
    std::vector<UI::ChartData> languages_data, religions_data, pop_types_data;
    for(const auto& language : gs.world->languages) {
        const auto size = province.languages[language] * province.total_pops();
        languages_data.emplace_back(size, language.name, Eng3D::Color::bgr32(language.color));
    }
    languages_pie->set_data(languages_data);

    for(const auto& religion : gs.world->religions) {
        const auto size = province.religions[religion] * province.total_pops();
        religions_data.emplace_back(size, religion.name, Eng3D::Color::bgr32(religion.color));
    }
    religions_pie->set_data(religions_data);

    std::vector<size_t> pop_type_sizes(gs.world->pop_types.size(), 0);
    for(const auto& pop : province.pops)
        pop_type_sizes[pop.type_id] += pop.size;
    for(const auto& pop_type : gs.world->pop_types) {
        const auto i = static_cast<size_t>(pop_type.get_id());
        const auto color = Eng3D::Color(
            static_cast<uint8_t>((i * 3) % 256),
            static_cast<uint8_t>((i * 7) % 256),
            static_cast<uint8_t>((i * 15) % 256)
        );
        pop_types_data.push_back(UI::ChartData(pop_type_sizes[gs.world->get_id(pop_type)], pop_type.name, color));
    }
    pop_types_pie->set_data(pop_types_data);
}

UI::Widget& ProvincePopulationTab::create_pop_table() {
    std::vector<int> sizes{ 64, 96 };
    std::vector<std::string> header{ "Size", "Budget" };
    if(gs.editor) {
        sizes.push_back(32);
        header.push_back(" ");
    }

    auto& pop_table = this->make_widget<UI::Table<uint32_t>>(0, 352, this->height - (352 + 32), 30, sizes, header);
    pop_table.reserve(this->province.pops.size());
    pop_table.set_on_each_tick([this, &pop_table](UI::Widget&) {
        for(size_t i = 0; i < this->province.pops.size(); i++) {
            auto& pop = this->province.pops[i];
            auto& row = pop_table.get_row(i);
            size_t row_index = 0;

            auto* size = row.get_element(row_index++);
            auto size_str = string_format("%.0f", pop.size);
            size->set_text(size_str);
            size->set_key(pop.size);

            auto* budget = row.get_element(row_index++);
            auto budget_str = string_format("%.2f", pop.budget / pop.size);
            budget->set_text(budget_str);
            budget->set_tooltip(Eng3D::translate_format("Total budget: %.2f\nPublic debt: %.2f\nPrivate debt: %.2f", pop.budget, pop.public_debt, pop.private_debt));
            budget->set_key(pop.budget / pop.size);

            if(this->gs.editor) {
                auto* remove_btn = row.get_element(row_index++);
                auto remove_btn_str = "X";
                remove_btn->set_text(remove_btn_str);
                remove_btn->set_key(remove_btn_str);
                remove_btn->set_on_click([this, i, &pop_table](UI::Widget&) {
                    pop_table.remove_row(i);
                    const_cast<Province&>(this->province).pops[i].size = 0.f;
                    pop_table.on_each_tick(pop_table);
                });
            }
        }
    });
    pop_table.on_each_tick(pop_table);
    return pop_table;
}

UI::Widget& ProvincePopulationTab::create_stock_table() {
    std::vector<int> sizes{ 100, 100, 100, 100, 100 };
    std::vector<std::string> header{ "Commodity", "Supply", "Demand", "Global demand", "Price" };
    auto& stock_table = this->make_widget<UI::Table<uint32_t>>(0, 352, this->height - (352 + 32), 30, sizes, header);
    stock_table.reserve(this->province.pops.size());
    stock_table.set_on_each_tick([this, &stock_table](UI::Widget&) {
        for(const auto& commodity : this->gs.world->commodities) {
            auto& product = this->province.products[commodity];
            auto& row = stock_table.get_row(commodity.get_id());
            size_t row_index = 0;

            auto* commodity_row = row.get_element(row_index++);
            commodity_row->set_key(commodity.name.data());
            auto& commodity_img = commodity_row->make_widget<UI::Image>(0, 0, 35, 35, commodity.get_icon_path(), true);
            commodity_img.set_tooltip(commodity.name.data());

            auto* amount = row.get_element(row_index++);
            amount->set_key(product.supply, "%.0f");

            auto* demand = row.get_element(row_index++);
            demand->set_key(product.demand, "%.0f");

            auto* global_demand = row.get_element(row_index++);
            global_demand->set_key(product.global_demand, "%.0f");

            auto* price = row.get_element(row_index++);
            price->set_key(product.price, "%.2f");
        }
    });
    stock_table.on_each_tick(stock_table);
    return stock_table;
}

ProvincePopulationTab::ProvincePopulationTab(GameState& _gs, int _x, int _y, Province& _province, UI::Widget* _parent)
    : UI::Group(_x, _y, _parent->width - _x, _parent->height - _y, _parent),
    gs{ _gs },
    province{ _province }
{
    this->is_scroll = true;
    this->set_text(province.name.data());

    const auto& terrain_type = gs.world->terrain_types[province.terrain_type_id];
    auto& landscape_img = this->make_widget<UI::Image>(0, 0, this->width - 16, 128 + 64 + 16, terrain_type.get_icon_path());
    landscape_img.set_tooltip(translate_format("%s, penalty %.2f", terrain_type.name.data(), terrain_type.penalty));
    
    auto& owner_flag = this->make_widget<UI::AspectImage>(0, 0, 96, 48, gs.get_nation_flag(gs.world->nations[this->province.owner_id]));
    owner_flag.set_on_click([this](UI::Widget&) {
        new Interface::NationView(this->gs, gs.world->nations[this->province.owner_id]);
    });
    owner_flag.set_tooltip(translate_format("%s owns this province", gs.world->nations[this->province.owner_id].name.data()));
    //this->make_widget<UI::Image>(owner_flag->x, owner_flag->y, owner_flag->width, owner_flag->height, "gfx/flag_rug.png");

    // Display all the nuclei
    auto& nuclei_flex_row = this->make_widget<UI::Div>(0, landscape_img.height - 24, this->width, 24);
    nuclei_flex_row.flex = UI::Flex::ROW;
    for(const auto nucleus_id : province.nuclei) {
        auto& nucleus = this->gs.world->nations[nucleus_id];
        auto& owner_flag = nuclei_flex_row.make_widget<UI::AspectImage>(0, 0, 32, 24, gs.get_nation_flag(nucleus));
        owner_flag.set_on_click([this, nucleus_id](UI::Widget&) {
            new Interface::NationView(this->gs, this->gs.world->nations[nucleus_id]);
        });
        owner_flag.set_tooltip(translate_format("%s has claims on this province", nucleus.name.data()));
    }

    auto& pop_table = create_pop_table();
    auto& stock_table = create_stock_table();
    stock_table.right_side_of(pop_table);

    auto& languages_lab = this->make_widget<UI::Label>(0, 0, "Languages");
    languages_lab.below_of(landscape_img);
    this->languages_pie = &this->make_widget<UI::PieChart>(0, 0, 96, 96);
    this->languages_pie->below_of(languages_lab);

    auto& religions_lab = this->make_widget<UI::Label>(0, 0, "Religions");
    religions_lab.below_of(landscape_img);
    religions_lab.right_side_of(*this->languages_pie);
    this->religions_pie = &this->make_widget<UI::PieChart>(0, 0, 96, 96);
    this->religions_pie->below_of(religions_lab);
    this->religions_pie->right_side_of(*this->languages_pie);

    auto& pop_types_lab = this->make_widget<UI::Label>(0, 0, "Professions");
    pop_types_lab.below_of(landscape_img);
    pop_types_lab.right_side_of(*this->religions_pie);
    this->pop_types_pie = &this->make_widget<UI::PieChart>(0, 0, 96, 96);
    this->pop_types_pie->below_of(pop_types_lab);
    this->pop_types_pie->right_side_of(*this->religions_pie);

    this->set_on_each_tick([this](UI::Widget&) {
        this->update_piecharts();
    });
    this->on_each_tick(*this);
}

ProvinceEconomyTab::ProvinceEconomyTab(GameState& _gs, int _x, int _y, Province& _province, UI::Widget* _parent)
    : UI::Group(_x, _y, _parent->width - _x, _parent->height - _y, _parent),
    gs{ _gs },
    province{ _province }
{
    this->set_text(province.name.data());
    this->is_scroll = true;
}

ProvinceBuildingTab::ProvinceBuildingTab(GameState& _gs, int _x, int _y, Province& _province, UI::Widget* _parent)
    : UI::Group(_x, _y, _parent->width - _x, _parent->height - _y, _parent),
    gs{ _gs },
    province{ _province }
{
    Interface::IndustryWindow::new_table(gs, 0, 0, 0, this->height, { province.get_id() }, this);
}

ProvinceEditLanguageTab::ProvinceEditLanguageTab(GameState& _gs, int _x, int _y, Province& _province, UI::Widget* _parent)
    : UI::Group(_x, _y, _parent->width - _x, _parent->height - _y, _parent),
    gs{ _gs },
    province{ _province },
    language{ _gs.world->languages[0] },
    religion{ _gs.world->religions[0] }
{
    this->is_scroll = false;

    std::vector<int> sizes{ 96, 128 };
    std::vector<std::string> header{ "Language", "Religion" };
    auto& table = this->make_widget<UI::Table<uint32_t>>(0, 0, this->height, 30, sizes, header);
    table.reserve(this->gs.world->languages.size());
    table.set_on_each_tick([this, &table](Widget&) {
        for(size_t i = 0; i < this->gs.world->religions.size() || i < this->gs.world->languages.size(); i++) {
            auto& row = table.get_row(i);
            size_t row_index = 0;

            auto& row_religion = i >= this->gs.world->religions.size() ? this->gs.world->religions[0] : this->gs.world->religions[i];
            auto* religion_icon = row.get_element(row_index++);
            religion_icon->current_texture = this->gs.tex_man.load(gs.package_man.get_unique(row_religion.get_icon_path()));
            religion_icon->set_tooltip(row_religion.name.data());
            religion_icon->set_key(row_religion.name.data());
            religion_icon->set_on_click([this, religion_id = row_religion.get_id()](UI::Widget&) {
                const_cast<Province&>(this->province).religions[religion_id] = 1.f;
                this->gs.map->update_mapmode();
                this->gs.input.selected_religion = &this->gs.world->religions[religion_id];
            });

            auto& row_language = i >= this->gs.world->languages.size() ? this->gs.world->languages[0] : this->gs.world->languages[i];
            auto* name = row.get_element(row_index++);
            name->set_text(row_language.name.data());
            name->set_tooltip(row_language.name.data());
            name->set_key(row_language.name.data());
            name->set_on_click([this, language_id = row_language.get_id()](UI::Widget&) {
                const_cast<Province&>(this->province).languages[language_id] = 1.f;
                this->gs.map->update_mapmode();
                this->gs.input.selected_language = &this->gs.world->languages[language_id];
            });
        }
    });
    table.on_each_tick(table);
}

ProvinceEditTerrainTab::ProvinceEditTerrainTab(GameState& _gs, int _x, int _y, Province& _province, UI::Widget* _parent)
    : UI::Group(_x, _y, _parent->width - _x, _parent->height - _y, _parent),
    gs{ _gs },
    province{ _province },
    terrain_type{ _gs.world->terrain_types[0] }
{
    std::vector<int> sizes{ 96, 128 };
    std::vector<std::string> header{ "Landscape", "Name" };
    auto& table = this->make_widget<UI::Table<uint32_t>>(0, 0, this->height, 30, sizes, header);
    table.reserve(gs.world->terrain_types.size());
    table.set_on_each_tick([this, &table](Widget&) {
        for(auto& terrain_type_row : this->gs.world->terrain_types) {
            auto& row = table.get_row(terrain_type_row);
            size_t row_index = 0;

            auto landscape = row.get_element(row_index++);
            auto landscape_icon = gs.tex_man.load(gs.package_man.get_unique(terrain_type_row.get_icon_path()));
            landscape->current_texture = landscape_icon;
            landscape->set_tooltip(terrain_type_row.name.data());
            landscape->set_key(terrain_type_row.name.data());

            auto name = row.get_element(row_index++);
            name->set_text(terrain_type_row.name.data());
            name->set_tooltip(terrain_type_row.name.data());
            name->set_key(terrain_type_row.name.data());
            name->set_on_click([this, &terrain_type_row](UI::Widget&) {
                auto& nc_province = const_cast<Province&>(this->province);
                nc_province.terrain_type_id = terrain_type_row;
                if(terrain_type_row.is_water_body) {
                    nc_province.unpopulate();
                    nc_province.nuclei.clear();
                    for(auto& building : nc_province.buildings)
                        building.level = 0;
                }

                this->gs.map->update_mapmode();
            });
        }
    });
    table.on_each_tick(table);
}

ProvinceView::ProvinceView(GameState& _gs, Province& _province)
    : UI::Window(-400, -400, 800, 800),
    gs{ _gs },
    province{ _province }
{
    this->set_close_btn_function([this](Widget&) {
        this->kill();
        this->gs.map->set_selected_province(false, ProvinceId(-1));
    });
    this->gs.map->set_selected_province(true, this->province.get_id());

    this->origin = UI::Origin::CENTER_SCREEN;
    this->is_scroll = false;
    this->set_text(province.name.data());

    auto& flex_row = this->make_widget<UI::Div>(0, 0, this->width, 32);
    flex_row.flex = UI::Flex::ROW;
    if(gs.editor)
        flex_row.height += 32;

    this->pop_tab = &this->make_widget<ProvincePopulationTab>(gs, 0, flex_row.height, province);
    this->pop_tab->is_render = true;
    auto& pop_ibtn = flex_row.make_widget<UI::Image>(0, 0, 32, 32, "gfx/pv_1.png");
    pop_ibtn.set_on_click([this](UI::Widget&) {
        this->pop_tab->is_render = true;
        this->econ_tab->is_render = false;
        this->build_tab->is_render = false;
        if(gs.editor) {
            this->edit_language_tab->is_render = false;
            this->edit_terrain_tab->is_render = false;
        }
    });
    pop_ibtn.set_tooltip(translate("Population"));

    this->econ_tab = &this->make_widget<ProvinceEconomyTab>(gs, 0, flex_row.height, province);
    this->econ_tab->is_render = false;
    auto& econ_ibtn = flex_row.make_widget<UI::Image>(0, 0, 32, 32, "gfx/money.png");
    econ_ibtn.set_on_click([this](UI::Widget&) {
        this->pop_tab->is_render = false;
        this->econ_tab->is_render = true;
        this->build_tab->is_render = false;
        if(gs.editor) {
            this->edit_language_tab->is_render = false;
            this->edit_terrain_tab->is_render = false;
        }
    });
    econ_ibtn.set_tooltip(translate("Economy"));

    this->build_tab = &this->make_widget<ProvinceBuildingTab>(gs, 0, flex_row.height, province);
    this->build_tab->is_render = false;
    auto& build_ibtn = flex_row.make_widget<UI::Image>(0, 0, 32, 32, "gfx/pv_0.png");
    build_ibtn.set_on_click([this](UI::Widget&) {
        this->pop_tab->is_render = false;
        this->econ_tab->is_render = false;
        this->build_tab->is_render = true;
        if(gs.editor) {
            this->edit_language_tab->is_render = false;
            this->edit_terrain_tab->is_render = false;
        }
    });
    build_ibtn.set_tooltip(translate("Buildings"));

    if(gs.editor) {
        auto& fill_pops_btn = flex_row.make_widget<UI::Button>(0, this->height - 64, 32, 32);
        fill_pops_btn.set_on_click([this](UI::Widget&) {
            // Get max sv
            auto max_sv = 1.f;
            for(const auto& pop_type : this->gs.world->pop_types)
                if(pop_type.social_value > max_sv)
                    max_sv = pop_type.social_value;

            if(this->gs.input.selected_language == nullptr)
                this->gs.input.selected_language = &this->gs.world->languages[0];
            if(this->gs.input.selected_religion == nullptr)
                this->gs.input.selected_religion = &this->gs.world->religions[0];

            for(const auto& pop_type : this->gs.world->pop_types) {
                auto& pop = province.pops[pop_type];
                pop.type_id = pop_type.get_id();
                pop.size = 1000.f;
                pop.literacy = 0.5;
                pop.budget = pop.size;
            }
            this->gs.map->update_mapmode();
            this->gs.update_tick = true;
        });
        fill_pops_btn.set_tooltip(translate_format("Add POPs (will add %zu POPs)", gs.world->pop_types.size()));

        this->edit_language_tab = &this->make_widget<ProvinceEditLanguageTab>(gs, 0, 32, province);
        this->edit_language_tab->is_render = false;
        auto& edit_language_btn = flex_row.make_widget<UI::Image>(0, 0, 32, 32, "gfx/money.png");
        edit_language_btn.set_on_click([this](UI::Widget&) {
            this->pop_tab->is_render = false;
            this->econ_tab->is_render = false;
            this->build_tab->is_render = false;
            this->edit_language_tab->is_render = true;
            this->edit_terrain_tab->is_render = false;
        });
        edit_language_btn.set_tooltip(translate("Edit primary language and religion"));

        this->edit_terrain_tab = &this->make_widget<ProvinceEditTerrainTab>(gs, 0, 32, province);
        this->edit_terrain_tab->is_render = false;
        auto& edit_terrain_btn = flex_row.make_widget<UI::Image>(0, 0, 32, 32, "gfx/money.png");
        edit_terrain_btn.set_on_click([this](UI::Widget&) {
            this->pop_tab->is_render = false;
            this->econ_tab->is_render = false;
            this->build_tab->is_render = false;
            this->edit_language_tab->is_render = false;
            this->edit_terrain_tab->is_render = true;
        });
        edit_terrain_btn.set_tooltip(translate("Edit terrain"));

        auto& density_sld = this->make_widget<UI::Slider>(0, 32, 128, 24, 0.1f, 2.f);
        density_sld.set_value(0.f);
        density_sld.set_on_click([this](UI::Widget& w) {
            auto& o = static_cast<UI::Slider&>(w);
            w.set_text(string_format("%.2f", o.get_value()));
            const auto den = o.get_value();
            for(auto& pop : const_cast<Province&>(this->province).pops)
                pop.size *= den;
            this->gs.map->update_mapmode();
            this->gs.map->map_render->request_update_visibility();
            this->gs.update_tick = true;
        });
        density_sld.set_tooltip(translate("Density slider"));

        auto& rename_inp = this->make_widget<UI::Input>(128, 32, 128, 24);
        rename_inp.set_buffer(province.name.data());
        auto& xchg_name_btn = this->make_widget<UI::Button>(128 + 128, 32, 32, 32);
        xchg_name_btn.set_on_click([this, &rename_inp](UI::Widget&) {
            const_cast<Province&>(this->province).name = Eng3D::StringRef{rename_inp.get_buffer()};
            this->gs.map->create_labels();
            this->gs.update_tick = true;
        });
        xchg_name_btn.set_tooltip("Rename province");
    }
}

ProvinceView::~ProvinceView() {
    gs.map->set_selected_province(true, ProvinceId(0));
}
