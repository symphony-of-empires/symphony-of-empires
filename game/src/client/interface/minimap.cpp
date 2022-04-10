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
//      client/interface/minimap.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cmath>
#include <unordered_map>

#include "unified_render/path.hpp"
#include "unified_render/string_format.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/ui/button.hpp"
#include "unified_render/ui/close_button.hpp"
#include "unified_render/ui/image.hpp"
#include "unified_render/ui/tooltip.hpp"
#include "unified_render/ui/div.hpp"
#include "unified_render/ui/label.hpp"
#include "unified_render/locale.hpp"

#include "client/interface/minimap.hpp"
#include "client/map.hpp"
#include "world.hpp"

using namespace Interface;

mapmode_tooltip good_tooltip(Good::Id id);
mapmode_generator good_map_mode(Good::Id id);
mapmode_generator relations_map_mode(Nation::Id id);
mapmode_tooltip relations_tooltip(Nation::Id id);
void relations_map_mode_selector(const World& world, Map& map, Province* province);
std::vector<ProvinceColor> terrain_color_map_mode(const World& world);
std::string terrain_type_tooltip(const World& world, const Province::Id id);
std::vector<ProvinceColor> population_map_mode(const World& world);
std::string population_tooltip(const World& world, const Province::Id id);
std::vector<ProvinceColor> culture_map_mode(const World& world);
std::string culture_tooltip(const World& world, const Province::Id id);
std::vector<ProvinceColor> religion_map_mode(const World& world);
std::string religion_tooltip(const World& world, const Province::Id id);
// UNUSED
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
    //flex_column1->flex_justify = UI::FlexJustify::SPACE_BETWEEN;
    flex_column1->flex_align = UI::Align::CENTER;
    auto* flex_column2 = new UI::Div(35, 5, 24, 190, this);
    flex_column2->flex = UI::Flex::COLUMN;
    //flex_column2->flex_justify = UI::FlexJustify::SPACE_BETWEEN;
    flex_column2->flex_align = UI::Align::CENTER;

    auto* flat_btn = new UI::Image(0, 0, 24, 24, "gfx/flat_icon.png", flex_column1);
    flat_btn->on_click = ([this](UI::Widget&) {
        this->gs.map->set_view(MapView::PLANE_VIEW);
    });
    flat_btn->set_tooltip("Flat map");

    auto* globe_btn = new UI::Image(0, 0, 24, 24, "gfx/globe_icon.png", flex_column2);
    globe_btn->on_click = ([this](UI::Widget&) {
        this->gs.map->set_view(MapView::SPHERE_VIEW);
    });
    globe_btn->set_tooltip("Globe map");

    auto* landscape_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column1);
    landscape_ibtn->on_click = ([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);

        mapmode_generator map_mode = terrain_map_mode;
        mapmode_tooltip tooltip = empty_province_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    landscape_ibtn->set_tooltip("Political");

    auto* political_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column1);
    political_ibtn->on_click = ([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);

        mapmode_generator map_mode = political_map_mode;
        mapmode_tooltip tooltip = empty_province_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    political_ibtn->set_tooltip("Political");

    auto* relations_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column1);
    relations_ibtn->on_click = ([this](UI::Widget&) {
        this->gs.map->set_selection(relations_map_mode_selector);
        Nation* current_nation = this->gs.curr_nation;
        Nation::Id id = this->gs.world->get_id(*current_nation);
        mapmode_generator map_mode = relations_map_mode(id);
        mapmode_tooltip tooltip = relations_tooltip(id);
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    relations_ibtn->set_tooltip("Relations");

    auto* population_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column1);
    population_ibtn->on_click = ([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = population_map_mode;
        mapmode_tooltip tooltip = population_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        this->gs.current_mode = MapMode::NORMAL;
        set_mapmode_options(nullptr);
    });
    population_ibtn->set_tooltip("Population");

    auto* terrain_color_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column2);
    terrain_color_ibtn->on_click = ([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = terrain_color_map_mode;
        mapmode_tooltip tooltip = terrain_type_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    terrain_color_ibtn->set_tooltip("Terrain type");

    auto* culture_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column2);
    culture_ibtn->on_click = ([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = culture_map_mode;
        mapmode_tooltip tooltip = culture_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    culture_ibtn->set_tooltip("Culture diversity");

    auto* religion_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column2);
    religion_ibtn->on_click = ([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);
        mapmode_generator map_mode = religion_map_mode;
        mapmode_tooltip tooltip = religion_tooltip;
        this->gs.map->set_map_mode(map_mode, tooltip);
        set_mapmode_options(nullptr);
    });
    religion_ibtn->set_tooltip("Religion");

    auto* good_price_ibtn = new UI::Image(0, 0, 24, 24, "gfx/icon.png", flex_column2);
    good_price_ibtn->on_click = ([this](UI::Widget&) {
        this->gs.map->set_selection(nullptr);

        set_mapmode_options(new MapmodeGoodOptions(this->gs));
    });
    good_price_ibtn->set_tooltip("Prices");

    new UI::Image(65, 5, 332, 166, &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/minimap.png")), this);
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
    this->current_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/window_background.png"));
    this->is_scroll = true;

    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    auto tex_man = UnifiedRender::State::get_instance().tex_man;
    auto border_tex = &tex_man->load(Path::get("gfx/ui/border2.png"));
    this->border = UI::Border(border_tex, size, texture_size);

    auto goods = gs.world->goods;

    auto* flex_column = new UI::Div(4, 4, 192, goods.size() * 35, this);
    flex_column->flex = UI::Flex::COLUMN;
    flex_column->flex_justify = UI::FlexJustify::START;

    for(size_t i = 0; i < goods.size(); i++) {
        Good* good = goods[i];
        UnifiedRender::TextureOptions options;
        options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        options.mag_filter = GL_LINEAR;
        auto good_tex = &tex_man->load(Path::get("gfx/good/" + good->ref_name + ".png"), options);

        auto* good_div = new UI::Div(0, 0, 200, 35, flex_column);
        new UI::Image(0, 0, 35, 35, good_tex, good_div);
        new UI::Label(35, 0, good->name, good_div);
        good_div->on_click = ([this, i](UI::Widget&) {
            this->gs.current_mode = MapMode::NORMAL;

            mapmode_generator map_mode = good_map_mode((Good::Id)i);
            mapmode_tooltip tooltip = good_tooltip((Good::Id)i);
            this->gs.map->set_map_mode(map_mode, tooltip);
        });
    }

    mapmode_generator map_mode = good_map_mode(0);
    mapmode_tooltip tooltip = good_tooltip(0);
    this->gs.map->set_map_mode(map_mode, tooltip);
}

mapmode_tooltip good_tooltip(Good::Id good_id) {
    return [good_id](const World& world, const Province::Id id) {
        const Province& province = *world.provinces[id];

        if(province.controller == nullptr) {
            return UnifiedRender::Locale::translate("Uncontrolled");
        }
        const Product& product = province.products[good_id]; 
        std::string str;
        str += UnifiedRender::string_format("Price: %f\n", product.price);
        str += UnifiedRender::string_format("Demand: %f\n", product.demand);
        str += UnifiedRender::string_format("Supply: %f\n", product.supply);
        return str;
    };
}

mapmode_generator good_map_mode(Good::Id id) {
    return [id](const World& world) {
        std::vector<std::pair<Province::Id, float>> province_amounts;
        float max_price = 0.0001f;
        for(auto const& province : world.provinces) {
            Product product = province->products[id];
            float price = log2(product.price + 1.f);
            max_price = std::max<float>(price, max_price);
            province_amounts.push_back(std::make_pair(world.get_id(*province), price));
        }

        // Mix each color depending of how many live there compared to max_amount
        UnifiedRender::Color min = UnifiedRender::Color::rgb8(255, 229, 217);
        UnifiedRender::Color max = UnifiedRender::Color::rgb8(220, 46, 35);
        std::vector<ProvinceColor> province_color;
        for(auto const& prov_amount : province_amounts) {
            Province::Id prov_id = prov_amount.first;
            uint32_t price = prov_amount.second;
            float ratio = price / max_price;
            UnifiedRender::Color color = UnifiedRender::Color::lerp(min, max, ratio);
            province_color.push_back(ProvinceColor(prov_id, color));
        }
        return province_color;
    };
}

void relations_map_mode_selector(const World& world, Map& map, Province* province) {
    if(province->controller == nullptr) {
        return;
    }
    Nation::Id nation_id = world.get_id(*province->owner);
    mapmode_generator map_mode = relations_map_mode(nation_id);
    mapmode_tooltip tooltip = relations_tooltip(nation_id);
    map.set_map_mode(map_mode, tooltip);
}

#include "unified_render/utils.hpp"
mapmode_generator relations_map_mode(Nation::Id id) {
    return [id](const World& world) {
        std::vector<ProvinceColor> provinces_color;
        Nation* nation = world.nations[id];
        for(unsigned int i = 0; i < world.provinces.size(); i++) {
            const Province& province = *world.provinces[i];
            if(province.controller == nullptr) {
                UnifiedRender::Color color = UnifiedRender::Color::rgba32(bswap32(0x808080ff));
                provinces_color.push_back(ProvinceColor(i, color));
                continue;
            }

            if(province.controller == nation || province.controller->puppet_master == nation) {
                UnifiedRender::Color color = UnifiedRender::Color::rgba32(bswap32(0x8080ffff));
                provinces_color.push_back(ProvinceColor(i, color));
                continue;
            }

            const NationRelation& rel = province.controller->relations[id];
            if(rel.has_alliance) {
                UnifiedRender::Color color = UnifiedRender::Color::rgba32(bswap32(0x20d4d1ff));
                provinces_color.push_back(ProvinceColor(i, color));
                continue;
            }

            const uint8_t r = (rel.relation < 0.f) ? -rel.relation : 0.f;
            const uint8_t g = (rel.relation > 0.f) ? rel.relation : 0.f;
            const uint8_t b = g;
            UnifiedRender::Color color = UnifiedRender::Color::rgba32(bswap32(~(0x00000000 | (b << 24) | (r << 16) | (g << 8))));
            provinces_color.push_back(ProvinceColor(i, color));
        }
        return provinces_color;
    };
}

mapmode_tooltip relations_tooltip(Nation::Id nation_id) {
    return [nation_id](const World& world, const Province::Id id) {
        const Province& province = *world.provinces[id];
        std::string str;

        if(province.controller == nullptr) {
            str += UnifiedRender::Locale::translate("Uncontrolled");
            return str;
        }

        if(province.controller == province.owner) {
            str += UnifiedRender::Locale::translate(province.controller->get_client_hint().alt_name);
        } else {
            str += UnifiedRender::Locale::translate("Owned by") + " ";
            str += UnifiedRender::Locale::translate(province.owner->get_client_hint().alt_name);
            str += " " + UnifiedRender::Locale::translate("controlled by") + " ";
            str += UnifiedRender::Locale::translate(province.controller->get_client_hint().alt_name);
        }
        str += " ";

        if(province.controller->puppet_master == world.nations[nation_id]) {
            str += "(puppet of " + world.nations[nation_id]->get_client_hint().alt_name + ") ";
            return str;
        }

        const NationRelation& rel = province.controller->relations[nation_id];
        if(rel.has_alliance) {
            str += "allied with " + world.nations[nation_id]->get_client_hint().alt_name;
            return str;
        } else if(rel.has_war) {
            str += "at war with " + world.nations[nation_id]->get_client_hint().alt_name;
            return str;
        }

        const std::vector<std::string> rel_lvls = {
            "nemesis",
            "enemy",
            "disrespectful",
            "neutral",
            "respectful",
            "collaborates",
            "friendly"
        };

        int idx = ((rel.relation + 200.f) / (200.f * 2.f)) * rel_lvls.size();
        str += std::to_string(rel.relation) + "(" + rel_lvls[idx % rel_lvls.size()] + ")";

        /*int ally_cnt = 0;
        str += UnifiedRender::Locale::translate("Allied with") + " ";
        for(const auto& nation : gs.world->nations) {
            const NationRelation& rel = province.controller->relations[world.get_id(*nation)];
            if(rel.has_alliance) {
                str += UnifiedRender::Locale::translate(nation->get_client_hint().alt_name);
                str += ", ";
                ally_cnt++;
            }
        }
        if(ally_cnt == 0) {
            str += UnifiedRender::Locale::translate("nobody");
        }*/
        return str;
    };
}

std::vector<ProvinceColor> terrain_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        province_color.push_back(ProvinceColor(i, UnifiedRender::Color::rgba32(0x00000000)));
    }
    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UnifiedRender::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UnifiedRender::Color::rgba32(0x00000000)));
    return province_color;
}

