#pragma once

#include "client/ui.hpp"
class World;
class Nation;
class GameState;
class Unit;
class UnitType;
class Company;
class Province;
class Product;
class BuildingType;

namespace Interface {
    // Pretty brief summaries (for example to click oon a button from a list)
    class UnitButton : public UI::Button {
        GameState& gs;
    public:
        Unit* unit;
        UnitButton(GameState& gs, int x, int y, Unit* unit, UI::Widget* parent);
    };

    class UnitTypeButton : public UI::Group {
        GameState& gs;
        UI::Image* icon_img;
        UI::Button* name_btn;
    public:
        UnitType* unit_type;
        UnitTypeButton(GameState& gs, int x, int y, UnitType* unit_type, UI::Widget* parent);
    };

    class CompanyButton : public UI::Button {
        GameState& gs;
    public:
        Company* company;
        CompanyButton(GameState& gs, int x, int y, Company* company, UI::Widget* parent);
    };

    class ProvinceButton : public UI::Button {
        GameState& gs;
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

    class BuildingTypeButton : public UI::Button {
        GameState& gs;
        UI::Button* name_btn;
    public:
        BuildingType* building_type;
        BuildingTypeButton(GameState& gs, int x, int y, BuildingType* building_type, UI::Widget* parent);
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