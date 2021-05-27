#include "ui.hpp"
#include "world.hpp"
extern World * g_world;
extern int width;
extern int height;

static UI::Window * econ_win;
static std::vector<UI::Widget *> wm_lab;
static UI::Window * wm_win = nullptr;

static UI::Chart * supply_chart;
static UI::Chart * demand_chart;
static UI::Chart * price_chart;
static void do_view_product_info_or(UI::Widget * w, void *) {
	Product * product = (Product *)w->user_data;

	supply_chart->data = std::deque<float>(product->supply_history.begin(), product->supply_history.end());
	demand_chart->data = std::deque<float>(product->demand_history.begin(), product->demand_history.end());
	price_chart->data = std::deque<float>(product->price_history.begin(), product->price_history.end());
}
static void do_view_product_info(UI::Widget * w, void *) {
	Product * product = (Product *)w->user_data;
	UI::Window * prod_win = new UI::Window(nullptr, 96, 196, 512, 512);
	prod_win->user_data = w->user_data;
	prod_win->on_update = &do_view_product_info_or;

	supply_chart = new UI::Chart(prod_win, 24, 0, 482, 64, "Supply");
	demand_chart = new UI::Chart(prod_win, 24, 128, 482, 64, "Demand");
	price_chart = new UI::Chart(prod_win, 24, 256, 482, 64, "Price");
}

static void do_world_market_overview_or(UI::Widget *, void *) {
	const size_t n_labs = 4;
	char * str = new char[255];
	for(size_t i = 0; i < g_world->products.size(); i++) {
		UI::Label * lab;
		wm_lab[i * 5 + 0]->text(g_world->goods[g_world->products[i]->good_id]->name.c_str());
		sprintf(str, "%4.2f", g_world->products[i]->price);
		wm_lab[i * 5 + 1]->text(str);
		sprintf(str, "%4.2f", g_world->products[i]->price_vel);
		wm_lab[i * 5 + 2]->text(str);
		wm_lab[i * 5 + 3]->text(g_world->provinces[g_world->products[i]->origin_id]->name.c_str());
	}
	delete[] str;
}

static void do_world_market_overview(UI::Widget *, void *) {
	wm_lab.clear();

	wm_win = new UI::Window(nullptr, width - 1024, econ_win->y, 512 + 256 + 128, height - 256, "World market");
	wm_win->on_update = &do_world_market_overview_or;
	for(size_t i = 0; i < g_world->products.size(); i++) {
		UI::Label * lab;
		UI::Button * info;
		lab = new UI::Label(wm_win, 0, i * 24, "?");
		wm_lab.push_back(lab);
		lab = new UI::Label(wm_win, 128, i * 24, "?");
		wm_lab.push_back(lab);
		lab = new UI::Label(wm_win, 256, i * 24, "?");
		wm_lab.push_back(lab);
		lab = new UI::Label(wm_win, 400, i * 24, "?");
		wm_lab.push_back(lab);
		info = new UI::Button(wm_win, 700, i * 24, 32, 24, "?");
		info->user_data = g_world->products[i];
		info->on_click = &do_view_product_info;
		wm_lab.push_back(info);
	}
}

void do_economy_overview(UI::Widget *, void *) {
	econ_win = new UI::Window(nullptr, width - 256, 196, 256, height - 256, "Economy");
	
	UI::Button * econ_wm_btn = new UI::Button(econ_win, 0, 0, 256, 24, "World market");
	econ_wm_btn->on_click = &do_world_market_overview;
}