std::vector<ProvinceColor> terrain_color_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        Province* province = world.provinces[i];
        UnifiedRender::Color color = UnifiedRender::Color::rgba32(province->terrain_type->color);
        province_color.push_back(ProvinceColor(i, color));
    }
    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UnifiedRender::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UnifiedRender::Color::rgba32(0x00000000)));
    return province_color;
}

std::string terrain_type_tooltip(const World& world, const Province::Id id) {
    Province* province = world.provinces[id];
    return province->terrain_type->name;
}

std::vector<ProvinceColor> population_map_mode(const World& world) {
    // Find the maximum amount of pops in one province
    std::vector<std::pair<Province::Id, float>> province_amounts;
    float max_amount = 1.f;
    for(auto const& province : world.provinces) {
        float amount = 0.f;
        for(auto const& pop : province->pops) {
            amount += pop.size;
        }
        //amount = log2(amount + 1.f);
        max_amount = std::max<float>(amount, max_amount);
        province_amounts.push_back(std::make_pair(world.get_id(*province), amount));
    }

    // Mix each color depending of how many live there compared to max_amount
    UnifiedRender::Color min = UnifiedRender::Color::rgb8(255, 255, 255);
    UnifiedRender::Color max = UnifiedRender::Color::rgb8(180, 24, 24);
    std::vector<ProvinceColor> province_color;
    for(auto const& prov_amount : province_amounts) {
        Province::Id prov_id = prov_amount.first;
        uint32_t amount = prov_amount.second;
        float ratio = amount / max_amount;
        UnifiedRender::Color color = UnifiedRender::Color::lerp(min, max, ratio);
        province_color.push_back(ProvinceColor(prov_id, color));
    }
    return province_color;
}

