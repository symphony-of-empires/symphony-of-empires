#include "economy.hpp"
#include "world.hpp"

/// Checks if the industry can do output (if it has enough input)
bool Industry::can_do_output() {
	for(const auto& stock: this->stockpile) {
		if(!stock)
			return false;
	}
	return true;
}

/// Adds a good by id to a industries stockpile
void Industry::add_to_stock(World * world, const size_t good_id, const size_t add) {
	for(size_t i = 0; i < stockpile.size(); i++) {
		if(world->industry_types[this->type_id]->inputs[i] == good_id) {
			this->stockpile[i] += add;
			return;
		}
	}
	return;
}