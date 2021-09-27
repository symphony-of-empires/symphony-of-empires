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

ProvinceView::ProvinceView(GameState& _gs, UI::Window* top_win, const Tile& tile) : gs{_gs}, provinceId{tile.province_id} {
    World* world = gs.world;

    UI::Window* province_view_win = new UI::Window(0, 0, 320, 425);
    province_view_win->text("province overview");
    province_view_win->below_of(*top_win);

    selected_province = world->provinces[tile.province_id];

    const UnifiedRender::Texture& province_view_decor = g_texture_manager->load_texture(Path::get("ui/province_view_terrain.png"));
    UI::Image* view_province_decor = new UI::Image(9, 43, province_view_decor.width, province_view_decor.height, &province_view_decor, province_view_win);
    view_province_decor->text(world->provinces[tile.province_id]->name.c_str());

    UI::Button* view_province_pops = new UI::Button(9, 193, 303, 38, province_view_win);
    view_province_pops->text("Population");
    view_province_pops->user_data = (void*)world->provinces[tile.province_id];

    UI::Button* view_province_ind = new UI::Button(9, 0, 303, 38, province_view_win);
    view_province_ind->text("Economic activity");
    view_province_ind->below_of(*view_province_pops);
    view_province_ind->user_data = (void*)world->provinces[tile.province_id];

    UI::Button* view_province_owner = new UI::Button(9, 0, 303, 38, province_view_win);
    view_province_owner->text("Owner info");
    view_province_owner->below_of(*view_province_ind);
    view_province_owner->user_data = (void*)world->provinces[tile.province_id];

    UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, province_view_win);
    ok_btn->text("ok");
    ok_btn->below_of(*view_province_owner);

    if (world->provinces[tile.province_id]->owner == nullptr) {
        UI::Button* colonize_province_btn = new UI::Button(9, 0, 303, 38, province_view_win);
        colonize_province_btn->text("Colonize");
        colonize_province_btn->below_of(*ok_btn);
        colonize_province_btn->user_data = this;
        colonize_province_btn->on_click = (UI::Callback)colonize_province_cb;
    }
}