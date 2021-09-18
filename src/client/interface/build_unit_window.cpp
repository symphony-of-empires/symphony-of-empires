#include "build_unit_window.hpp"

BuildUnitWindow::BuildUnitWindow(GameState& _gs, Outpost* _outpost, UI::Window* top_win)
    : gs{_gs}, outpost{_outpost}, UI::Window(0, 0, 320, 501) {
    if (outpost->type == nullptr)
        throw std::runtime_error("UNKNOWN OUTPOST TYPE?");

    text("Build unit in outpost");
    below_of(*top_win);

    UI::Button *build_type_btn = nullptr, *prev_btn = nullptr;
    if (outpost->type->is_build_land_units) {
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
				
				Command* command = new BuildUnitCommand(state->outpost, unitType);
				state->gs.add_command(command);
            };
        }
    } else if (outpost->type->is_build_naval_units) {
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
				
				Command* command = new BuildUnitCommand(state->outpost, boatType);
				state->gs.add_command(command);
            };
        }
    }
    UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, this);
    ok_btn->text("OK");
    ok_btn->below_of(*build_type_btn);
}