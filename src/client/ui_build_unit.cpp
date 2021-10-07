#include "render/texture.hpp"
#include "ui.hpp"
#include "../path.hpp"

extern std::pair<int, int> mouse_pos;

#include "../nation.hpp"
extern Nation* curr_nation;

#include "../serializer.hpp"
#include "../io_impl.hpp"
#include "client_network.hpp"

Building* g_building;
void ui_build_unit(Building* building, UI::Window* top_win) {
    if(building->type == nullptr)
        throw std::runtime_error("UNKNOWN OUTPOST TYPE?");
    
    g_building = building;
    UI::Window* build_win = new UI::Window(0, 0, 320, 501);
    build_win->text("Build unit in outpost");
    build_win->below_of(*top_win);
    
    UI::Button* build_type_btn = nullptr,* prev_btn = nullptr;
    if(building->type->is_build_land_units) {
        for(auto& unit_type: g_world->unit_types) {
            if(build_type_btn != nullptr)
                prev_btn = build_type_btn;
            build_type_btn = new UI::Button(0, 0, 303, 38, build_win);
            build_type_btn->text(unit_type->ref_name.c_str());
            build_type_btn->user_data = unit_type;
            if(prev_btn != nullptr)
                build_type_btn->below_of(*prev_btn);
            build_type_btn->on_click = [](UI::Widget&, void* data) {
                g_client->packet_mutex.lock();
                Packet packet = Packet();
                Archive ar = Archive();
                ActionType action = ActionType::BUILDING_START_BUILDING_UNIT;
                ::serialize(ar, &action);
                
                ::serialize(ar, &g_building); // BuildingRef
                ::serialize(ar, (UnitType**)&data); // UnitTypeRef

                packet.data(ar.get_buffer(), ar.size());
                g_client->packet_queue.push_back(packet);
                g_client->packet_mutex.unlock();
            };
        }
    }
    UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, build_win);
    ok_btn->text("OK");
    ok_btn->below_of(*build_type_btn);
}
