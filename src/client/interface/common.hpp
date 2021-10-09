#ifndef COMMON_H
#define COMMON_H

#include "../../nation.hpp"
#include "../../world.hpp"
#include "../game_state.hpp"
#include "../ui.hpp"

namespace Interface {
    class PopInfo : public UI::Group {
        GameState& gs;
        Province* province;
        int index;

        UI::Image* type_icon;
        UI::Button* size_btn,* culture_btn,* religion_btn,* budget_btn;
    public:
        PopInfo(GameState& gs, int x, int y, Province* province, int index, UI::Widget* parent);
    };

    class ProductInfo : public UI::Group {
        GameState& gs;
        Product* product;

        UI::Image* good_icon;
        UI::Button* name_btn,* company_btn,* province_btn;
        UI::Button* price_btn,* price_rate_btn;
        UI::Chart* price_chart;
    public:
        ProductInfo(GameState& gs, int x, int y, Product* product, UI::Widget* parent);
    };
};

#endif