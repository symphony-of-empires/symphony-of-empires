#include "client/interface/common.hpp"
#include "client/interface/good_view.hpp"
#include "path.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"
#include "unit.hpp"
#include "company.hpp"
#include "province.hpp"
#include "product.hpp"
#include "unified_render/texture.hpp"

using namespace Interface;

UnitButton::UnitButton(GameState& _gs, int x, int y, Unit* _unit, UI::Widget* parent)
    : gs{ _gs },
    unit{ _unit },
    UI::Button(x, y, parent->width, 24, parent)
{
    text(std::to_string(unit->size) + " " + unit->type->ref_name);
    on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<UnitButton&>(*w.parent);
        w.text(std::to_string(o.unit->size) + " " + o.unit->type->ref_name);
    });
}

UnitTypeButton::UnitTypeButton(GameState& _gs, int x, int y, UnitType* _unit_type, UI::Widget* parent)
    : gs{ _gs },
    unit_type{ _unit_type },
    UI::Group(x, y, parent->width, 24, parent)
{
    this->is_scroll = false;

    this->icon_img = new UI::Image(0, 0, 32, 24, nullptr, this);
    this->icon_img->current_texture = &g_texture_manager->load_texture(Path::get("ui/icons/unit_types/" + unit_type->ref_name));

    this->name_btn = new UI::Button(0, 0, this->width - 32, 24, this);
    this->name_btn->right_side_of(*this->icon_img);
    this->name_btn->text(unit_type->ref_name);
}

CompanyButton::CompanyButton(GameState& _gs, int x, int y, Company* _company, UI::Widget* parent)
    : gs{ _gs },
    company{ _company },
    UI::Button(x, y, parent->width, 24, parent)
{
    text(company->name);
    on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<CompanyButton&>(w);
        if(o.gs.world->time % o.gs.world->ticks_per_day) return;
        w.text(o.company->name);
    });
}

ProvinceButton::ProvinceButton(GameState& _gs, int x, int y, Province* _province, UI::Widget* parent)
    : gs{ _gs },
    province{ _province },
    UI::Button(x, y, parent->width, 24, parent)
{
    text(province->ref_name);
    on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceButton&>(w);
        w.text(o.province->ref_name);
    });
}

NationButton::NationButton(GameState& _gs, int x, int y, Nation* _nation, UI::Widget* parent)
    : gs{ _gs },
    nation{ _nation },
    UI::Group(x, y, parent->width, 24, parent)
{
    this->is_scroll = false;

    this->flag_icon = new UI::Image(0, 0, 32, 24, nullptr, this);
    this->flag_icon->current_texture = &gs.get_nation_flag(*nation);
    this->flag_icon->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationButton&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_day) return;
        w.current_texture = &o.gs.get_nation_flag(*o.nation);
    });

    this->name_btn = new UI::Button(0, 0, this->width - 32, 24, this);
    this->name_btn->right_side_of(*this->flag_icon);
    this->name_btn->text(nation->get_client_hint().alt_name);
    this->name_btn->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationButton&>(*w.parent);
        if(o.gs.world->time % o.gs.world->ticks_per_day) return;
        w.text(o.nation->get_client_hint().alt_name);
    });
}

BuildingTypeButton::BuildingTypeButton(GameState& _gs, int x, int y, BuildingType* _building_type, UI::Widget* parent)
    : gs{ _gs },
    building_type{ _building_type },
    UI::Button(x, y, parent->width, 24, parent)
{
    text(building_type->ref_name);
}

PopInfo::PopInfo(GameState& _gs, int x, int y, Province* _province, int _index, UI::Widget* parent)
    : gs{ _gs },
    province{ _province },
    index{ _index },
    UI::Group(x, y, parent->width, 24, parent)
{
    this->is_scroll = false;

    this->size_btn = new UI::Button(0, 0, 96, 24, this);

    this->budget_btn = new UI::Button(0, 0, 128, 24, this);
    this->budget_btn->right_side_of(*this->size_btn);

    this->religion_btn = new UI::Button(0, 0, 128, 24, this);
    this->religion_btn->right_side_of(*this->budget_btn);

    this->culture_btn = new UI::Button(0, 0, 128, 24, this);
    this->culture_btn->right_side_of(*this->religion_btn);

    if(index < province->pops.size()) {
        const Pop& pop = province->pops[index];
        this->size_btn->text(std::to_string(pop.size));
        this->budget_btn->text(std::to_string(pop.budget));
        this->religion_btn->text(pop.religion->name);
        this->culture_btn->text(pop.culture->name);
    }
    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<PopInfo&>(w);
        if(o.index >= o.province->pops.size()) return;

        const Pop& pop = o.province->pops[o.index];
        o.size_btn->text(std::to_string(pop.size));
        o.budget_btn->text(std::to_string(pop.budget));
        o.religion_btn->text(pop.religion->name);
        o.culture_btn->text(pop.culture->name);
    });
}

ProductInfo::ProductInfo(GameState& _gs, int x, int y, Product* _product, UI::Widget* parent)
    : gs{ _gs },
    product{ _product },
    UI::Group(x, y, parent->width, 24, parent)
{
    this->is_scroll = false;

    this->company_btn = new UI::Button(0, 0, 96, 24, this);

    this->good_btn = new UI::Button(0, 0, 96, 24, this);
    this->good_btn->text(product->good->name);
    this->good_btn->right_side_of(*this->company_btn);
    this->good_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductInfo&>(*w.parent);
        new GoodView(o.gs, o.product->good);
    });

    this->price_rate_btn = new UI::Button(0, 0, 96, 24, this);
    this->price_rate_btn->right_side_of(*this->good_btn);

    this->price_chart = new UI::Chart(0, 0, 96, 24, this);
    this->price_chart->right_side_of(*this->price_rate_btn);
    this->price_chart->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductInfo&>(*w.parent);
        new ProductView(o.gs, o.product);
    });

    this->supply_chart = new UI::Chart(0, 0, 96, 24, this);
    this->supply_chart->right_side_of(*this->price_chart);
    this->supply_chart->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductInfo&>(*w.parent);
        new ProductView(o.gs, o.product);
    });

    this->demand_chart = new UI::Chart(0, 0, 96, 24, this);
    this->demand_chart->right_side_of(*this->supply_chart);
    this->demand_chart->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductInfo&>(*w.parent);
        new ProductView(o.gs, o.product);
    });

    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductInfo&>(w);

        // Only update every ticks_per_day ticks
        if(o.gs.world->time % o.gs.world->ticks_per_day) return;

        o.price_chart->data.push_back(o.product->price);
        if(o.price_chart->data.size() >= 30)
            o.price_chart->data.pop_back();
        o.price_chart->text(std::to_string(o.product->price));
        
        o.supply_chart->data.push_back(o.product->supply);
        if(o.supply_chart->data.size() >= 30)
            o.supply_chart->data.pop_back();

        o.demand_chart->data.push_back(o.product->demand);
        if(o.demand_chart->data.size() >= 30)
            o.demand_chart->data.pop_back();

        o.company_btn->text(o.product->owner->name);
        o.price_rate_btn->text(std::to_string(o.product->price_vel));
    });
}