#ifndef PROVINCE_VIEW_H
#define PROVINCE_VIEW_H

#include "../../nation.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

#include "common.hpp"

namespace Interface {
    // View information related to the population and diversity of cultures and religions
    // on the specified province
    class ProvincePopulationTab : public UI::Group {
        GameState& gs;
        Province* province;

        UI::PieChart* cultures_pie;
        UI::PieChart* religions_pie;
        UI::PieChart* pop_types_pie;

        std::vector<PopInfo*> pop_infos;
    public:
        ProvincePopulationTab(GameState& gs, int x, int y, Province* province, UI::Widget* _parent);
    };

    // View information related to the economic activity of the province
    class ProvinceEconomyTab : public UI::Group {
        UI::PieChart* company_share_pie;
        UI::PieChart* products_pie;
        UI::PieChart* goods_pie;
    public:
        ProvinceEconomyTab(GameState& gs, int x, int y, Province* province, UI::Widget* _parent);

        GameState& gs;
        Province* province;
    };

    class ProvinceView : public UI::Window {
        GameState& gs;
        Province* province;

        UI::Button* pop_btn;
        ProvincePopulationTab* pop_tab;

        UI::Button* econ_btn;
        ProvinceEconomyTab* econ_tab;

        UI::CloseButton* close_btn;
    public:
        ProvinceView(GameState& gs, Province* province);
    };
};

#endif