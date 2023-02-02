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
//      client/interface/minimap.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <unordered_map>
#include "eng3d/string.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/close_button.hpp"
#include "eng3d/ui/image.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/div.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/table.hpp"

#include "client/interface/minimap.hpp"
#include "client/map.hpp"
#include "world.hpp"
#include "server/ai.hpp"
#include "server/economy.hpp"

using namespace Interface;

const Eng3D::Color MAPMODE_DEFAULT_MIN_COL = Eng3D::Color::rgb8(255, 0, 0);
const Eng3D::Color MAPMODE_DEFAULT_MAX_COL = Eng3D::Color::rgb8(16, 255, 128);
mapmode_generator generic_map_mode(
    std::function<float(const World&, ProvinceId)> province_value,
    Eng3D::Color min_col = MAPMODE_DEFAULT_MIN_COL,
    Eng3D::Color max_col = MAPMODE_DEFAULT_MAX_COL);
std::vector<ProvinceColor> terrain_map_mode(const World& world);
mapmode_tooltip commodity_tooltip(CommodityId id);
mapmode_generator commodity_map_mode(CommodityId id);
mapmode_generator relations_map_mode(NationId id);
mapmode_tooltip relations_tooltip(NationId id);
mapmode_generator trade_map_mode(ProvinceId id);
mapmode_tooltip trade_tooltip(ProvinceId id);
std::vector<ProvinceColor> religion_map_mode(const World& world);
std::string religion_tooltip(const World& world, const ProvinceId id);
std::vector<ProvinceColor> language_map_mode(const World& world);
std::string language_tooltip(const World& world, const ProvinceId id);
std::vector<ProvinceColor> population_map_mode(const World& world);
std::string population_tooltip(const World& world, const ProvinceId id);

