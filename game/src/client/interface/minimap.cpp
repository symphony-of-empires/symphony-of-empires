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

#include "client/interface/minimap.hpp"
#include "client/map.hpp"
#include "world.hpp"

using namespace Interface;

mapmode_tooltip good_tooltip(GoodId id);
mapmode_generator goods_map_mode(GoodId id);
mapmode_generator relations_map_mode(NationId id);
mapmode_tooltip relations_tooltip(NationId id);
std::vector<ProvinceColor> terrain_color_map_mode(const World& world);
std::string terrain_type_tooltip(const World& world, const ProvinceId id);
std::vector<ProvinceColor> population_map_mode(const World& world);
std::vector<ProvinceColor> language_map_mode(const World& world);
std::string language_tooltip(const World& world, const ProvinceId id);
std::vector<ProvinceColor> religion_map_mode(const World& world);
std::string religion_tooltip(const World& world, const ProvinceId id);
std::vector<ProvinceColor> terrain_map_mode(const World& world);

Minimap::Minimap(GameState& _gs, int x, int y, UI::Origin origin)
    : UI::Window(x, y, 400, 200),
    gs{ _gs }
{
    this->origin = origin;
    this->is_pinned = true;
    this->is_scroll = false;
    this->padding = glm::ivec2(0, 24);

    auto* flex_column1 = new UI::Div(5, 5, 24, 190, this);
    flex_column1->flex = UI::Flex::COLUMN;
    flex_column1->flex_align = UI::Align::CENTER;
    auto* flex_column2 = new UI::Div(35, 5, 24, 190, this);
    flex_column2->flex = UI::Flex::COLUMN;
    flex_column2->flex_align = UI::Align::CENTER;

    auto* flat_btn = new UI::Image(0, 0, 24, 24, "gfx/flat_icon.png", flex_column1);
    flat_btn->set_on_click([this](UI::Widget&) {
        this->gs.map->set_view(MapView::PLANE_VIEW);
    });
    flat_btn->set_tooltip("Flat map");

    auto* globe_btn = new UI::Image(0, 0, 24, 24, "gfx/globe_icon.png", flex_column2);
    globe_btn->set_on_click([this](UI::Widget&) {
        this->gs.map->set_view(MapView::SPHERE_VIEW);
    });
    globe_btn->set_tooltip("Globe map");

    auto* landscape_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column1);
    landscape_ibtn->set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = terrain_map_mode;
        mapmode_tooltip tooltip = empty_province_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    landscape_ibtn->set_tooltip("Terrain");

    auto* political_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column1);
    political_ibtn->set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = political_map_mode;
        mapmode_tooltip tooltip = political_province_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    political_ibtn->set_tooltip("Political");

    auto* relations_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column1);
    relations_ibtn->set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection([](const World&, Map& map, const Province& province) {
            if(Nation::is_invalid(province.controller_id)) return;
            NationId nation_id = province.owner_id;
            mapmode_generator map_mode = relations_map_mode(nation_id);
            mapmode_tooltip tooltip = relations_tooltip(nation_id);
            map.set_map_mode(map_mode, tooltip);
        });
        const auto nation_id = this->gs.curr_nation->get_id();
        mapmode_generator map_mode = relations_map_mode(nation_id);
        mapmode_tooltip tooltip = relations_tooltip(nation_id);
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    relations_ibtn->set_tooltip("Relations");

    auto* population_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column1);
    population_ibtn->set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = population_map_mode;
        mapmode_tooltip tooltip = [](const World& world, const ProvinceId id) -> std::string {
            const auto& province = world.provinces[id];
            if(province.pops.empty()) return "";
            size_t amount = 0;
            for(auto const& pop : province.pops)
                amount += pop.size;
            return string_format("\nPopulation: %zu", province.name.c_str(), amount);
        };
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    population_ibtn->set_tooltip("Population");

    auto* terrain_color_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column2);
    terrain_color_ibtn->set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = terrain_color_map_mode;
        mapmode_tooltip tooltip = terrain_type_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    terrain_color_ibtn->set_tooltip("Simple terrain");

    auto* language_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column2);
    language_ibtn->set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = language_map_mode;
        mapmode_tooltip tooltip = language_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    language_ibtn->set_tooltip("Language diversity");

    auto* religion_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column2);
    religion_ibtn->set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = religion_map_mode;
        mapmode_tooltip tooltip = religion_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    religion_ibtn->set_tooltip("Religion");

    auto* good_price_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column2);
    good_price_ibtn->set_on_click([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        set_mapmode_options(new MapmodeGoodOptions(this->gs));
    });
    good_price_ibtn->set_tooltip("Prices");

    new UI::Image(65, 5, 332, 166, gs.tex_man.load(gs.package_man.get_unique("gfx/minimap.png")), this);
}

void Minimap::set_mapmode_options(Widget* widget) {
    if(mapmode_options) {
        mapmode_options->kill();
    }
    mapmode_options = widget;
}

