#include "economy.hpp"
#include "world.hpp"

bool Industry::can_do_output() {
	for(const auto& stock: this->stockpile) {
		if(!stock) return false;
	}
	return true;
}

void Industry::add_to_stock(World * world, const size_t good_id, const size_t add) {
	for(size_t i = 0; i < stockpile.size(); i++) {
		if(world->industry_types[this->type_id].inputs[i] == good_id) {
			this->stockpile[i] += add;
			return;
		}
	}
	return;
}