#pragma once

#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "client/ui.hpp"

namespace Interface {
    // Pretty brief summaries (for example to click oon a button from a list)
    class UnitButton : public UI::Group {
        GameState& gs;
        UI::Button* name_btn;
    public:
        Unit* unit;
        UnitButton(GameState& gs, int x, int y, Unit* unit, UI::Widget* parent);
    };

    class CompanyButton : public UI::Group {
        GameState& gs;
        UI::Button* name_btn;
    public:
        Company* company;
        CompanyButton(GameState& gs, int x, int y, Company* company, UI::Widget* parent);
    };

    class ProvinceButton : public UI::Group {
        GameState& gs;
        UI::Button* name_btn;
    public:
        Province* province;
        ProvinceButton(GameState& gs, int x, int y, Province* province, UI::Widget* parent);
    };

    class NationButton : public UI::Group {
        GameState& gs;
        UI::Image* flag_icon;
        UI::Button* name_btn;
    public:
        Nation* nation;
        NationButton(GameState& gs, int x, int y, Nation* nation, UI::Widget* parent);
    };

    // More extended information about these elements (information display, for example)
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
        UI::Button* good_btn;
        UI::Button* price_btn,* price_rate_btn;
        UI::Chart* price_chart;
    public:
        ProductInfo(GameState& gs, int x, int y, Product* product, UI::Widget* parent);
    };
};