#include "policies.hpp"

using namespace Interface;

PoliciesScreen::PoliciesScreen(GameState& _gs)
    : gs{_gs},
    UI::Window(0, 0, 512, 512, nullptr)
{
    this->ideology_pie = new UI::PieChart(0, 24, 128, 128, this);
    this->ideology_pie->text("Ideologies");

    this->militancy_chart = new UI::Chart(0, 0, 128, 128, this);
    this->militancy_chart->text("Militancy");
    this->militancy_chart->right_side_of(*ideology_pie);

    this->con_chart = new UI::Chart(0, 0, 128, 128, this);
    this->con_chart->text("Conciousness");
    this->con_chart->right_side_of(*militancy_chart);

    this->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = dynamic_cast<PoliciesScreen&>(w);
        const World& world = *(o.gs.world);

        std::vector<UI::ChartData> ideology_data;
        for(const auto& ideology : world.ideologies) {
            ideology_data.push_back(UI::ChartData(
                1.f,
                ideology->name,
                UI::Color(rand() % 255, rand() % 255, rand() % 255)
            ));
        }
        o.ideology_pie->set_data(ideology_data);

        float militancy = 0.f, con = 0.f;
        for(const auto& province : o.gs.curr_nation->owned_provinces) {
            for(const auto& pop : province->pops) {
                ideology_data[world.get_id(pop.ideology)].num += pop.size;
                militancy += pop.militancy;
                con += pop.consciousness;
            }
        }

        o.militancy_chart->data.push_back(militancy);
        if(o.militancy_chart->data.size() >= 30)
            o.militancy_chart->data.pop_back();
        
        o.con_chart->data.push_back(con);
        if(o.con_chart->data.size() >= 30)
            o.con_chart->data.pop_back();
    });
}
