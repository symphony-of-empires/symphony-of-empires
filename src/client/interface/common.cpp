#include "common.hpp"

using namespace Interface;

PopInfo::PopInfo(GameState& _gs, int x, int y, Province* _province, int _index, UI::Widget* parent)
    : gs{ _gs },
    province{ _province },
    index{ _index },
    UI::Group(x, y, parent->width - x, 24, parent)
{
    x = 0;
    this->size_btn = new UI::Button(x, 0, 96, 24, this);
    x += 96;
    this->budget_btn = new UI::Button(x, 0, 96, 24, this);
    x += 96;
    this->religion_btn = new UI::Button(x, 0, 96, 24, this);
    x += 96;
    this->culture_btn = new UI::Button(x, 0, 96, 24, this);

    this->on_each_tick = ([](UI::Widget& w, void*) {
        PopInfo& o = dynamic_cast<PopInfo&>(w);
        if(o.index >= o.province->pops.size()) {
            return;
        }

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
    x = 0;
    this->company_btn = new UI::Button(x, 0, 96, 24, this);
    this->company_btn->text(product->owner->name);
    x += 96;
    this->price_rate_btn = new UI::Button(x, 0, 96, 24, this);
    this->price_rate_btn->text(std::to_string(product->price_vel));
    x += 96;
    this->price_btn = new UI::Button(x, 0, 96, 24, this);
    this->price_btn->text(std::to_string(product->price_vel));
    x += 96;
    this->price_chart = new UI::Chart(x, 0, 96, 24, this);
    
    this->on_each_tick = ([](UI::Widget& w, void*) {
        ProductInfo& o = dynamic_cast<ProductInfo&>(w);

        // Only update every 48 ticks
        if(o.gs.world->time % 48) {
            return;
        }
        
        if(o.price_chart->data.size() >= 30) {
            o.price_chart->data.pop_back();
        }
        o.price_chart->data.push_back(o.product->price);
    });
}