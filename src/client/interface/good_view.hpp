#pragma once

#include "good.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui/window.hpp"

namespace UI {
    class Label;
    class Image;
    class PieChart;
    class CloseButton;
    class Chart;
    class Group;
}

namespace Interface {
    class ProductView : public UI::Window {
        GameState& gs;
        Product* product;

        UI::Image* icon_img;
        UI::Label* name_lab;
        UI::PieChart *supply_pie;
        UI::Chart* price_chart,* supply_chart,* demand_chart;

        UI::CloseButton* close_btn;
    public:
        ProductView(GameState& gs, Product* product);
    };

    class GoodView : public UI::Window {
        GameState& gs;
        Good* good;

        UI::Image* icon_img;
        UI::Label* name_lab;
        UI::PieChart *sellers_pie;

        UI::Chart* avg_price_chart;

        UI::Group* producing_it_group;
        UI::Group* consumer_it_group;

        UI::CloseButton* close_btn;
    public:
        GoodView(GameState& gs, Good* good);
    };
};