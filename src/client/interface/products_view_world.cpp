#include "../ui.hpp"

#include "nation.hpp"
extern Nation* curr_nation;

extern std::pair<int, int> mouse_pos;

#include "world.hpp"
extern World* g_world;

static UI::Window* products_view_win = nullptr;
static uint8_t products_view_page_num = 0;
static void products_view_world(void) {
    // Do not make duplicate windows
    if(products_view_win != nullptr) {
        return;
    }
    
    // View the provinces in a country - along with the population in them
    products_view_win = new UI::Window(mouse_pos.first, mouse_pos.second, 609, 476);
    products_view_win->on_update = ([](UI::Widget& w, void*) {
        std::lock_guard<std::recursive_mutex> l1(g_world->time_mutex);
        if(g_world->time % 48 == 35) {
            std::lock_guard<std::recursive_mutex> l2(g_world->products_mutex);
            size_t list_idx = products_view_page_num * 16;
            for(size_t i = 3; i < w.children.size(); i++) {
                if(list_idx < g_world->products.size()) {
                    // Skip products that are not from our country
                    while(g_world->products[list_idx]->origin->owner != curr_nation) {
                        list_idx++;
                        if(list_idx >= g_world->products.size())
                            break;
                    }

                    const Product* product = g_world->products[list_idx];
                    sprintf(tmpbuf, "%zu %zu %.2f$ %.2f%% %s %s", product->supply, product->demand, product->price, product->price_vel * 100.f, product->origin->name.c_str(), product->good->name.c_str());
                    w.children[i]->text(tmpbuf);
                    w.children[i]->user_data = &product;
                } else {
                    sprintf(tmpbuf, " ");
                    w.children[i]->text(tmpbuf);
                    w.children[i]->user_data = nullptr;
                }
                list_idx++;
            }
        }
    });
    
    sprintf(tmpbuf, "Your province's products (page %zu)", (size_t)products_view_page_num);
    products_view_win->text(tmpbuf);
    
    UI::Button* ok_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, products_view_win);
    ok_btn->text("OK");
    ok_btn->on_click = [](UI::Widget& w, void *) {
        products_view_win = nullptr;
        delete w.parent;
    };
    
    UI::Button* prev_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, products_view_win);
    prev_btn->text("Previous");
    prev_btn->right_side_of(*ok_btn);
    prev_btn->on_click = [](UI::Widget&, void *) {
        products_view_page_num--;
        sprintf(tmpbuf, "Your province's products (page %zu)", (size_t)products_view_page_num);
        products_view_win->text(tmpbuf);
    };
    
    UI::Button* next_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, products_view_win);
    next_btn->text("Next");
    next_btn->right_side_of(*prev_btn);
    next_btn->on_click = [](UI::Widget&, void *) {
        products_view_page_num++;
        sprintf(tmpbuf, "Your province's products (page %zu)", (size_t)products_view_page_num);
        products_view_win->text(tmpbuf);
    };
    
    for(size_t i = 0; i < 16; i++) {
        UI::Button* elem = new UI::Button(0, 32 + (i * 24), products_view_win->width, 24, products_view_win);
        elem->on_click = ([](UI::Widget& w, void* data) {
            Product* product = (Product*)data;
            if(data == nullptr)
                return;

            UI::Window* info_win = new UI::Window(0, 0, 320, 200);
            info_win->text("Product info");

            UI::Chart* supply_graph = new UI::Chart(0, 32, 120, 64, info_win);
            supply_graph->user_data = product;
            supply_graph->text("Supply");
            supply_graph->on_update = ([](UI::Widget& w, void* data) {
                std::lock_guard<std::recursive_mutex> l1(g_world->time_mutex);
                Product* product = (Product*)data;
                
                UI::Chart& wc = dynamic_cast<UI::Chart&>(w);
                if(g_world->time % 48 == 32) {
                    std::lock_guard<std::recursive_mutex> l2(g_world->products_mutex);

                    wc.data.push_back(product->supply);
                    if(wc.data.size() >= 30)
                        wc.data.pop_front();
                }
            });

            UI::Chart* demand_graph = new UI::Chart(0, 32, 120, 64, info_win);
            demand_graph->below_of(*supply_graph);
            demand_graph->user_data = product;
            demand_graph->text("Demand");
            demand_graph->on_update = ([](UI::Widget& w, void* data) {
                std::lock_guard<std::recursive_mutex> l1(g_world->time_mutex);
                Product* product = (Product*)data;

                UI::Chart& wc = dynamic_cast<UI::Chart&>(w);
                if(g_world->time % 48 == 32) {
                    std::lock_guard<std::recursive_mutex> l2(g_world->products_mutex);

                    wc.data.push_back(product->demand);
                    if(wc.data.size() >= 30)
                        wc.data.pop_front();
                }
            });

            UI::Chart* price_graph = new UI::Chart(0, 32, 120, 64, info_win);
            price_graph->below_of(*demand_graph);
            price_graph->user_data = data;
            price_graph->text("Price");
            price_graph->on_update = ([](UI::Widget& w, void* data) {
                std::lock_guard<std::recursive_mutex> l1(g_world->time_mutex);
                Product* product = (Product*)data;

                UI::Chart& wc = dynamic_cast<UI::Chart&>(w);
                if(g_world->time % 48 == 32) {
                    std::lock_guard<std::recursive_mutex> l2(g_world->products_mutex);

                    wc.data.push_back(product->price);
                    if(wc.data.size() >= 30)
                        wc.data.pop_front();
                }
            });

            UI::Label* good_name_text = new UI::Label(128, 32, " ", info_win);
            good_name_text->text(product->good->name.c_str());

            UI::Label* province_name_text = new UI::Label(128, 32, " ", info_win);
            province_name_text->below_of(*good_name_text);
            province_name_text->text(product->origin->name.c_str());

            UI::Label* company_name_text = new UI::Label(128, 32, " ", info_win);
            company_name_text->below_of(*province_name_text);
            company_name_text->text(product->industry->owner->name.c_str());
        });
    }
}