Minimap::Minimap(GameState& _gs, int _x, int _y, UI::Origin _origin)
    : UI::Window(_x, _y, 400, 200),
    gs{ _gs }
{
    this->origin = _origin;
    this->is_pinned = true;
    this->is_scroll = false;
    this->padding = glm::ivec2(0, 24);

    auto& flex_column1 = this->make_widget<UI::Div>(5, 5, 24, 190);
    flex_column1.flex = UI::Flex::COLUMN;
    flex_column1.flex_align = UI::Align::CENTER;
    auto& flex_column2 = this->make_widget<UI::Div>(35, 5, 24, 190);
    flex_column2.flex = UI::Flex::COLUMN;
    flex_column2.flex_align = UI::Align::CENTER;

    auto& flat_btn = flex_column1.make_widget<UI::Image>(0, 0, 24, 24, "gfx/flat_icon.png");
    flat_btn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_view(MapView::PLANE_VIEW);
    });
    flat_btn.set_tooltip("Flat map");

    auto& globe_btn = flex_column2.make_widget<UI::Image>(0, 0, 24, 24, "gfx/globe_icon.png");
    globe_btn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_view(MapView::SPHERE_VIEW);
    });
    globe_btn.set_tooltip("Globe map");

    auto& landscape_ibtn = flex_column1.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    landscape_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        this->gs.map->set_map_mode(terrain_map_mode, empty_province_tooltip);
        set_mapmode_options(nullptr);
    });
    landscape_ibtn.set_tooltip("Terrain");

    auto& political_ibtn = flex_column1.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    political_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        this->gs.map->set_map_mode(political_map_mode, political_province_tooltip);
        set_mapmode_options(nullptr);
    });
    political_ibtn.set_tooltip("Political");

    auto& relations_ibtn = flex_column1.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    relations_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection([](const World&, Map& map, const Province& province) {
            NationId nation_id = province.owner_id;
            mapmode_generator map_mode = relations_map_mode(nation_id);
            mapmode_tooltip map_tooltip = relations_tooltip(nation_id);
            map.set_map_mode(map_mode, map_tooltip);
        });
        const auto nation_id = this->gs.curr_nation->get_id();
        mapmode_generator map_mode = relations_map_mode(nation_id);
        mapmode_tooltip map_tooltip = relations_tooltip(nation_id);
        this->gs.map->set_map_mode(map_mode, map_tooltip);
        set_mapmode_options(nullptr);
    });
    relations_ibtn.set_tooltip("Relations");

    auto& transport_cost_ibtn = flex_column1.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    transport_cost_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection([](const World&, Map& map, const Province& selected_province) {
            map.set_map_mode(trade_map_mode(selected_province), trade_tooltip(selected_province));
        });
        set_mapmode_options(nullptr);
    });
    transport_cost_ibtn.set_tooltip("Transport cost");

    auto& population_ibtn = flex_column1.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    population_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        this->gs.map->set_map_mode(population_map_mode, population_tooltip);
        set_mapmode_options(nullptr);
    });
    population_ibtn.set_tooltip("Population");

    auto& debt_and_loans_ibtn = flex_column1.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    debt_and_loans_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        this->gs.map->set_map_mode([](const World& world) {
            auto max_amount = 0.f;
            for(const auto& province : world.provinces) {
                max_amount = glm::max(1.f, max_amount);
            }

            // Mix each color depending of how many live there compared to max_amount
            auto min = Eng3D::Color::rgb8(128, 128, 255);
            auto max = Eng3D::Color::rgb8(255, 64, 64);
            std::vector<ProvinceColor> province_color;
            for(const auto& province : world.provinces) {
                auto ratio = 1.f / max_amount;
                province_color.emplace_back(province.get_id(), Eng3D::Color::lerp(min, max, ratio));
            }
            return province_color;
        },
        [](const World& world, const ProvinceId id) -> std::string {
            return translate_format("debt");
        });
        set_mapmode_options(nullptr);
    });
    debt_and_loans_ibtn.set_tooltip("Debt and loans");

    auto& terrain_color_ibtn = flex_column2.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    terrain_color_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = [](const World& world) {
            std::vector<ProvinceColor> province_color;
            for(unsigned int i = 0; i < world.provinces.size(); i++) {
                const auto& province = world.provinces[i];
                Eng3D::Color color = Eng3D::Color::bgr32(world.terrain_types[province.terrain_type_id].color);
                province_color.emplace_back(ProvinceId(i), color);
            }
            province_color.emplace_back(ProvinceId(-2), Eng3D::Color{}); // Water
            province_color.emplace_back(ProvinceId(-1), Eng3D::Color{}); // Land
            return province_color;
        };
        mapmode_tooltip map_tooltip = [](const World& world, const ProvinceId id) -> std::string {
            return world.terrain_types[world.provinces[id].terrain_type_id].name;
        };
        this->gs.map->set_map_mode(map_mode, map_tooltip);
        set_mapmode_options(nullptr);
    });
    terrain_color_ibtn.set_tooltip("Simple terrain");

    auto& language_ibtn = flex_column2.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    language_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        // this->gs.map->set_map_mode(map_mode, map_tooltip);
        this->gs.map->set_map_mode(language_map_mode, language_tooltip);
        set_mapmode_options(nullptr);
    });
    language_ibtn.set_tooltip("Language diversity");

    auto& religion_ibtn = flex_column2.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    religion_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        this->gs.map->set_map_mode(religion_map_mode, religion_tooltip);
        set_mapmode_options(nullptr);
    });
    religion_ibtn.set_tooltip("Religion");

    auto& commodity_price_ibtn = flex_column2.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    commodity_price_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        set_mapmode_options(new MapmodeCommodityOptions(this->gs));
    });
    commodity_price_ibtn.set_tooltip("Commodity");

    auto& militancy_ibtn = flex_column2.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    militancy_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection([](const World&, Map& map, const Province& selected_province) {
            map.set_map_mode(([](ProvinceId id) {
                return [id](const World& world) {
                    const auto& province = world.provinces[id];
                    const auto& nation = world.nations[province.owner_id];
                    const auto& ai = ai_man[nation];

                    auto max_amount = 0.f;
                    for(const auto& province : world.provinces)
                        max_amount = glm::max(ai.potential_risk[province], max_amount);

                    // Mix each color depending of how many live there compared to max_amount
                    auto min = Eng3D::Color::rgb8(128, 128, 255);
                    auto max = Eng3D::Color::rgb8(255, 64, 64);
                    std::vector<ProvinceColor> province_color;
                    for(const auto& province : world.provinces) {
                        auto ratio = province.average_militancy();
                        province_color.emplace_back(province.get_id(), Eng3D::Color::lerp(min, max, ratio));
                    }
                    return province_color;
                };
            })(selected_province),
            ([](ProvinceId province_id) {
                return [province_id](const World& world, const ProvinceId id) -> std::string {
                    const auto& province = world.provinces[id];
                    return translate_format("Average militancy: %.2f\nAverage life needs met: %.2f", province.average_militancy(), province.average_life_needs());
                };
            })(selected_province));
        });
        set_mapmode_options(nullptr);
    });
    militancy_ibtn.set_tooltip("Militancy");

    auto& debug_ibtn = flex_column2.make_widget<UI::Image>(0, 0, 24, 24, "gfx/icon.png");
    debug_ibtn.set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection([](const World&, Map& map, const Province& selected_province) {
            map.set_map_mode(([](ProvinceId id) {
                return [id](const World& world) {
                    const auto& province = world.provinces[id];
                    const auto& nation = world.nations[province.owner_id];
                    const auto& ai = ai_man[nation];

                    auto max_amount = 0.f;
                    for(const auto& province : world.provinces)
                        max_amount = glm::max(ai.potential_risk[province], max_amount);

                    // Mix each color depending of how many live there compared to max_amount
                    auto min = Eng3D::Color::rgb8(128, 128, 255);
                    auto max = Eng3D::Color::rgb8(255, 64, 64);
                    std::vector<ProvinceColor> province_color;
                    for(const auto& province : world.provinces) {
                        auto ratio = ai.potential_risk[province] / max_amount;
                        province_color.emplace_back(province.get_id(), Eng3D::Color::lerp(min, max, ratio));
                    }
                    return province_color;
                };
            })(selected_province),
            ([](ProvinceId province_id) {
                return [province_id](const World& world, const ProvinceId id) -> std::string {
                    const auto& province = world.provinces[id];
                    const auto& nation = world.nations[province.owner_id];
                    const auto& ai = ai_man[nation];
                    return translate_format("Potential risk: %.2f (Our strenght: %.2f)\nWar weight: %.2f\nUnit battle weight: %.2f\nUnit exist weight: %.2f\nCoastal weight: %.2f\nReconquer weight: %.2f\nStrength threshold: %.2f\nErratic: %.2f\nOverride threshold: %.2f\nGains/losses: %zu/%zu\nMilitary strength: %.2f", ai.potential_risk[province], ai.military_strength, ai.war_weight, ai.unit_battle_weight, ai.unit_exist_weight, ai.coastal_weight, ai.reconquer_weight, ai.strength_threshold, ai.erratic, ai.override_threshold, ai.gains, ai.losses);
                };
            })(selected_province));
        });
        set_mapmode_options(nullptr);
    });
    debug_ibtn.set_tooltip("Debug");

    this->make_widget<UI::Image>(65, 5, 332, 166, gs.tex_man.load(gs.package_man.get_unique("gfx/minimap.png")));
}

