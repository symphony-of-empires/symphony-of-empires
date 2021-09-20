#include "build_unit_window.hpp"

BuildUnitWindow::BuildUnitWindow(GameState& _gs, Building* _building, UI::Window* top_win)
    : gs{_gs}, building{_building}, UI::Window(0, 0, 320, 501) {
    if (building->type == nullptr)
        throw std::runtime_error("Unknown building type?");

    text("Build unit in building");
    below_of(*top_win);

    UI::Button *build_type_btn = nullptr, *prev_btn = nullptr;
    if (building->type->is_build_land_units) {
        for (auto& unit_type : gs.world->unit_types) {
            if (build_type_btn != nullptr)
                prev_btn = build_type_btn;
            build_type_btn = new UI::Button(0, 0, 303, 38, this);
            build_type_btn->text(unit_type->ref_name.c_str());
            build_type_btn->user_data = unit_type;
            if (prev_btn != nullptr)
                build_type_btn->below_of(*prev_btn);

            build_type_btn->on_click = [](UI::Widget& w, void* data) {
                BuildUnitWindow* state = (BuildUnitWindow*)w.parent;
				UnitType* unitType = (UnitType*)data;
				
				Command* command = new BuildUnitCommand(state->building, unitType);
				state->gs.add_command(command);
            };
        }
    } else if (building->type->is_build_naval_units) {
        for (auto& boat_type : gs.world->boat_types) {
            if (build_type_btn != nullptr)
                prev_btn = build_type_btn;
            build_type_btn = new UI::Button(0, 0, 303, 38, this);
            build_type_btn->text(boat_type->ref_name.c_str());
            build_type_btn->user_data = boat_type;
            if (prev_btn != nullptr)
                build_type_btn->below_of(*prev_btn);

            build_type_btn->on_click = [](UI::Widget& w, void* data) {
                BuildUnitWindow* state = (BuildUnitWindow*)w.parent;
				BoatType* boatType = (BoatType*)data;
				
				Command* command = new BuildUnitCommand(state->building, boatType);
				state->gs.add_command(command);
            };
        }
    }
    UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, this);
    ok_btn->text("OK");
    ok_btn->below_of(*build_type_btn);
}