std::string population_tooltip(const World& world, const Province::Id id){
    Province* province = world.provinces[id];

    size_t amount = 0;
    for(auto const& pop : province->pops) {
        amount += pop.size;
    }
    std::string out;
    out += province->name + "\n";
    out += "Population: " + std::to_string(amount);
    return out;
}

std::vector<ProvinceColor> culture_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    UnifiedRender::Color min = UnifiedRender::Color::rgb8(255, 255, 255);
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        Province* province = world.provinces[i];
        std::unordered_map<Culture::Id, size_t> culture_amounts;
        size_t total_amount = 0;
        size_t max_amount = 0;
        Culture::Id max_culture_id = 0;
        for(unsigned int j = 0; j < province->pops.size(); j++) {
            const Pop& pop = province->pops[j];
            total_amount += pop.size;

            auto search = culture_amounts.find(pop.culture->cached_id);
            if(search == culture_amounts.end()) {
                culture_amounts[pop.culture->cached_id] = pop.size;
            }
            else {
                culture_amounts[pop.culture->cached_id] += pop.size;
            }

            size_t amount = culture_amounts[pop.culture->cached_id];
            if(amount > max_amount) {
                max_amount = amount;
                max_culture_id = pop.culture->cached_id;
            }
        }
        UnifiedRender::Color max = UnifiedRender::Color::rgba32(world.cultures[max_culture_id]->color);
        UnifiedRender::Color color = UnifiedRender::Color::lerp(min, max, ((float)max_amount) / total_amount);
        province_color.push_back(ProvinceColor(i, color));
    }

    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UnifiedRender::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UnifiedRender::Color::rgba32(0xffdddddd)));
    return province_color;
}