void Minimap::set_mapmode_options(Widget* widget) {
    if(mapmode_options)
        mapmode_options->kill();
    mapmode_options = widget;
}


void MapmodeCommodityOptions::update_map_mode() {
    this->gs.current_mode = MapMode::NORMAL;
    mapmode_generator map_mode = generic_map_mode(this->province_value);
    mapmode_tooltip map_tooltip = commodity_tooltip(this->commodity_id);
    this->gs.map->set_map_mode(map_mode, map_tooltip);
}

MapmodeCommodityOptions::MapmodeCommodityOptions(GameState& _gs)
    : UI::Div(-200, -300, 200, 500, nullptr),
    gs{ _gs }
{
    this->origin = UI::Origin::MIDDLE_RIGHT_SCREEN;
    this->current_texture = gs.tex_man.load(gs.package_man.get_unique("gfx/window_background.png"));

    auto border_tex = gs.tex_man.load(gs.package_man.get_unique("gfx/border2.png"));
    this->border = UI::Border(border_tex, { 4, 4 }, { 10, 10 });
    this->flex = UI::Flex::COLUMN;

    this->province_value = [this](const World& world, ProvinceId id) {
        const auto& product = world.provinces[id].products[this->commodity_id];
        return product.price;
    };

    auto& price_btn = this->make_widget<UI::Button>(0, 0, 125, 24);
    price_btn.set_text("Price");
    price_btn.set_on_click([this](UI::Widget&) {
        this->province_value = [this](const World& world, ProvinceId id) {
            const auto& product = world.provinces[id].products[this->commodity_id];
            return product.price;
        };
        update_map_mode();
    });
    auto& demand_btn = this->make_widget<UI::Button>(0, 0, 125, 24);
    demand_btn.set_text("Demand");
    demand_btn.set_on_click([this](UI::Widget&) {
        this->province_value = [this](const World& world, ProvinceId id) {
            const auto& product = world.provinces[id].products[this->commodity_id];
            return product.demand;
        };
        update_map_mode();
    });
    auto& supply_btn = this->make_widget<UI::Button>(0, 0, 125, 24);
    supply_btn.set_text("Supply");
    supply_btn.set_on_click([this](UI::Widget&) {
        this->province_value = [this](const World& world, ProvinceId id) {
            const auto& product = world.provinces[id].products[this->commodity_id];
            return product.supply;
        };
        update_map_mode();
    });
    auto& price_delta_btn = this->make_widget<UI::Button>(0, 0, 125, 24);
    price_delta_btn.set_text("Price change");
    price_delta_btn.set_on_click([this](UI::Widget&) {
        this->province_value = [this](const World& world, ProvinceId id) {
            const auto& product = world.provinces[id].products[this->commodity_id];
            return product.price_delta;
        };
        update_map_mode();
    });
    auto& production_btn = this->make_widget<UI::Button>(0, 0, 125, 24);
    production_btn.set_text("Production");
    production_btn.set_on_click([this](UI::Widget&) {
        this->province_value = [this](const World& world, ProvinceId id) {
            const auto& province = world.provinces[id];
            const auto& product = province.products[this->commodity_id];
            auto total = 0.f;
            for(const auto& building_type : world.building_types)
                if(building_type.output_id.has_value() && building_type.output_id.value() == this->commodity_id)
                    total += province.buildings[building_type].get_output_amount();
            return total;
        };
        update_map_mode();
    });

    std::vector<int> sizes{ 160 };
    std::vector<std::string> header{ "Goods" };
    auto& table = this->make_widget<UI::Table<CommodityId::Type>>(4, 4, 400 - 8, 35, sizes, header);
    for(const auto& commodity : gs.world->commodities) {
        auto& row = table.get_row(commodity);
        auto commodity_row = row.get_element(0);
        commodity_row->flex = UI::Flex::ROW;
        auto good_str = Eng3D::Locale::translate(commodity.name.get_string());
        auto good_tex = gs.tex_man.load(gs.package_man.get_unique(commodity.get_icon_path()));
        commodity_row->make_widget<UI::Image>(0, 0, 35, 35, good_tex);
        commodity_row->make_widget<UI::Label>(0, 0, good_str);
        commodity_row->set_key(good_str);
        row.set_on_click([this, commodity](UI::Widget&) {
            this->commodity_id = commodity.get_id();
            update_map_mode();
        });
    }
    update_map_mode();
}