MapmodeGoodOptions::MapmodeGoodOptions(GameState& gs)
    : UI::Div(-200, -250, 200, 500, nullptr),
    gs{ gs }
{
    this->origin = UI::Origin::MIDDLE_RIGHT_SCREEN;
    this->current_texture = gs.tex_man.load(gs.package_man.get_unique("gfx/window_background.png"));
    this->is_scroll = true;

    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    auto& tex_man = gs.tex_man;
    auto border_tex = tex_man.load(gs.package_man.get_unique("gfx/border2.png"));
    this->border = UI::Border(border_tex, size, texture_size);

    auto goods = gs.world->goods;

    auto* flex_column = new UI::Div(4, 4, 192, goods.size() * 35, this);
    flex_column->flex = UI::Flex::COLUMN;
    flex_column->flex_justify = UI::FlexJustify::START;
    for(const auto& good : goods) {
        auto good_tex = tex_man.load(gs.package_man.get_unique(good.get_icon_path()));
        auto* good_div = new UI::Div(0, 0, 200, 35, flex_column);
        new UI::Image(0, 0, 35, 35, good_tex, good_div);
        new UI::Label(35, 0, good.name, good_div);
        good_div->set_on_click([this, &good](UI::Widget&) {
            this->gs.current_mode = MapMode::NORMAL;
            mapmode_generator map_mode = goods_map_mode(good);
            mapmode_tooltip tooltip = good_tooltip(good);
            this->gs.map->set_map_mode(map_mode, tooltip);
        });
    }

    mapmode_generator map_mode = goods_map_mode(GoodId(0zu));
    mapmode_tooltip tooltip = good_tooltip(GoodId(0zu));
    this->gs.map->set_map_mode(map_mode, tooltip);
}

mapmode_tooltip good_tooltip(GoodId good_id) {
    return [good_id](const World& world, const ProvinceId id) -> std::string {
        const auto& province = world.provinces[id];
        if(Nation::is_invalid(province.controller_id))
            return "";
        const auto& product = province.products[good_id];
        std::string str;
        str += Eng3D::translate_format("Price: %.2f\n", product.price);
        str += Eng3D::translate_format("Demand: %.2f\n", product.demand);
        str += Eng3D::translate_format("Supply: %.2f\n", product.supply);
        return str;
    };
}

mapmode_generator goods_map_mode(GoodId id) {
    return [id](const World& world) {
        std::vector<std::pair<ProvinceId, float>> province_amounts;
        auto max_price = 0.0001f;
        for(auto const& province : world.provinces) {
            const auto& product = province.products[id];
            auto price = std::log2f(product.price + 1.f);
            max_price = glm::max(price, max_price);
            province_amounts.emplace_back(province, price);
        }

        // Mix each color depending of how many live there compared to max_amount
        Eng3D::Color min = Eng3D::Color::rgb8(220, 46, 35);
        Eng3D::Color max = Eng3D::Color::rgb8(255, 229, 217);
        std::vector<ProvinceColor> province_color;
        for(auto const& [prov_id, price] : province_amounts) {
            auto ratio = price / max_price;
            Eng3D::Color color = Eng3D::Color::lerp(min, max, ratio);
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
            if(Nation::is_invalid(province.controller_id)) {
                Eng3D::Color color = Eng3D::Color::rgba32(std::byteswap<std::uint32_t>(0x808080ff));
                provinces_color.emplace_back(ProvinceId(i), color);
                continue;
            }

            if(province.controller_id == nation || g_world.nations[province.controller_id].puppet_master_id == nation) {
                auto color = Eng3D::Color::rgb8(0x00, 0x00, 0xff);
                provinces_color.emplace_back(ProvinceId(i), color);
                continue;
            }

            const auto& relation = g_world.get_relation(province.controller_id, id);
            const uint8_t r = (relation.relation < 0) ? -relation.relation : 0;
            const uint8_t g = (relation.relation > 0) ? relation.relation : 0;
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
        const auto& province_controller = g_world.nations[province.controller_id];
        std::string str;

        if(Nation::is_invalid(province.controller_id))
            return "";

        if(Nation::is_valid(province.controller_id) && province.controller_id == province.owner_id) {
            str += province_controller.get_client_hint().alt_name;
        } else if(Nation::is_valid(province.owner_id)) {
            str += translate_format("Owned by %s and controlled by %s", world.nations[province.owner_id].get_client_hint().alt_name.c_str(), province_controller.get_client_hint().alt_name.c_str());
        }

        if(province_controller.puppet_master_id == nation_id) {
            str += string_format(" (puppet of %s)", world.nations[nation_id].get_client_hint().alt_name.c_str());
            return str;
        }

        if(province.controller_id != nation_id) {
            const auto& relation = world.get_relation(province.controller_id, nation_id);
            if(relation.is_allied()) {
                str += string_format(" allied with %s", world.nations[nation_id].get_client_hint().alt_name.c_str());
            } else if(relation.has_war) {
                str += string_format(" at war with %s", world.nations[nation_id].get_client_hint().alt_name.c_str());
            }

            static std::array<std::string, 7> rel_lvls = {
                "nemesis",
                "enemy",
                "disrespectful",
                "neutral",
                "respectful",
                "collaborates",
                "friendly"
            };

            int idx = ((static_cast<float>(relation.relation) + 100.f) / 200.f) * rel_lvls.size();
            str += std::to_string(relation.relation) + "(" + rel_lvls[idx % rel_lvls.size()] + ")";

            int ally_cnt = 0;
            str += " allied with ";
            for(const auto& nation : world.nations) {
                if(province.controller_id == nation) continue;
                const auto& relation = world.get_relation(province.controller_id, world.get_id(nation));
                if(relation.is_allied()) {
                    str += string_format("%s,", nation.get_client_hint().alt_name.c_str());
                    ally_cnt++;
                }
            }
            if(ally_cnt == 0) str += "nobody";
        }
        return str;
    };
}

std::vector<ProvinceColor> terrain_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        province_color.emplace_back(ProvinceId(i), Eng3D::Color::rgba32(0x00000000));
    }
    // Water
    province_color.emplace_back(ProvinceId(-2), Eng3D::Color::rgba32(0x00000000));
    // Land
    province_color.emplace_back(ProvinceId(-1), Eng3D::Color::rgba32(0x00000000));
    return province_color;
}

