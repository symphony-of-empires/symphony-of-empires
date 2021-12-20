#include "province_view.hpp"
#include "nation_view.hpp"

#include "../game_state.hpp"
#include "../../nation.hpp"
#include "../../world.hpp"

using namespace Interface;

void ProvincePopulationTab::update_piecharts() {
    // Obtain population information
    std::vector<size_t> culture_sizes(gs.world->cultures.size(), 0);
    std::vector<size_t> religion_sizes(gs.world->religions.size(), 0);
    std::vector<size_t> pop_type_sizes(gs.world->pop_types.size(), 0);
    for(const auto& pop : province->pops) {
        culture_sizes[gs.world->get_id(pop.culture)] += pop.size;
        religion_sizes[gs.world->get_id(pop.religion)] += pop.size;
        pop_type_sizes[gs.world->get_id(pop.type)] += pop.size;
    }

    std::vector<UI::ChartData> cultures_data, religions_data, pop_types_data;
    for(const auto& culture : gs.world->cultures) {
        const auto color = UI::Color(
            gs.world->get_id(culture) * 12,
            gs.world->get_id(culture) * 31,
            gs.world->get_id(culture) * 97
        );
        cultures_data.push_back(UI::ChartData(culture_sizes[gs.world->get_id(culture)], culture->name, color));
    }
    cultures_pie->set_data(cultures_data);

    for(const auto& religion : gs.world->religions) {
        const auto color = UI::Color(
            gs.world->get_id(religion) * 12,
            gs.world->get_id(religion) * 31,
            gs.world->get_id(religion) * 97
        );
        religions_data.push_back(UI::ChartData(religion_sizes[gs.world->get_id(religion)], religion->name, color));
    }
    religions_pie->set_data(religions_data);

    for(const auto& pop_type : gs.world->pop_types) {
        const auto color = UI::Color(
            gs.world->get_id(pop_type) * 12,
            gs.world->get_id(pop_type) * 31,
            gs.world->get_id(pop_type) * 97
        );
        pop_types_data.push_back(UI::ChartData(pop_type_sizes[gs.world->get_id(pop_type)], pop_type->name, color));
    }
    pop_types_pie->set_data(pop_types_data);

    if(pop_infos.size() < province->pops.size()) {
        for(size_t i = pop_infos.size(); i < province->pops.size(); i++) {
            PopInfo* info = new PopInfo(gs, 0, (i * 24) + 128, province, i, this);
            pop_infos.push_back(info);
        }
    } else if(pop_infos.size() > province->pops.size()) {
        for(size_t i = province->pops.size(); i < pop_infos.size(); i++) {
            pop_infos[i]->kill();
        }
        pop_infos.erase(pop_infos.begin() + province->pops.size(), pop_infos.end());
    }
}

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

    update_piecharts();
    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvincePopulationTab&>(w);
        if(o.gs.world->time % o.gs.world->ticks_per_month) return;
        o.update_piecharts();
    });
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
        for(const auto& product : o.province->products) {
            if(product->building == nullptr) continue;

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
    unsigned int i = 0;
    for(const auto& product : this->province->products) {
        if(product->building == nullptr) continue;

        ProductInfo* info = new ProductInfo(this->gs, 0, (i * 24) + 128, product, this);
        this->product_infos.push_back(info);
        i++;
    }
}

#include "building.hpp"
ProvinceView::ProvinceView(GameState& _gs, Province* _province)
    : gs{ _gs },
    province{ _province },
    UI::Window(0, 0, 720, 600)
{
    this->is_scroll = false;
    this->text(province->name);

    this->pop_tab = new ProvincePopulationTab(gs, 128, 24, province, this);
    this->pop_tab->is_render = true;
    auto* pop_btn = new UI::Button(0, 24, 128, 24, this);
    pop_btn->text("Population");
    pop_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);

        o.pop_tab->is_render = true;
        o.econ_tab->is_render = false;
    });

    this->econ_tab = new ProvinceEconomyTab(gs, 128, 24, province, this);
    this->econ_tab->is_render = false;
    auto* econ_btn = new UI::Button(0, 0, 128, 24, this);
    econ_btn->below_of(*pop_btn);
    econ_btn->text("Economy");
    econ_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);

        o.pop_tab->is_render = false;
        o.econ_tab->is_render = true;
    });

    auto* nation_btn = new UI::Button(0, 0, 128, 24, this);
    nation_btn->below_of(*econ_btn);
    nation_btn->text("Nation");
    nation_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);

        // View the nation info only if the province has a valid owner
        if(o.province->owner == nullptr) return;
        new NationView(o.gs, o.province->owner);
    });

    // TODO: Make this a tab
    auto* build_btn = new UI::Button(0, 0, 128, 24, this);
    build_btn->below_of(*nation_btn);
    build_btn->text("Buildings");
    build_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<ProvinceView&>(*w.parent);
        new BuildingBuildView(o.gs, 0, 0, false, o.gs.curr_nation, o.province, nullptr);
    });

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*build_btn);
    close_btn->text("Close");
}