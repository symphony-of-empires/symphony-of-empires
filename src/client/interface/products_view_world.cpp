#include "products_view_world.hpp"

#include "../ui.hpp"
#include "../../nation.hpp"

extern char* tmpbuf;
ProductsViewWorld::ProductsViewWorld(GameState& _gs) : gs{_gs}, products_view_win{nullptr} {
}

void ProductsViewWorld::show() {
    if (products_view_win != nullptr) {
        return;
    }

    // View the provinces in a country - along with the population in them
    products_view_win = new UI::Window(gs.input.mouse_pos.first, gs.input.mouse_pos.second, 609, 476);

    for (size_t i = 0; i < 16; i++) {
        buttons.push_back(ProductsViewWorldButton(gs, products_view_win, i));
    }

    products_view_win->user_data = this;
    products_view_win->on_update = ([](UI::Widget& w, void* data) {
        ProductsViewWorld* pvw = (ProductsViewWorld*)data;
        World* world = pvw->gs.world;
        Nation* curr_nation = pvw->gs.curr_nation;
        std::lock_guard<std::recursive_mutex> lock(world->world_mutex);
        if (world->time % 48 == 35) {
            size_t list_idx = pvw->page_nr * 16;
            for (size_t i = 0; i < pvw->buttons_nr; i++) {
                if (list_idx < world->products.size()) {
                    // Skip products that are not from our country
                    while (world->products[list_idx]->origin->owner != curr_nation) {
                        list_idx++;
                        if (list_idx >= world->products.size())
                            break;
                    }

                    const Product* product = world->products[list_idx];
                    sprintf(tmpbuf, "%zu %zu %.2f$ %.2f%% %s %s", product->supply, product->demand, product->price, product->price_vel * 100.f, product->origin->name.c_str(), product->good->name.c_str());
                    pvw->buttons[i].text = std::string(tmpbuf);
                    pvw->buttons[i].product = product;
                } else {
                    sprintf(tmpbuf, " ");
                    pvw->buttons[i].text = std::string(" ");
                    pvw->buttons[i].product = nullptr;
                }
                list_idx++;
            }
        }
    });

    sprintf(tmpbuf, "Your province's products (page %zu)", (size_t)page_nr);
    products_view_win->text(tmpbuf);

    UI::Button* ok_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, products_view_win);
    ok_btn->text("OK");
    ok_btn->user_data = this;
    ok_btn->on_click = [](UI::Widget& w, void* data) {
        ProductsViewWorld* pvw = (ProductsViewWorld*)data;
        pvw->buttons.clear();
        pvw->products_view_win = nullptr;
        delete w.parent;
    };

    UI::Button* prev_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, products_view_win);
    prev_btn->text("Previous");
    prev_btn->right_side_of(*ok_btn);
    prev_btn->user_data = this;
    prev_btn->on_click = [](UI::Widget&, void* data) {
        ProductsViewWorld* pvw = (ProductsViewWorld*)data;
        pvw->page_nr--;
        sprintf(tmpbuf, "Your province's products (page %zu)", (size_t)pvw->page_nr);
        pvw->products_view_win->text(tmpbuf);
    };

    UI::Button* next_btn = new UI::Button(0, 32 + (16 * 24), 196, 24, products_view_win);
    next_btn->text("Next");
    next_btn->right_side_of(*prev_btn);
    next_btn->user_data = this;
    next_btn->on_click = [](UI::Widget&, void* data) {
        ProductsViewWorld* pvw = (ProductsViewWorld*)data;
        pvw->page_nr++;
        sprintf(tmpbuf, "Your province's products (page %zu)", (size_t)pvw->page_nr);
        pvw->products_view_win->text(tmpbuf);
    };
}

ProductsViewWorldButton::ProductsViewWorldButton(GameState& _gs, UI::Window* parent, size_t index) : gs{_gs} {
    UI::Button* elem = new UI::Button(0, 32 + (index * 24), parent->width, 24, parent);
    elem->user_data = this;
    elem->on_click = ([](UI::Widget& w, void* data) {
        ProductsViewWorldButton* state = (ProductsViewWorldButton*)data;
        const Product* product = state->product;
        if (product == nullptr)
            return;

        UI::Window* info_win = new UI::Window(0, 0, 320, 200);
        info_win->text("Product info");

        UI::Chart* supply_graph = new UI::Chart(0, 32, 120, 64, info_win);
        supply_graph->text("Supply");
        supply_graph->user_data = data;
        supply_graph->on_update = ([](UI::Widget& w, void* data) {
            ProductsViewWorldButton* state = (ProductsViewWorldButton*)data;
            const Product* product = state->product;
            World* world = state->gs.world;
            std::lock_guard<std::recursive_mutex> lock(world->world_mutex);

            UI::Chart& wc = dynamic_cast<UI::Chart&>(w);
            if (world->time % 48 == 32) {
                wc.data.push_back(product->supply);
                if (wc.data.size() >= 30)
                    wc.data.pop_front();
            }
        });

        UI::Chart* demand_graph = new UI::Chart(0, 32, 120, 64, info_win);
        demand_graph->below_of(*supply_graph);
        demand_graph->text("Demand");
        demand_graph->user_data = data;
        demand_graph->on_update = ([](UI::Widget& w, void* data) {
            ProductsViewWorldButton* state = (ProductsViewWorldButton*)data;
            const Product* product = state->product;
            World* world = state->gs.world;
            std::lock_guard<std::recursive_mutex> lock(world->world_mutex);

            UI::Chart& wc = dynamic_cast<UI::Chart&>(w);
            if (world->time % 48 == 32) {
                wc.data.push_back(product->demand);
                if (wc.data.size() >= 30)
                    wc.data.pop_front();
            }
        });

        UI::Chart* price_graph = new UI::Chart(0, 32, 120, 64, info_win);
        price_graph->below_of(*demand_graph);
        price_graph->text("Price");
        price_graph->user_data = data;
        price_graph->on_update = ([](UI::Widget& w, void* data) {
            ProductsViewWorldButton* state = (ProductsViewWorldButton*)data;
            const Product* product = state->product;
            World* world = state->gs.world;
            std::lock_guard<std::recursive_mutex> lock(world->world_mutex);

            UI::Chart& wc = dynamic_cast<UI::Chart&>(w);
            if (world->time % 48 == 32) {
                wc.data.push_back(product->price);
                if (wc.data.size() >= 30)
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