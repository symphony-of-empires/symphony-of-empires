#include "client/interface/army.hpp"
#include "client/map.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"

using namespace Interface;

ArmyArmyTab::ArmyArmyTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : gs{ _gs },
    UI::Group(x, y, parent->width - x, parent->height - y, parent)
{
    unsigned int i = 0;
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
    UI::Group(x, y, parent->width - x, parent->height - y, parent)
{
    unsigned int i = 0;
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
    UI::Group(x, y, parent->width - x, parent->height - y, parent)
{
    unsigned int i = 0;
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

ArmyProductionTab::ArmyProductionTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : gs{ _gs },
    UI::Group(x, y, parent->width - x, parent->height - y, parent)
{
    // Chart showing total number of required materials
    this->reqmat_chart = new UI::Chart(0, 0, 128, 128, this);
    this->reqmat_chart->text("Material demand");
    this->reqmat_chart->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyProductionTab&>(*w.parent);

        float reqtotal = 0.f;
        for(const auto& building : o.gs.world->buildings) {
            if(building->get_owner() != o.gs.curr_nation) continue;

            for(const auto& req : building->req_goods_for_unit) {
                reqtotal += req.second;
            }

            for(const auto& req : building->req_goods) {
                reqtotal += req.second;
            }
        }

        o.reqmat_chart->data.push_back(reqtotal);
        if(o.reqmat_chart->data.size() >= 30)
            o.reqmat_chart->data.pop_back();
    });

    unsigned int i = 0;
    for(const auto& building : gs.world->buildings) {
        if(building->get_owner() != gs.curr_nation) continue;
        auto* btn = new ArmyProductionUnitInfo(gs, 0, 128 + (24 * i), building, this);
        i++;
    }
}

ArmyProductionUnitInfo::ArmyProductionUnitInfo(GameState& _gs, int x, int y, Building* _building, UI::Widget* parent)
    : gs{ _gs },
    building{ _building },
    UI::Group(x, y, parent->width - x, parent->height - y, parent)
{
    this->is_scroll = false;

    this->unit_icon = new UI::Image(0, 0, 24, 24, nullptr, this);
    if(building->working_unit_type != nullptr) {
        this->unit_icon->current_texture = &g_texture_manager->load_texture(Path::get("ui/icons/unit_types/" + building->working_unit_type->ref_name + ".png"));
    }

    this->province_lab = new UI::Label(0, 0, "?", this);
    this->province_lab->right_side_of(*this->unit_icon);
    if(building->get_province() != nullptr)
        this->province_lab->text(building->get_province()->ref_name);
    this->province_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyProductionUnitInfo&>(*w.parent);
        if(o.building->get_province() != nullptr)
            w.text(o.building->get_province()->ref_name);
    });

    this->company_lab = new UI::Label(0, 0, "?", this);
    this->company_lab->right_side_of(*this->province_lab);
    this->company_lab->text(building->corporate_owner->name);
    this->company_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyProductionUnitInfo&>(*w.parent);
        w.text(o.building->corporate_owner->name);
    });

    this->name_lab = new UI::Label(0, 0, "?", this);
    this->name_lab->right_side_of(*this->company_lab);
    this->name_lab->text((building->working_unit_type != nullptr) ? building->working_unit_type->ref_name : "No unit");
    this->name_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyProductionUnitInfo&>(*w.parent);
        w.text((o.building->working_unit_type != nullptr) ? o.building->working_unit_type->ref_name : "No unit");
    });

    auto* progress_pgbar = new UI::ProgressBar(0, 0, 128, 24, 0.f, 1.f, this);
    progress_pgbar->right_side_of(*this->name_lab);
    progress_pgbar->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyProductionUnitInfo&>(*w.parent);

        std::string text = "";

        size_t full = 0, needed = 0;
        text = "Needs ";
        for(size_t i = 0; i < o.building->req_goods_for_unit.size(); i++) {
            auto need_req = o.building->req_goods_for_unit[i];
            auto full_req = o.building->working_unit_type->req_goods[i];

            full_req.second -= need_req.second;

            full += full_req.second;
            needed += need_req.second;

            text += std::to_string(need_req.second) + " of " + need_req.first->name + " (has " + std::to_string(full_req.second) + "), ";
        }

        ((UI::ProgressBar&)w).value = (float)full / (float)needed;
        w.tooltip->text(text);
    });
    progress_pgbar->tooltip = new UI::Tooltip(progress_pgbar, 512, 24);
}

