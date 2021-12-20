#include "client/interface/minimap.hpp"
#include "client/map.hpp"
#include "world.hpp"
#include "path.hpp"
#include "unified_render/texture.hpp"

using namespace Interface;

std::vector<ProvinceColor> terrain_map_mode(World* world);
std::vector<ProvinceColor> political_map_mode(World* world);
std::vector<ProvinceColor> population_map_mode(World* world);

Minimap::Minimap(GameState& _gs, int x, int y, UI_Origin origin)
    : gs{ _gs },
    UI::Window(x, y, 300, 300)
{
    this->origin = origin;
    this->is_pinned = true;
    this->is_scroll = false;
    this->text("Minimap");

    const UnifiedRender::Texture& map_texture = g_texture_manager->load_texture(Path::get("ui/globe.png"));

    new UI::Image(0, 0, 300, 150, &map_texture, this);

    UI::Button* political_button = new UI::Button(10, 160, 100, 24, this);
    political_button->user_data = this;
    political_button->text("Political");
    political_button->on_click = ([](UI::Widget& w, void* data) {
        Minimap* state = (Minimap*)data;

        auto map_mode = political_map_mode(state->gs.world);
        state->gs.map->set_map_mode(map_mode);
    });
    UI::Button* terrain_button = new UI::Button(10, 200, 100, 24, this);
    terrain_button->user_data = this;
    terrain_button->text("Terrain");
    terrain_button->on_click = ([](UI::Widget& w, void* data) {
        Minimap* state = (Minimap*)data;

        auto map_mode = terrain_map_mode(state->gs.world);
        state->gs.map->set_map_mode(map_mode);
    });
    UI::Button* population_button = new UI::Button(10, 240, 100, 24, this);
    population_button->user_data = this;
    population_button->text("Population");
    population_button->on_click = ([](UI::Widget& w, void* data) {
        Minimap* state = (Minimap*)data;

        auto map_mode = population_map_mode(state->gs.world);
        state->gs.map->set_map_mode(map_mode);
    });
}

std::vector<ProvinceColor> terrain_map_mode(World* world) {
    std::vector<ProvinceColor> province_color;
    for(unsigned int i = 0; i < world->provinces.size(); i++) {
        province_color.push_back(ProvinceColor(i, UI::Color::rgba32(0x00000000)));
    }
    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UI::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UI::Color::rgba32(0x00000000)));
    return province_color;
}
std::vector<ProvinceColor> political_map_mode(World* world) {
    std::vector<ProvinceColor> province_color;
    for(unsigned int i = 0; i < world->provinces.size(); i++) {
        Nation* province_owner = world->provinces[i]->owner;
        if(province_owner == nullptr) {
            province_color.push_back(ProvinceColor(i, UI::Color::rgba32(0xffdddddd)));
        }
        else if(province_owner->cached_id == (Nation::Id)-1) {
            province_color.push_back(ProvinceColor(i, UI::Color::rgba32(0xffdddddd)));
        }
        else {
            province_color.push_back(ProvinceColor(i, UI::Color::rgba32(province_owner->get_client_hint().color)));
        }
    }
    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UI::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UI::Color::rgba32(0xffdddddd)));
    return province_color;
}

std::vector<ProvinceColor> population_map_mode(World* world) {
    std::vector<std::pair<Province::Id, uint32_t>> province_amounts;
    uint32_t max_amount = 0;
    for(auto const& province : world->provinces) {
        uint32_t color;
        uint32_t amount = 0;
        for(auto const& pop : province->pops) {
            amount += pop.size;
        }
        max_amount = std::max<size_t>(amount, max_amount);
        province_amounts.push_back(std::make_pair(world->get_id(province), amount));
    }
    max_amount = std::max<uint32_t>(1, max_amount);
    UI::Color max = UI::Color::rgb8(255, 229, 217);
    UI::Color min = UI::Color::rgb8(220, 46, 35);
    std::vector<ProvinceColor> province_color;
    for(auto const& prov_amount : province_amounts) {
        Province::Id prov_id = prov_amount.first;
        uint32_t amount = prov_amount.second;
        float ratio = ((float)amount) / max_amount;
        UI::Color color = UI::Color::lerp(min, max, ratio);
        province_color.push_back(ProvinceColor(prov_id, color));
    }
    return province_color;
}