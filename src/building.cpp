#include "building.hpp"
#include "product.hpp"
#include "province.hpp"
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
    return this->province;
}

Nation* Building::get_owner(void) {
    return this->owner;
}

void Building::create_factory(void) {
    World& world = World::get_instance();

    // Add a product for each output
    for(const auto& output : type->outputs) {
        Product* new_product = new Product();
        new_product->building = this;
        new_product->good = output;

        province->products.push_back(new_product);
        output_products.push_back(new_product);
        world.insert(new_product);

        // Add an element representing this product on all the province's stockpile
        employees_needed_per_output.push_back(500);
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

std::pair<float, float> Building::get_pos(void) const {
    return std::make_pair(
        province->min_x + ((province->max_x - province->min_x) / 2.f),
        province->min_y + ((province->max_y - province->min_y) / 2.f)
    );
};