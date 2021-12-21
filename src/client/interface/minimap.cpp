#include "client/interface/minimap.hpp"
#include "client/map.hpp"
#include "world.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include <unordered_map>

using namespace Interface;

std::vector<ProvinceColor> terrain_map_mode(const World& world);
std::vector<ProvinceColor> terrain_color_map_mode(const World& world);
std::vector<ProvinceColor> population_map_mode(const World& world);
std::vector<ProvinceColor> culture_map_mode(const World& world);
std::vector<ProvinceColor> religion_map_mode(const World& world);

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

        mapmode_generator map_mode = political_map_mode;
        state->gs.map->set_map_mode(map_mode);
    });
    UI::Button* terrain_button = new UI::Button(10, 200, 100, 24, this);
    terrain_button->user_data = this;
    terrain_button->text("Terrain");
    terrain_button->on_click = ([](UI::Widget& w, void* data) {
        Minimap* state = (Minimap*)data;

        mapmode_generator map_mode = terrain_map_mode;
        state->gs.map->set_map_mode(map_mode);
    });
    UI::Button* population_button = new UI::Button(10, 240, 100, 24, this);
    population_button->user_data = this;
    population_button->text("Population");
    population_button->on_click = ([](UI::Widget& w, void* data) {
        Minimap* state = (Minimap*)data;

        mapmode_generator map_mode = population_map_mode;
        state->gs.map->set_map_mode(map_mode);
    });
    UI::Button* terrain_color_button = new UI::Button(110, 160, 100, 24, this);
    terrain_color_button->user_data = this;
    terrain_color_button->text("Terrain type");
    terrain_color_button->on_click = ([](UI::Widget& w, void* data) {
        Minimap* state = (Minimap*)data;

        mapmode_generator map_mode = terrain_color_map_mode;
        state->gs.map->set_map_mode(map_mode);
    });
    UI::Button* culture_button = new UI::Button(110, 200, 100, 24, this);
    culture_button->user_data = this;
    culture_button->text("Culture");
    culture_button->on_click = ([](UI::Widget& w, void* data) {
        Minimap* state = (Minimap*)data;

        mapmode_generator map_mode = culture_map_mode;
        state->gs.map->set_map_mode(map_mode);
    });
    UI::Button* religion_button = new UI::Button(110, 240, 100, 24, this);
    religion_button->user_data = this;
    religion_button->text("Religion");
    religion_button->on_click = ([](UI::Widget& w, void* data) {
        Minimap* state = (Minimap*)data;

        mapmode_generator map_mode = religion_map_mode;
        state->gs.map->set_map_mode(map_mode);
    });
}

std::vector<ProvinceColor> terrain_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        province_color.push_back(ProvinceColor(i, UI::Color::rgba32(0x00000000)));
    }
    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UI::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UI::Color::rgba32(0x00000000)));
    return province_color;
}

std::vector<ProvinceColor> terrain_color_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    for(unsigned int i = 0; i < world.provinces.size(); i++) {
        Province* province = world.provinces[i];
        UI::Color color = UI::Color::rgba32(province->terrain_type->color);
        province_color.push_back(ProvinceColor(i, color));
    }
    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UI::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UI::Color::rgba32(0x00000000)));
    return province_color;
}

std::vector<ProvinceColor> population_map_mode(const World& world) {
    // Find the maximum amount of pops in one province
    std::vector<std::pair<Province::Id, uint32_t>> province_amounts;
    uint32_t max_amount = 1;
    for(auto const& province : world.provinces) {
        uint32_t color;
        uint32_t amount = 0;
        for(auto const& pop : province->pops) {
            amount += pop.size;
        }
        max_amount = std::max<size_t>(amount, max_amount);
        province_amounts.push_back(std::make_pair(world.get_id(province), amount));
    }

    // Mix each color depending of how many live there compared to max_amount
    UI::Color min = UI::Color::rgb8(255, 229, 217);
    UI::Color max = UI::Color::rgb8(220, 46, 35);
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

std::vector<ProvinceColor> culture_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    UI::Color min = UI::Color::rgb8(255, 255, 255);
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
        UI::Color max = UI::Color::rgba32(world.cultures[max_culture_id]->color);
        UI::Color color = UI::Color::lerp(min, max, ((float)max_amount) / total_amount);
        province_color.push_back(ProvinceColor(i, color));
    }
    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UI::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UI::Color::rgba32(0xffdddddd)));
    return province_color;
}

std::vector<ProvinceColor> religion_map_mode(const World& world) {
    std::vector<ProvinceColor> province_color;
    UI::Color min = UI::Color::rgb8(255, 255, 255);
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
        UI::Color max = UI::Color::rgba32(world.religions[max_religion_id]->color);
        UI::Color color = UI::Color::lerp(min, max, ((float)max_amount) / total_amount);
        province_color.push_back(ProvinceColor(i, color));
    }
    // Water
    province_color.push_back(ProvinceColor((Province::Id)-2, UI::Color::rgba32(0x00000000)));
    // Land
    province_color.push_back(ProvinceColor((Province::Id)-1, UI::Color::rgba32(0xffdddddd)));
    return province_color;
}