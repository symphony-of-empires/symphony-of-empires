#include "client/interface/army.hpp"

using namespace Interface;

ArmyArmyTab::ArmyArmyTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : gs{ _gs },
    UI::Group(x, y, parent->width - x, parent->height -y, parent)
{
    uint i = 0;
    for(const auto& unit : gs.world->units) {
        if(unit->owner != gs.curr_nation) continue;
        if(!(unit->type->is_ground == true && unit->type->is_naval == false)) continue;

        auto* btn = new UnitButton(gs, 0, 24 * i, unit, this);
        btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ArmyArmyTab&>(*w.parent);
            //static_cast<UnitButton&>(w).unit;
        });
        i++;
    }
}

ArmyAirforceTab::ArmyAirforceTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : gs{ _gs },
    UI::Group(x, y, parent->width - x, parent->height -y, parent)
{
    uint i = 0;
    for(const auto& unit : gs.world->units) {
        if(unit->owner != gs.curr_nation) continue;
        if(!(unit->type->is_ground == true && unit->type->is_naval == true)) continue;

        auto* btn = new UnitButton(gs, 0, 24 * i, unit, this);
        btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ArmyAirforceTab&>(*w.parent);
            //static_cast<UnitButton&>(w).unit;
        });
        i++;
    }
}

ArmyNavyTab::ArmyNavyTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : gs{ _gs },
    UI::Group(x, y, parent->width - x, parent->height -y, parent)
{
    uint i = 0;
    for(const auto& unit : gs.world->units) {
        if(unit->owner != gs.curr_nation) continue;
        if(!(unit->type->is_ground == false && unit->type->is_naval == true)) continue;

        auto* btn = new UnitButton(gs, 0, 24 * i, unit, this);
        btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<ArmyNavyTab&>(*w.parent);
            //static_cast<UnitButton&>(w).unit;
        });
        i++;
    }
}

ArmyView::ArmyView(GameState& _gs)
    : gs{ _gs },
    UI::Window(0, 0, 512, 512)
{
    this->is_scroll = false;
    this->text("Army manager");

    this->army_tab = new ArmyArmyTab(gs, 128, 0, this);
    this->army_tab->is_render = false;
    this->airforce_tab = new ArmyAirforceTab(gs, 128, 0, this);
    this->airforce_tab->is_render = false;
    this->navy_tab = new ArmyNavyTab(gs, 128, 0, this);
    this->navy_tab->is_render = false;

    auto* army_btn = new UI::Button(0, 0, 128, 24, this);
    army_btn->text("Army");
    army_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = true;
        o.airforce_tab->is_render = false;
        o.navy_tab->is_render = false;
    });

    auto* airforce_btn = new UI::Button(0, 0, 128, 24, this);
    airforce_btn->below_of(*army_btn);
    airforce_btn->text("Airforce");
    airforce_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = false;
        o.airforce_tab->is_render = true;
        o.navy_tab->is_render = false;
    });

    auto* navy_btn = new UI::Button(0, 0, 128, 24, this);
    navy_btn->below_of(*airforce_btn);
    navy_btn->text("Navy");
    navy_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = false;
        o.airforce_tab->is_render = false;
        o.navy_tab->is_render = true;
    });

    auto* build_btn = new UI::Button(0, 0, 128, 24, this);
    build_btn->below_of(*navy_btn);
    build_btn->text("Build");

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*build_btn);
    close_btn->text("Close");
}