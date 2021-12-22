#include "client/interface/top_window.hpp"
#include "client/game_state.hpp"
#include "unified_render/path.hpp"
#include "client/map.hpp"
#include "world.hpp"
#include "unified_render/texture.hpp"
#include "client/interface/policies.hpp"
#include "client/interface/army.hpp"

using namespace Interface;

TopWindow::TopWindow(GameState& _gs)
    : gs{ _gs },
    UI::Group(0, 0, _gs.width, 128)
{
    this->is_scroll = false;
    this->is_pinned = true;
    
    auto* policy_ibtn = new UI::Image(0, 0, 32, 32, &g_texture_manager->load_texture(Path::get("ui/icons/literacy.png")), this);
    policy_ibtn->on_click = (UI::Callback)([](UI::Widget& w, void*) {
        auto& o = static_cast<TopWindow&>(*w.parent);
        new Interface::PoliciesScreen(o.gs);
    });
    policy_ibtn->tooltip = new UI::Tooltip(policy_ibtn, 512, 24);
    policy_ibtn->tooltip->text("Laws & Policies");

    auto* economy_ibtn = new UI::Image(0, 0, 32, 32, &g_texture_manager->load_texture(Path::get("ui/icons/economy_score.png")), this);
    economy_ibtn->right_side_of(*policy_ibtn);
    economy_ibtn->on_click = (UI::Callback)([](UI::Widget& w, void*) {
        auto& o = static_cast<TopWindow&>(*w.parent);
        
    });
    economy_ibtn->tooltip = new UI::Tooltip(economy_ibtn, 512, 24);
    economy_ibtn->tooltip->text("Economy & World Market");

    auto* military_ibtn = new UI::Image(0, 0, 32, 32, &g_texture_manager->load_texture(Path::get("ui/icons/militar_score.png")), this);
    military_ibtn->right_side_of(*economy_ibtn);
    military_ibtn->on_click = (UI::Callback)([](UI::Widget& w, void*) {
        auto& o = static_cast<TopWindow&>(*w.parent);
        new Interface::ArmyView(o.gs);
    });
    military_ibtn->tooltip = new UI::Tooltip(military_ibtn, 512, 24);
    military_ibtn->tooltip->text("Military");

    auto* speed0_btn = new UI::Button(0, 0, 48, 24, this);
    speed0_btn->below_of(*military_ibtn);
    speed0_btn->text("||");
    speed0_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TopWindow&>(*w.parent);
        o.gs.paused = true;
    });
    speed0_btn->tooltip = new UI::Tooltip(speed0_btn, 512, 24);
    speed0_btn->tooltip->text("Pause");

    auto* speed1_btn = new UI::Button(0, 0, 48, 24, this);
    speed1_btn->below_of(*military_ibtn);
    speed1_btn->right_side_of(*speed0_btn);
    speed1_btn->text(">");
    speed1_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TopWindow&>(*w.parent);
        o.gs.paused = false;
        o.gs.ms_delay_speed = 1000;
    });
    speed1_btn->tooltip = new UI::Tooltip(speed1_btn, 512, 24);
    speed1_btn->tooltip->text("Turtle speed");

    auto* speed2_btn = new UI::Button(0, 0, 48, 24, this);
    speed2_btn->below_of(*military_ibtn);
    speed2_btn->right_side_of(*speed1_btn);
    speed2_btn->text(">>");
    speed2_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TopWindow&>(*w.parent);
        o.gs.paused = false;
        o.gs.ms_delay_speed = 500;
    });
    speed2_btn->tooltip = new UI::Tooltip(speed2_btn, 512, 24);
    speed2_btn->tooltip->text("Horse speed");

    auto* speed3_btn = new UI::Button(0, 0, 48, 24, this);
    speed3_btn->below_of(*military_ibtn);
    speed3_btn->right_side_of(*speed2_btn);
    speed3_btn->text(">>>");
    speed3_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TopWindow&>(*w.parent);
        o.gs.paused = false;
        o.gs.ms_delay_speed = 50;
    });
    speed3_btn->tooltip = new UI::Tooltip(speed3_btn, 512, 24);
    speed3_btn->tooltip->text("Fire speed");
}