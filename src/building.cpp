#include "building.hpp"
#include "product.hpp"
#include "world.hpp"

// Adds a good by id to a building stockpile
void Building::add_to_stock(const Good* good, const size_t add) {
    const World& world = World::get_instance();
    for(size_t i = 0; i < stockpile.size(); i++) {
        if(world.get_id(type->inputs[i]) != world.get_id(good)) {
            continue;
        }

        stockpile[i] += add;
        break;
    }
}

Province* Building::get_province(void) {
    //const Tile& tile = world.get_tile(this->x, this->y);
    //if(tile.province_id == (Province::Id)-1) {
    //    return nullptr;
    //}
    //return world.provinces[tile.province_id];
    return this->province;
}

Nation* Building::get_owner(void) {
    //const Tile& tile = world.get_tile(this->x, this->y);
    //if(tile.owner_id == (Nation::Id)-1) {
    //    return nullptr;
    //}
    //return world.nations[tile.owner_id];
    return this->owner;
}

void Building::create_factory(void) {
    World& world = World::get_instance();
    corporate_owner->operating_provinces.insert(get_province());

    // Add a product for each output
    for(const auto& output : type->outputs) {
        Product* new_product = new Product();
        new_product->building = this;
        new_product->good = output;
        new_product->owner = corporate_owner;
        province->products.push_back(new_product);

        output_products.push_back(new_product);
        world.insert(new_product);

        employees_needed_per_output.push_back(500);

        // Add an element representing this product on all the province's stockpile
        for(auto& province : world.provinces) {
            province->stockpile.push_back(0);
        }
    }

    // We will set inputs_satisfied to same size as inputs
    // TODO: Make the starting number of stockpiles be dynamic
    stockpile.insert(stockpile.begin(), type->inputs.size(), 100);
}

// Helper method to delete a factory
void Building::delete_factory(void) {
    World& world = World::get_instance();
    // Products become building-less
    for(const auto& product : output_products) {
		product->building = nullptr;
    }
}

// Checks if the building can produce output (if it has enough input)
bool Building::can_do_output(void) {
    const World& world = World::get_instance();

    // No output products?
    if(type->outputs.empty() || output_products.empty())
        return false;

    // Always can produce if RGO
    if(type->inputs.empty())
        return true;

    // Check that we have enough stockpile
    for(const auto& stock : this->stockpile) {
        if(!stock) return false;
    }
    return true;
}

Building::~Building() {
    // Delete factory (products related to the factory must be destroyed too)
    if(this->type != nullptr && this->type->is_factory == true) {
        this->delete_factory();
    }
}
