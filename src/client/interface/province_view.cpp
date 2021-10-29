#include "province_view.hpp"
#include "nation_view.hpp"

#include "../game_state.hpp"
#include "../../nation.hpp"
#include "../../world.hpp"

using namespace Interface;

ProvincePopulationTab::ProvincePopulationTab(GameState& _gs, int x, int y, Province* _province, UI::Widget* _parent)
    : gs{ _gs },
    province{ _province },
    UI::Group(x, y, _parent->width - x, _parent->height - y, _parent)
{
    this->text(province->name);

    this->cultures_pie = new UI::PieChart(0, 0, 128, 128, this);

    this->religions_pie = new UI::PieChart(0, 0, 128, 128, this);
    this->religions_pie->right_side_of(*this->cultures_pie);

    this->pop_types_pie = new UI::PieChart(0, 0, 128, 128, this);
    this->pop_types_pie->right_side_of(*this->religions_pie);

    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvincePopulationTab&>(w);

        // Obtain population information
        // TODO: This is not correct!! - we are calculating per each pop, we are supposed
        // to merge pops from same culture/religion/type into a single dataset
        std::vector<UI::ChartData> cultures_data, religions_data, pop_types_data;
        for(const auto& pop : o.province->pops) {
            const auto culture_col = UI::Color(
                o.gs.world->get_id(pop.culture) * 12,
                o.gs.world->get_id(pop.culture) * 31,
                o.gs.world->get_id(pop.culture) * 97
            );

            const auto religion_col = UI::Color(
                o.gs.world->get_id(pop.religion) * 12,
                o.gs.world->get_id(pop.religion) * 31,
                o.gs.world->get_id(pop.religion) * 97
            );

            const auto type_col = UI::Color(
                o.gs.world->get_id(pop.type) * 12,
                o.gs.world->get_id(pop.type) * 31,
                o.gs.world->get_id(pop.type) * 97
            );

            cultures_data.push_back(UI::ChartData(
                pop.size,
                pop.culture->name,
                culture_col
            ));

            religions_data.push_back(UI::ChartData(
                pop.size,
                pop.religion->name,
                religion_col
            ));
            
            pop_types_data.push_back(UI::ChartData(
                pop.size,
                pop.type->name,
                type_col
            ));
        }

        o.cultures_pie->set_data(cultures_data);
        o.religions_pie->set_data(religions_data);
        o.pop_types_pie->set_data(pop_types_data);

        if(o.pop_infos.size() < o.province->pops.size()) {
            for(size_t i = o.pop_infos.size(); i < o.province->pops.size(); i++) {
                PopInfo* info = new PopInfo(o.gs, 0, (i * 24) + 24, o.province, i, &o);
                o.pop_infos.push_back(info);
            }
        }
        else if(o.pop_infos.size() > o.province->pops.size()) {
            for(size_t i = o.province->pops.size(); i < o.pop_infos.size(); i++) {
                o.pop_infos[i]->kill();
            }
            o.pop_infos.erase(o.pop_infos.begin() + o.province->pops.size(), o.pop_infos.end());
        }
    });

    // Add the initial POPs infoboxes, we can later add/remove as needed on each tick update
    int i = 0;
    for(const auto& pop : this->province->pops) {
        PopInfo* info = new PopInfo(this->gs, 0, (i * 24) + 128, this->province, i, this);
        this->pop_infos.push_back(info);
        i++;
    }
}

ProvinceEconomyTab::ProvinceEconomyTab(GameState& _gs, int x, int y, Province* _province, UI::Widget* _parent)
    : gs{ _gs },
    province{ _province },
    UI::Group(x, y, _parent->width - x, _parent->height - y, _parent)
{
    this->text(province->name);

    this->products_pie = new UI::PieChart(0, 24, 128, 128, this);

    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceEconomyTab&>(w);

        // Obtain demand, supply and other information about the goods
        std::vector<UI::ChartData> goods_data, products_data;
        for(const auto& product : o.gs.world->products) {
            if(product->origin != o.province) {
                continue;
            }

            const auto product_col = UI::Color(
                o.gs.world->get_id(product) * 12,
                o.gs.world->get_id(product) * 31,
                o.gs.world->get_id(product) * 97
            );

            products_data.push_back(UI::ChartData(
                product->demand,
                product->owner->name + "'s " + product->good->name,
                product_col
            ));
        }
    });

    // Initial product info
    int i = 0;
    for(const auto& product : this->gs.world->products) {
        if(product->origin != this->province) {
            continue;
        }

        ProductInfo* info = new ProductInfo(this->gs, 0, (i * 24) + 128, product, this);
        this->product_infos.push_back(info);
        i++;
    }
}

ProvinceView::ProvinceView(GameState& _gs, Province* _province)
    : gs{ _gs },
    province{ _province },
    UI::Window(0, 0, 720, 300)
{
    this->is_scroll = false;
    this->text(province->name);

    this->pop_tab = new ProvincePopulationTab(gs, 128, 24, _province, this);
    this->pop_tab->is_render = true;

    this->econ_tab = new ProvinceEconomyTab(gs, 128, 24, _province, this);
    this->econ_tab->is_render = false;

    this->pop_btn = new UI::Button(0, 24, 128, 24, this);
    this->pop_btn->text("Population");
    this->pop_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);

        o.pop_tab->is_render = true;
        o.econ_tab->is_render = false;
    });

    this->econ_btn = new UI::Button(0, 0, 128, 24, this);
    this->econ_btn->below_of(*this->pop_btn);
    this->econ_btn->text("Economy");
    this->econ_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);

        o.pop_tab->is_render = false;
        o.econ_tab->is_render = true;
    });

    this->nation_btn = new UI::Button(0, 0, 128, 24, this);
    this->nation_btn->below_of(*this->econ_btn);
    this->nation_btn->text("Nation");
    this->nation_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);

        // View the nation info only if the province has a valid owner
        if(o.province->owner == nullptr) return;
        auto* nview = new NationView(o.gs, o.province->owner);
    });

    this->close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    this->close_btn->below_of(*this->nation_btn);
    this->close_btn->text("Close");
}