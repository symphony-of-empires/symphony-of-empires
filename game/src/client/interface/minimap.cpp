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
#include "unified_render/texture.hpp"
#include "unified_render/ui/button.hpp"
#include "unified_render/ui/close_button.hpp"
#include "unified_render/ui/image.hpp"
#include "unified_render/ui/tooltip.hpp"
#include "unified_render/locale.hpp"

#include "client/interface/minimap.hpp"
#include "client/map.hpp"
#include "world.hpp"

using namespace Interface;

std::vector<ProvinceColor> relations_map_mode(const World& world);
std::string relations_tooltip(const World& world, const Province::Id id);
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

    auto* flat_btn = new UI::Image(5, 5, 24, 24, "gfx/flat_icon.png", this);
    flat_btn->on_click = ([](UI::Widget& w) {
        Minimap* state = (Minimap*)w.parent;

        state->gs.map->set_view(MapView::PLANE_VIEW);
    });
    flat_btn->set_tooltip("Flat map");

    auto* globe_btn = new UI::Image(35, 5, 24, 24, "gfx/globe_icon.png", this);
    globe_btn->on_click = ([](UI::Widget& w) {
        Minimap* state = (Minimap*)w.parent;

        state->gs.map->set_view(MapView::SPHERE_VIEW);
    });
    globe_btn->set_tooltip("Globe map");

    auto* political_ibtn = new UI::Image(5, 35, 24, 24, "gfx/icon.png", this);
    political_ibtn->on_click = ([](UI::Widget& w) {
        Minimap* state = (Minimap*)w.parent;
        mapmode_generator map_mode = political_map_mode;
        mapmode_tooltip tooltip = empty_province_tooltip;
        state->gs.map->set_map_mode(map_mode, tooltip);
    });
    political_ibtn->set_tooltip("Political");

    auto* relations_ibtn = new UI::Image(5, 65, 24, 24, "gfx/icon.png", this);
    relations_ibtn->on_click = ([](UI::Widget& w) {
        Minimap* state = (Minimap*)w.parent;
        mapmode_generator map_mode = relations_map_mode;
        mapmode_tooltip tooltip = relations_tooltip;
        state->gs.map->set_map_mode(map_mode, tooltip);
    });
    relations_ibtn->set_tooltip("Relations");

    auto* population_ibtn = new UI::Image(5, 95, 24, 24, "gfx/icon.png", this);
    population_ibtn->on_click = ([](UI::Widget& w) {
        Minimap* state = (Minimap*)w.parent;
        mapmode_generator map_mode = population_map_mode;
        mapmode_tooltip tooltip = population_tooltip;
        state->gs.map->set_map_mode(map_mode, tooltip);
    });
    population_ibtn->set_tooltip("Population");

    auto* terrain_color_ibtn = new UI::Image(35, 35, 24, 24, "gfx/icon.png", this);
    terrain_color_ibtn->on_click = ([](UI::Widget& w) {
        Minimap* state = (Minimap*)w.parent;
        mapmode_generator map_mode = terrain_color_map_mode;
        mapmode_tooltip tooltip = terrain_type_tooltip;
        state->gs.map->set_map_mode(map_mode, tooltip);
    });
    terrain_color_ibtn->set_tooltip("Terrain type");

    auto* culture_ibtn = new UI::Image(35, 65, 24, 24, "gfx/icon.png", this);
    culture_ibtn->on_click = ([](UI::Widget& w) {
        Minimap* state = (Minimap*)w.parent;
        mapmode_generator map_mode = culture_map_mode;
        mapmode_tooltip tooltip = culture_tooltip;
        state->gs.map->set_map_mode(map_mode, tooltip);
    });
    culture_ibtn->set_tooltip("Culture diversity");

    auto* religion_ibtn = new UI::Image(35, 95, 24, 24, "gfx/icon.png", this);
    religion_ibtn->on_click = ([](UI::Widget& w) {
        Minimap* state = (Minimap*)w.parent;
        mapmode_generator map_mode = religion_map_mode;
        mapmode_tooltip tooltip = religion_tooltip;
        state->gs.map->set_map_mode(map_mode, tooltip);
    });
    religion_ibtn->set_tooltip("Religion");

    new UI::Image(65, 5, 332, 166, &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/minimap.png")), this);
}

#include "unified_render/byteswap.hpp"
std::vector<ProvinceColor> relations_map_mode(const World& world) {
    GameState& gs = ((GameState&)GameState::get_instance());

    std::vector<ProvinceColor> provinces_color;
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        const Province& province = *world.provinces[i];
        if(province.controller == nullptr) {
            UnifiedRender::Color color = UnifiedRender::Color::rgba32(bswap32(0x808080ff));
            provinces_color.push_back(ProvinceColor(i, color));
            continue;
        }

        if(province.controller == gs.curr_nation) {
            UnifiedRender::Color color = UnifiedRender::Color::rgba32(bswap32(0x8080ffff));
            provinces_color.push_back(ProvinceColor(i, color));
            continue;
        }

        const NationRelation& rel = province.controller->relations[world.get_id(gs.curr_nation)];
        
        const uint8_t r = (rel.relation < 0.f) ? -rel.relation : 0.f;
        const uint8_t g = (rel.relation > 0.f) ? rel.relation : 0.f;
        const uint8_t b = g;
        UnifiedRender::Color color = UnifiedRender::Color::rgba32(bswap32(~(0x00000000 | (b << 24) | (r << 16) | (g << 8))));
        provinces_color.push_back(ProvinceColor(i, color));
    }
    return provinces_color;
}

std::string relations_tooltip(const World& world, const Province::Id id) {
    const Province& province = *world.provinces[id];
    std::string str;

    if(province.controller == nullptr) {
        str += UnifiedRender::Locale::translate("Uncontrolled");
        return str;
    }

    if(province.controller == province.owner) {
        str += UnifiedRender::Locale::translate(province.owner->get_client_hint().alt_name);
        goto form_final;
    }

    str += UnifiedRender::Locale::translate("Owned by") + " ";
    str += UnifiedRender::Locale::translate(province.owner->get_client_hint().alt_name);
    str += " " + UnifiedRender::Locale::translate("controlled by") + " ";
    str += UnifiedRender::Locale::translate(province.controller->get_client_hint().alt_name);
form_final:
    str += " ";

    const NationRelation& rel = province.controller->relations[world.get_id(((GameState&)GameState::get_instance()).curr_nation)];
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

    return str;
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
    float max_amount = 1;
    for(auto const& province : world.provinces) {
        float amount = 0;
        for(auto const& pop : province->pops) {
            amount += pop.size;
        }
        amount = log2(amount + 1.f);
        max_amount = std::max<float>(amount, max_amount);
        province_amounts.push_back(std::make_pair(world.get_id(province), amount));
    }

    // Mix each color depending of how many live there compared to max_amount
    UnifiedRender::Color min = UnifiedRender::Color::rgb8(255, 229, 217);
    UnifiedRender::Color max = UnifiedRender::Color::rgb8(220, 46, 35);
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
            } else {
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
            } else {
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