std::string culture_tooltip(const World& world, const Province::Id id){
    Province* province = world.provinces[id];

    typedef std::pair<Culture::Id, size_t> culture_amount;
    std::vector<culture_amount> cultures;
    for(unsigned int i = 0; i < province->pops.size(); i++) {
        const Pop& pop = province->pops[i];

        bool found = false;
        for(auto culture_amount : cultures) {
            if(culture_amount.first == pop.culture->cached_id) {
                culture_amount.second += pop.size;
                found = true;
            }
        }

        if(!found) {
            cultures.push_back(std::make_pair(pop.culture->cached_id, pop.size));
        }
    }

    std::sort(cultures.begin(), cultures.end(), [](culture_amount a, culture_amount b) {
        return a.second > b.second;
    });

    std::string out;
    for(auto culture_amount : cultures) {
        out += world.cultures[culture_amount.first]->name + std::to_string(culture_amount.second) + "\n";
    }
    return out;
}

std::vector<ProvinceColor> religion_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    UnifiedRender::Color min = UnifiedRender::Color::rgb8(255, 255, 255);
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        Province* province = world.provinces[i];
        std::unordered_map<Religion::Id, size_t> religion_amounts;
        size_t total_amount = 0;
        size_t max_amount = 0;
        Religion::Id max_religion_id = 0;
        for(unsigned int j = 0; j < province->pops.size(); j++) {
            const Pop& pop = province->pops[j];
            total_amount += pop.size;

            auto search = religion_amounts.find(pop.religion->cached_id);
            if(search == religion_amounts.end()) {
                religion_amounts[pop.religion->cached_id] = pop.size;
            }
            else {
                religion_amounts[pop.religion->cached_id] += pop.size;
            }

            size_t amount = religion_amounts[pop.religion->cached_id];
            if(amount > max_amount) {
                max_amount = amount;
                max_religion_id = pop.religion->cached_id;
            }
        }
        UnifiedRender::Color max = UnifiedRender::Color::rgba32(world.religions[max_religion_id]->color);
        UnifiedRender::Color color = UnifiedRender::Color::lerp(min, max, ((float)max_amount) / total_amount);
        province_color.push_back(ProvinceColor(i, color));
    }

    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UnifiedRender::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UnifiedRender::Color::rgba32(0xffdddddd)));
    return province_color;
}

std::string religion_tooltip(const World& world, const Province::Id id){
    Province* province = world.provinces[id];

    typedef std::pair<Religion::Id, size_t> religion_amount;
    std::vector<religion_amount> religions;
    for(unsigned int i = 0; i < province->pops.size(); i++) {
        const Pop& pop = province->pops[i];

        bool found = false;
        for(auto religion_amount : religions) {
            if(religion_amount.first == pop.religion->cached_id) {
                religion_amount.second += pop.size;
                found = true;
            }
        }

        if(!found) {
            religions.push_back(std::make_pair(pop.religion->cached_id, pop.size));
        }
    }

    std::sort(religions.begin(), religions.end(), [](religion_amount a, religion_amount b) {
        return a.second > b.second;
    });

    std::string out;
    for(auto religion_amount : religions) {
        out += world.religions[religion_amount.first]->name + std::to_string(religion_amount.second) + "\n";
    }
    return out;
}