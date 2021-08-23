#ifndef PRODUCTS_VIEW_WORLD_H
#define PRODUCTS_VIEW_WORLD_H

#include "../game_state.hpp"
#include "../ui.hpp"
#include "industry.hpp"
#include "nation.hpp"
#include "world.hpp"

class GameState;

class ProductsViewWorldButton {
   public:
    ProductsViewWorldButton(GameState& gs, UI::Window* parent, size_t index);
    GameState& gs;
    const Product* product;
    std::string text;
};

class ProductsViewWorld {
   public:
    ProductsViewWorld(GameState& gs);

    GameState& gs;
    UI::Window* products_view_win;
    std::vector<ProductsViewWorldButton> buttons;
    void show();

   private:
    uint8_t page_nr;
};

#endif