mapmode_tooltip commodity_tooltip(CommodityId good_id) {
    return [good_id](const World& world, const ProvinceId id) -> std::string {
        const auto& province = world.provinces[id];
        const auto& product = province.products[good_id];

        auto total_production = 0.f;
        for(const auto& building_type : world.building_types)
            if(building_type.output_id.has_value() && building_type.output_id.value() == good_id)
                total_production += province.buildings[building_type].get_output_amount();

        std::string str = Eng3D::translate_format(
            "%s,\nPrice %.2f\nGlobal demand %.2f\nDemand %.2f (Sold today %.2f)\nSupply %.2f (Produced today %.2f)\nProduction %.2f\n",
            province.name.data(), product.price, product.global_demand, product.demand, product.bought, product.supply, product.produced, total_production);
        for(const auto& building_type : world.building_types) {
            const auto& building = province.buildings[building_type];
            if(building.level == 0.f)
                continue;
            if(building_type.output_id.has_value() && building_type.output_id.value() != good_id)
                continue;
            str += translate_format("%s %s (level %.0f), scale %.0f, workers %.0f, budget %.0f\n",
                building_type.name.data(),
                building.can_do_output(province,
                building_type.input_ids) ? "(Active)" : "(Inactive)",
                building.level,
                building.production_scale,
                building.workers,
                building.budget);
        }
        return str;
    };
}

