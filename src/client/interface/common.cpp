#include "common.hpp"

using namespace Interface;

PopInfo::PopInfo(GameState& _gs, int x, int y, Province* _province, int _index, UI::Widget* parent)
    : gs{ _gs },
    province{ _province },
    index{ _index },
    UI::Group(x, y, parent->width - x, 24, parent)
{
    this->is_scroll = false;
    
    this->size_btn = new UI::Button(0, 0, 96, 24, this);

    this->budget_btn = new UI::Button(0, 0, 96, 24, this);
    this->budget_btn->right_side_of(*this->size_btn);

    this->religion_btn = new UI::Button(0, 0, 96, 24, this);
    this->religion_btn->right_side_of(*this->budget_btn);

    this->culture_btn = new UI::Button(0, 0, 96, 24, this);
    this->culture_btn->right_side_of(*this->religion_btn);

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
    UI::Group(x, y, parent->width - x, 24, parent)
{
    this->is_scroll = false;

    this->company_btn = new UI::Button(0, 0, 96, 24, this);

    this->price_rate_btn = new UI::Button(0, 0, 96, 24, this);
    this->price_rate_btn->right_side_of(*this->company_btn);

    this->price_btn = new UI::Button(0, 0, 96, 24, this);
    this->price_btn->right_side_of(*this->price_rate_btn);

    this->price_chart = new UI::Chart(0, 0, 96, 24, this);
    this->price_chart->right_side_of(*this->price_btn);
    
    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProductInfo&>(w);

        // Only update every 48 ticks
        if(o.gs.world->time % 48) return;
        
        if(o.price_chart->data.size() >= 30)
            o.price_chart->data.pop_back();

        o.company_btn->text(o.product->owner->name);
        o.price_chart->data.push_back(o.product->price);
        o.price_rate_btn->text(std::to_string(o.product->price_vel));
        o.price_btn->text(std::to_string(o.product->price_vel));
    });
}