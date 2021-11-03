#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui.hpp"
#include "client/interface/common.hpp"

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

        std::vector<ProductInfo*> product_infos;
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

        UI::Button* nation_btn;

        UI::CloseButton* close_btn;
    public:
        ProvinceView(GameState& gs, Province* province);
    };

    // Brief
    class ProvinceBrief : public UI::Window {
        UI::PieChart* cultures_pie;
        UI::Chart* economy_chart;
    public:
        ProvinceBrief(GameState& gs, Province* province);
    };
};