mapmode_generator generic_map_mode(
    std::function<float(const World&, ProvinceId)> province_value,
    Eng3D::Color min_col, Eng3D::Color max_col)
{
    return [province_value, min_col, max_col](const World& world) {
        std::vector<std::pair<ProvinceId, float>> province_amounts;
        auto max_value = glm::epsilon<float>();
        for(auto const& province : world.provinces) {
            auto id = province.get_id();
            // Use logaritmic scaling, could be a setting later if needed
            float value = std::log2f(province_value(world, id) + 1.f);
            max_value = glm::max(value, max_value);
            province_amounts.emplace_back(id, value);
        }

        // Mix each color by comparing the province value to max_value
        std::vector<ProvinceColor> province_color;
        for(auto const& [prov_id, value] : province_amounts) {
            auto ratio = value / max_value;
            Eng3D::Color color = Eng3D::Color::lerp(min_col, max_col, ratio);
            province_color.push_back(ProvinceColor(prov_id, color));
        }
        return province_color;
    };
}

#include "eng3d/utils.hpp"
mapmode_generator relations_map_mode(NationId id) {
    return [id](const World& world) {
        std::vector<ProvinceColor> provinces_color;
        const auto& nation = world.nations[id];
        for(size_t i = 0; i < world.provinces.size(); i++) {
            const auto& province = world.provinces[i];
            if(province.controller_id == nation || world.nations[province.controller_id].is_puppeted_by(nation)) {
                auto color = Eng3D::Color::rgb8(0x00, 0x00, 0xff);
                provinces_color.emplace_back(ProvinceId(i), color);
                continue;
            }

            const auto& relation = world.get_relation(province.controller_id, id);
            const uint8_t r = relation.relation < 0.f ? -relation.relation : 0;
            const uint8_t g = relation.relation > 0.f ? relation.relation : 0;
            const uint8_t b = relation.is_allied() ? 0x80 : 0;
            auto color = Eng3D::Color::rgb8(r, g, b);
            provinces_color.emplace_back(ProvinceId(i), color);
        }
        return provinces_color;
    };
}