std::vector<ProvinceColor> terrain_color_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        const auto& province = world.provinces[i];
        Eng3D::Color color = Eng3D::Color::rgba32(world.terrain_types[province.terrain_type_id].color);
        province_color.emplace_back(ProvinceId(i), color);
    }
    // Water
    province_color.emplace_back(ProvinceId(-2), Eng3D::Color::rgba32(0x00000000));
    // Land
    province_color.emplace_back(ProvinceId(-1), Eng3D::Color::rgba32(0x00000000));
    return province_color;
}

std::string terrain_type_tooltip(const World& world, const ProvinceId id) {
    return world.terrain_types[world.provinces[id].terrain_type_id].name;
}

std::vector<ProvinceColor> population_map_mode(const World& world) {
    // Find the maximum amount of pops in one province
    std::vector<std::pair<ProvinceId, float>> province_amounts;
    float max_amount = 1.f;
    for(auto const& province : world.provinces) {
        float amount = std::accumulate(province.pops.cbegin(), province.pops.cend(), 0, [](const float a, const auto& e) {
            return a + e.size;
        });
        //amount = log2(amount + 1.f);
        max_amount = glm::max(amount, max_amount);
        province_amounts.emplace_back(province.get_id(), amount);
    }

    // Mix each color depending of how many live there compared to max_amount
    Eng3D::Color min = Eng3D::Color::rgb8(255, 255, 255);
    Eng3D::Color max = Eng3D::Color::rgb8(180, 24, 24);
    std::vector<ProvinceColor> province_color;
    for(auto const& prov_amount : province_amounts) {
        auto prov_id = prov_amount.first;
        auto amount = prov_amount.second;
        float ratio = amount / max_amount;
        Eng3D::Color color = Eng3D::Color::lerp(min, max, ratio);
        province_color.emplace_back(prov_id, color);
    }
    return province_color;
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
        const auto max = Eng3D::Color::rgba32(world.languages[max_language_id].color);
        const auto color = Eng3D::Color::lerp(min, max, ((float)max_amount) / total_amount);
        province_color.emplace_back(ProvinceId(i), color);
    }

    // Water
    province_color.emplace_back((ProvinceId)-2, Eng3D::Color::rgba32(0x00000000));
    // Land
    province_color.emplace_back((ProvinceId)-1, Eng3D::Color::rgba32(0xffdddddd));
    return province_color;
}

std::string language_tooltip(const World& world, const ProvinceId id) {
    const auto& province = world.provinces[id];
    if(province.pops.empty())
        return "";
    const auto it = std::max_element(province.languages.begin(), province.languages.end());
    return world.languages[std::distance(province.languages.begin(), it)].name;
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
        const auto max = Eng3D::Color::rgba32(world.religions[max_religion_id].color);
        const auto color = Eng3D::Color::lerp(min, max, ((float)max_amount) / total_amount);
        province_color.emplace_back(ProvinceId(i), color);
    }

    // Water
    province_color.emplace_back((ProvinceId)-2, Eng3D::Color::rgba32(0x00000000));
    // Land
    province_color.emplace_back((ProvinceId)-1, Eng3D::Color::rgba32(0xffdddddd));
    return province_color;
}

std::string religion_tooltip(const World& world, const ProvinceId id) {
    const auto& province = world.provinces[id];
    const auto it = std::max_element(province.religions.begin(), province.religions.end());
    return world.religions[std::distance(province.religions.begin(), it)].name;
}
