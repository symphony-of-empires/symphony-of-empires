#include "province_view.hpp"

#include "../../nation.hpp"
#include "../../path.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../render/texture.hpp"
#include "../ui.hpp"

void ProvinceView::colonize_province_cb(UI::Widget& w, ProvinceView* data) {
    // TODO send package
    // size_t& curr_selected_nation = data->gs.select_nation->curr_selected_nation;
    // data->selected_province->owner = data->gs.world->nations[curr_selected_nation];
    // data->gs.curr_nation->budget -= 10000;
}

ProvinceView::ProvinceView(GameState& _gs, const Tile& tile)
    : gs{_gs},
      UI::Window(0, 0, 320, 400) {
    
    World& world = *gs.world;

    province = world.provinces[tile.province_id];
    text(province->name.c_str());

    const UnifiedRender::Texture& province_view_decor = g_texture_manager->load_texture(Path::get("ui/province_view_terrain.png"));
    UI::Image* view_province_decor = new UI::Image(9, 43, province_view_decor.width, province_view_decor.height, &province_view_decor, this);
    view_province_decor->text(province->name.c_str());

    UI::Button* view_province_pops = new UI::Button(9, 193, 303, 38, this);
    view_province_pops->text("Population");
    view_province_pops->user_data = (void*)province;

    UI::Button* view_province_ind = new UI::Button(9, 0, 303, 38, this);
    view_province_ind->text("Economic activity");
    view_province_ind->below_of(*view_province_pops);
    view_province_ind->user_data = (void*)this;
    view_province_ind->on_click = (UI::Callback)([](UI::Widget& w, void *data) {
        ProvinceView *state = (ProvinceView*)data;
        new ProvinceViewEconomic(state->gs, state->province);
    });

    UI::Button* view_province_owner = new UI::Button(9, 0, 303, 38, this);
    view_province_owner->text("Owner info");
    view_province_owner->below_of(*view_province_ind);
    view_province_owner->user_data = (void*)province;

    UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, this);
    ok_btn->text("ok");
    ok_btn->below_of(*view_province_owner);

    if (world.provinces[tile.province_id]->owner == nullptr) {
        UI::Button* colonize_province_btn = new UI::Button(9, 0, 303, 38, this);
        colonize_province_btn->text("Colonize");
        colonize_province_btn->below_of(*ok_btn);
        colonize_province_btn->user_data = this;
        colonize_province_btn->on_click = (UI::Callback)colonize_province_cb;
    }
}

ProvinceViewEconomic::ProvinceViewEconomic(GameState& _gs, Province* _province)
    : gs{_gs},
      province{_province},
      UI::Window(0, 0, 320, 400) {
    
    UI::CloseButton* ok_btn = new UI::CloseButton(9, 24, 303, 38, this);
    ok_btn->text("ok");

    UI::Chart* pop_chart = new UI::Chart(0, 32, 120, 64, this);
    pop_chart->below_of(*ok_btn);
    pop_chart->user_data = this;
    pop_chart->on_update = (UI::Callback)([](UI::Widget& w, void* data) {
        ProvinceViewEconomic *state = (ProvinceViewEconomic*)data;
        UI::Chart& wc = dynamic_cast<UI::Chart&>(w);
        if (state->gs.world->time % 48 == 35) {
            wc.data.push_back(state->province->total_pops());
            if (wc.data.size() >= 30)
                wc.data.pop_front();
        }
    });
}