mapmode_tooltip relations_tooltip(NationId nation_id) {
    return [nation_id](const World& world, const ProvinceId id) -> std::string {
        const auto& province = world.provinces[id];
        const auto& province_controller = world.nations[province.controller_id];
        std::string str;

        if(province.controller_id == province.owner_id) {
            str += province_controller.get_client_hint().name;
        } else {
            str += translate_format("Province owned by %s and controlled by %s", world.nations[province.owner_id].get_client_hint().name.data(), province_controller.get_client_hint().name.data());
        }

        if(province_controller.is_puppeted) {
            str += string_format("\nPuppet of %s", world.nations[province_controller.puppet_master_id].get_client_hint().name.data());
            return str;
        }

        if(province.controller_id != nation_id) {
            const auto& nation = world.nations[province.controller_id];
            const auto& relation = world.get_relation(nation, nation_id);
            if(relation.is_allied()) {
                str += string_format("\nAllied with %s", nation.get_client_hint().name.data());
            } else if(relation.has_war) {
                str += string_format("\nAt war with %s", nation.get_client_hint().name.data());
            }

            static std::array<std::string, 7> rel_lvls = {
                "unfriendly",
                "uncordial",
                "discorteous",
                "neutral",
                "respectful",
                "cordial",
                "friendly"
            };

            size_t idx = (1.f + relation.relation) * rel_lvls.size();
            str += string_format("\n%.2f%% - %s", relation.relation * 100.f, rel_lvls[idx % rel_lvls.size()].data());

            nation.get_allies([&](const auto& _nation) {
                str += string_format("%s,", _nation.get_client_hint().name.data());
            });
        }
        return str;
    };
}

std::vector<ProvinceColor> terrain_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    for(size_t i = 0; i < world.provinces.size(); i++)
        province_color.emplace_back(ProvinceId(i), Eng3D::Color{});
    province_color.emplace_back(ProvinceId(-2), Eng3D::Color{}); // Water
    province_color.emplace_back(ProvinceId(-1), Eng3D::Color{}); // Land
    return province_color;
}

std::vector<ProvinceColor> religion_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    auto min = Eng3D::Color::rgb8(255, 255, 255);
    for(size_t i = 0; i < world.provinces.size(); i++) {
        const auto& province = world.provinces[i];
        std::vector<size_t> religion_amounts(world.religions.size());
        size_t total_amount = province.total_pops();
        size_t max_amount = 0;
        size_t max_religion_id = 0;
        for(const auto& religion : world.religions) {
            religion_amounts[religion] = province.religions[religion] * total_amount;
            size_t amount = religion_amounts[religion];
            if(amount > max_amount) {
                max_amount = amount;
                max_religion_id = religion;
            }
        }
        const auto max = Eng3D::Color::bgr32(world.religions[max_religion_id].color);
        const auto color = Eng3D::Color::lerp(min, max, ((float)max_amount) / total_amount);
        province_color.emplace_back(ProvinceId(i), color);
    }
    province_color.emplace_back(ProvinceId(-2), Eng3D::Color(0.f, 0.f, 0.f)); // Water
    province_color.emplace_back(ProvinceId(-1), Eng3D::Color(0.8f, 0.8f, 0.8f)); // Land
    return province_color;
}

std::string religion_tooltip(const World& world, const ProvinceId id) {
    const auto& province = world.provinces[id];
    const auto it = std::max_element(province.religions.begin(), province.religions.end());
    return world.religions[std::distance(province.religions.begin(), it)].name;
}

std::vector<ProvinceColor> language_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    const auto min = Eng3D::Color::rgb8(255, 255, 255);
    for(size_t i = 0; i < world.provinces.size(); i++) {
        const auto& province = world.provinces[i];
        std::vector<size_t> language_amounts(world.languages.size());
        size_t total_amount = province.total_pops();
        size_t max_amount = 0;
        size_t max_language_id = 0;
        for(const auto& language : world.languages) {
            language_amounts[language] = province.languages[language] * total_amount;
            size_t amount = language_amounts[language];
            if(amount > max_amount) {
                max_amount = amount;
                max_language_id = language;
            }
        }
        const auto max = Eng3D::Color::bgr32(world.languages[max_language_id].color);
        const auto color = Eng3D::Color::lerp(min, max, ((float)max_amount) / total_amount);
        province_color.emplace_back(ProvinceId(i), color);
    }
    province_color.emplace_back((ProvinceId)-2, Eng3D::Color{}); // Water
    province_color.emplace_back((ProvinceId)-1, Eng3D::Color(0.8f, 0.8f, 0.8f)); // Land
    return province_color;
}