ArmySelectUnitTab::ArmySelectUnitTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : gs{ _gs },
    UI::Group(x, y, parent->width - x, parent->height - y, parent)
{
    unsigned int i = 0;
    for(const auto& unit_type : gs.world->unit_types) {
        auto* btn = new UnitTypeButton(gs, 0, 24 * i, unit_type, this);
        btn->user_data = unit_type;
        btn->on_click = ([](UI::Widget& w, void* data) {
            auto& o = static_cast<ArmyView&>(*w.parent->parent);
            o.new_unit_tab->unit_type = (UnitType*)data;

            // This tab gets hidden and "pass control" to the new_unit_tab
            o.new_unit_tab->is_render = true;
            o.select_unit_tab->is_render = false;
        });
        i++;
    }
}

ArmyNewUnitTab::ArmyNewUnitTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : gs{ _gs },
    UI::Group(x, y, parent->width - x, parent->height - y, parent)
{
    auto* select_btn = new UI::Button(0, 0, 128, 24, this);
    select_btn->text("Select type");
    select_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent->parent);

        // "Give control" to the selection tab - they will return us control after selecting an unit type
        w.parent->is_render = false;
        o.select_unit_tab->is_render = true;
    });

    auto* create_btn = new UI::Button(0, 0, 128, 24, this);
    create_btn->text("Create");
    create_btn->below_of(*select_btn);
    create_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyNewUnitTab&>(*w.parent);
        o.gs.production_queue.push_back(o.unit_type);
    });
}

ArmyView::ArmyView(GameState& _gs)
    : gs{ _gs },
    UI::Window(0, 0, 800, 256)
{
    this->is_scroll = false;
    this->text("Army manager");

    this->army_tab = new ArmyArmyTab(gs, 128 + 24, 0, this);
    this->army_tab->is_render = true;
    auto* army_btn = new UI::Button(0, 0, 128, 24, this);
    army_btn->text("Army");
    army_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = true;
        o.airforce_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.production_tab->is_render = false;
        o.new_unit_tab->is_render = false;
        o.select_unit_tab->is_render = false;
    });

    this->airforce_tab = new ArmyAirforceTab(gs, 128 + 24, 0, this);
    this->airforce_tab->is_render = false;
    auto* airforce_btn = new UI::Button(0, 0, 128, 24, this);
    airforce_btn->below_of(*army_btn);
    airforce_btn->text("Airforce");
    airforce_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = false;
        o.airforce_tab->is_render = true;
        o.navy_tab->is_render = false;
        o.production_tab->is_render = false;
        o.new_unit_tab->is_render = false;
        o.select_unit_tab->is_render = false;
    });

    this->navy_tab = new ArmyNavyTab(gs, 128 + 24, 0, this);
    this->navy_tab->is_render = false;
    auto* navy_btn = new UI::Button(0, 0, 128, 24, this);
    navy_btn->below_of(*airforce_btn);
    navy_btn->text("Navy");
    navy_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = false;
        o.airforce_tab->is_render = false;
        o.navy_tab->is_render = true;
        o.production_tab->is_render = false;
        o.new_unit_tab->is_render = false;
        o.select_unit_tab->is_render = false;
    });

    this->production_tab = new ArmyProductionTab(gs, 128 + 24, 0, this);
    this->production_tab->is_render = false;
    auto* production_btn = new UI::Button(0, 0, 128, 24, this);
    production_btn->below_of(*navy_btn);
    production_btn->text("Production");
    production_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = false;
        o.airforce_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.production_tab->is_render = true;
        o.new_unit_tab->is_render = false;
        o.select_unit_tab->is_render = false;
    });

    this->new_unit_tab = new ArmyNewUnitTab(gs, 128 + 24, 0, this);
    this->new_unit_tab->is_render = false;
    auto* new_unit_btn = new UI::Button(0, 0, 128, 24, this);
    new_unit_btn->below_of(*production_btn);
    new_unit_btn->text("New Unit");
    new_unit_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ArmyView&>(*w.parent);

        o.army_tab->is_render = false;
        o.airforce_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.production_tab->is_render = false;
        o.new_unit_tab->is_render = true;
        o.select_unit_tab->is_render = false;
    });

    // Hidden - only rendered on invokation and does not store state on the View directly
    // rather it stores it on the new_unit_tab
    this->select_unit_tab = new ArmySelectUnitTab(gs, 128 + 24, 0, this);
    this->select_unit_tab->is_render = false;

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*new_unit_btn);
    close_btn->text("Close");
}