std::string language_tooltip(const World& world, const ProvinceId id) {
    const auto& province = world.provinces[id];
    if(!province.is_populated()) return "";
    const auto it = std::max_element(province.languages.begin(), province.languages.end());
    return world.languages[std::distance(province.languages.begin(), it)].name;
}

std::vector<ProvinceColor> population_map_mode(const World& world) {
    // Find the maximum amount of pops in one province
    std::vector<std::pair<ProvinceId, float>> province_amounts;
    float max_amount = 1.f;
    for(auto const& province : world.provinces) {
        float amount = std::accumulate(province.pops.cbegin(), province.pops.cend(), 0, [](const float a, const auto& e) {
            return a + e.size;
        });
        amount = std::log2(amount);
        max_amount = glm::max(amount, max_amount);
        province_amounts.emplace_back(province.get_id(), amount);
    }

    // Mix each color depending of how many live there compared to max_amount
    Eng3D::Color min = Eng3D::Color::rgb8(255, 0, 0);
    Eng3D::Color max = Eng3D::Color::rgb8(16, 255, 128);
    std::vector<ProvinceColor> province_color;
    for(auto const& [province_id, amount] : province_amounts) {
        if(amount == 0.f)
        {
            province_color.emplace_back(province_id, Eng3D::Color::rgb8(255, 255, 255));
            continue;
        }
        auto ratio = amount / max_amount;
        Eng3D::Color color = Eng3D::Color::lerp(min, max, ratio);
        province_color.emplace_back(province_id, color);
    }
    return province_color;
}

std::string population_tooltip(const World& world, const ProvinceId id) {
    const auto& province = world.provinces[id];
    if(!province.is_populated()) return "";
    size_t amount = province.total_pops();
    return string_format("%s\nPopulation: %zu", province.name.data(), amount);
}

mapmode_generator trade_map_mode(ProvinceId id) {
    return [id](const World& world) {
        // Find the maximum amount of pops in one province
        std::vector<std::pair<ProvinceId, float>> province_amounts;
        auto max_amount = 1.f;
        for(const auto& province : world.provinces) {
            auto amount = world.economy_state.trade.get_trade_cost(world.provinces[id], province, glm::vec2{ world.width, world.height });
            amount = std::log2(amount);
            max_amount = glm::max(amount, max_amount);
            province_amounts.emplace_back(province.get_id(), amount);
        }

        // Mix each color depending of how many live there compared to max_amount
        auto min = Eng3D::Color::rgb8(128, 128, 255);
        auto max = Eng3D::Color::rgb8(255, 64, 64);
        std::vector<ProvinceColor> province_color;
        for(const auto& [province_id, amount] : province_amounts) {
            if(province_id == id)
            {
                province_color.emplace_back(province_id, Eng3D::Color::rgb8(255, 255, 128));
                continue;
            }
            auto ratio = amount / max_amount;
            province_color.emplace_back(province_id, Eng3D::Color::lerp(min, max, ratio));
        }
        return province_color;
    };
}

mapmode_tooltip trade_tooltip(ProvinceId province_id) {
    return [province_id](const World& world, const ProvinceId id) -> std::string {
        const auto& province = world.provinces[province_id];
        const auto& other_province = world.provinces[id];
        auto amount = world.economy_state.trade.get_trade_cost(province, other_province, glm::vec2{ world.width, world.height });
        return translate_format("Transport from %s to %s costs %.2f", province.name.data(), other_province.name.data(), amount);
    };
}
