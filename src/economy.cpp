#include <algorithm>

#include "economy.hpp"
#include "world.hpp"
#include "print.hpp"
#include "pathfinding.hpp"

size_t Industry::get_id(const World& world, size_t province_id) {
	const std::vector<Industry *> * industries = &world.provinces[province_id]->industries;
	const auto industry = std::find(industries->begin(), industries->end(), this);
	if(industry != industries->end()) {
		return std::distance(industries->begin(), industry);
	}
	return (size_t)-1;
}

/**
 * Checks if the industry can produce output (if it has enough input)
 */
bool Industry::can_do_output(const World& world) {
	// Always can produce if RGO
	if(!world.industry_types[this->type_id]->inputs.size()) {
		return true;
	}

	// Check that we have enough stockpile
	for(const auto& stock: this->stockpile) {
		if(!stock)
			return false;
	}
	return true;
}

/**
 * Adds a good by id to a industries stockpile
 */
void Industry::add_to_stock(const World& world, const size_t good_id, const size_t add) {
	for(size_t i = 0; i < this->stockpile.size(); i++) {
		if(world.industry_types[this->type_id]->inputs[i] == good_id) {
			this->stockpile[i] += add;
			break;
		}
	}
}

/**
 * Phase 1 of economy: Delivers & Orders are sent from all factories in the world
 */
void Economy::do_phase_1(World& world) {
	const size_t n_provinces = world.provinces.size();

	// All factories will place their orders for their inputs
	// All RGOs will do deliver requests
	for(ProvinceId i = 0; i < n_provinces; i++) {
		// Reset remaining supplies
		world.provinces[i]->supply_rem = world.provinces[i]->supply_limit;
		
		// This new tick, we will start the chain starting with RGOs producing deliver
		// orders and the factories that require inputs will put their orders in the
		// table so they are served by transport companies.
		// This is the start of the factory chain.
		for(size_t j = 0; j < world.provinces[i]->industries.size(); j++) {
			Industry * industry = world.provinces[i]->industries[j];
			if(!world.provinces[i]->worker_pool) {
				industry->ticks_unoperational++;
				continue;
			}

			industry->ticks_unoperational = 0;

			size_t available_manpower;
			available_manpower = 1500 % world.provinces[i]->worker_pool;
			world.provinces[i]->worker_pool -= available_manpower;
			if(!available_manpower)
				continue;
			
			IndustryType * it = world.industry_types[industry->type_id];
			for(const auto& input: it->inputs) {
				OrderGoods order;
				order.quantity = available_manpower / 7;
				order.payment = 500.f;
				order.good_id = input;
				order.requester_industry_id = j;
				order.requester_province_id = i;
				world.orders.push_back(order);
			}

			if(industry->can_do_output(world) == false)
				continue;

			// Now produce anything as we can!
			// Take a constant of workers needed for factories, 1.5k workers!
			available_manpower = 1500 % world.provinces[i]->worker_pool;
			world.provinces[i]->worker_pool -= available_manpower;
			if(!available_manpower)
				continue;
				
			// Place deliver orders (we are a RGO)
			for(size_t k = 0; k < it->outputs.size(); k++) {
				DeliverGoods deliver;
				deliver.quantity = available_manpower / 8;
				deliver.payment = 1500.f;
				deliver.good_id = it->outputs[k];
				deliver.sender_industry_id = j;
				deliver.sender_province_id = i;
				deliver.product_id = industry->output_products[k];

				// Do not overproduce
				Product * product = world.products[deliver.product_id];
				if(product->supply > product->demand) {
					continue;
				}

				// Cannot be below production cost, so we can be profitable
				if(product->price < industry->production_cost * 1.2f) {
					product->price = industry->production_cost * 1.2f;
				}
				
				if(!deliver.quantity)
					continue;
				
				world.delivers.push_back(deliver);
			}
		}
	}
}

void Economy::do_phase_2(World& world) {
	// Now we will deliver stuff accordingly
	while(world.delivers.size() && world.orders.size()) {
		// Now transport companies will check and transport accordingly
		for(auto& company: world.companies) {
			if(!company->is_transport)
				continue;
			
			// Check all delivers
			for(size_t i = 0; i < world.delivers.size(); i++) {
				DeliverGoods * deliver = &world.delivers[i];
				deliver->rejections++;
				
				if(!company->in_range(deliver->sender_province_id))
					continue;
				
				// Check all orders
				for(size_t j = 0; j < world.orders.size(); j++) {
					OrderGoods * order = &world.orders[j];
					
					// Do they want this goods?
					if(order->good_id != deliver->good_id)
						continue;
					
					// Are we in the range to deliver?
					if(!company->in_range(order->requester_province_id))
						continue;
					
					// Province receives a small supply buff from commerce
					world.provinces[order->requester_province_id]->supply_rem += 5.f;
					
					// Obtain number of goods that we could satisfy
					size_t count = order->quantity % deliver->quantity;
					deliver->quantity -= count;
					order->quantity -= count;

					// Yes - we go and deliver their stuff
					world.provinces[order->requester_province_id]->industries[order->requester_industry_id]->add_to_stock(world, order->good_id, count);
					
					// Increment demand
					world.products[deliver->product_id]->demand += count;
					// Decrement supply
					if(world.products[deliver->product_id]->supply) {
						size_t satisfied = count % world.products[deliver->product_id]->supply;
						world.products[deliver->product_id]->supply -= satisfied;
						world.products[deliver->product_id]->demand += satisfied;
					}

					world.provinces[order->requester_province_id]->industries[order->requester_industry_id]->production_cost += world.products[deliver->product_id]->price;
					
					// On 0 it overflows to -1, on 1 it goes to zero, but after this loop it's
					// incremented again (see incrementing below). So we don't take a rejection hit
					deliver->rejections--;
					
					// Delete this deliver and order tickets from the system since
					// they are now fullfilled (only delete when no quanity left)
					if(!order->quantity) {
						world.orders.erase(world.orders.begin() + j);
					} if(!deliver->quantity) {
						world.delivers.erase(world.delivers.begin() + i);
						i--;
					}
					break;
				}
			}
		}
		
		// Get number of transporter companies
		size_t n_transporters = 0;
		for(const auto& company: world.companies) {
			if(company->is_transport)
				n_transporters++;
		}
		
		// Drop all rejected delivers
		for(size_t i = 0; i < world.delivers.size(); i++) {
			DeliverGoods * deliver = &world.delivers[i];
			Product * product = world.products[deliver->product_id];
				
			// Add up to province stockpile
			world.provinces[deliver->sender_province_id]->stockpile[deliver->product_id] += deliver->quantity;
				
			// Increment supply because of incremented stockpile
			product->supply += deliver->quantity;
			if(product->demand) {
				product->demand -= deliver->quantity % product->demand;
			}
		}
		world.delivers.clear();
		break;
	}
	world.delivers.clear();
	world.orders.clear();
}

void Economy::do_phase_3(World& world) {
	// Now, it's like 1 am here, but i will try to write a very nice economic system
	// TODO: There is a lot to fix here, first the economy system commits inverse great depression and goes way too happy
	for(Province *& province: world.provinces) {
		// Reset worker pool
		province->worker_pool = 0;
		for(size_t i = 0; i < province->pops.size(); i++) {
			Pop * pop = province->pops[i];
			// If pop is of size 0, then delete it, it's dead :(
			if(!pop->size) {
				province->pops.erase(province->pops.begin() + i);
				i--;
				continue;
			}

			//printf("%s: %s %zu (%4.f$)\n", province->name.c_str(), this->pop_types[pop->type_id]->name.c_str(), pop->size, pop->budget);
			
			float salary;
			
			// TODO: This is very stupid
			switch(pop->type_id) {
			case POP_TYPE_ENTRPRENEUR:
				salary = 5.f;
				break;
			case POP_TYPE_ARTISAN:
				salary = 0.4f;
				break;
			case POP_TYPE_CRAFTSMEN:
				salary = 0.2f;
				break;
			case POP_TYPE_BUREAUCRAT:
				salary = 0.1f;
				break;
			case POP_TYPE_ARISTOCRAT:
				salary = 0.6f;
				break;
			case POP_TYPE_CLERGYMEN:
				salary = 0.2f;
				break;
			case POP_TYPE_FARMER:
				salary = 0.7f;
				break;
			case POP_TYPE_SOLDIER:
				salary = 0.3f;
				break;
			case POP_TYPE_OFFICER:
				salary = 0.7f;
				break;
			case POP_TYPE_LABORER:
				salary = 0.2f;
				break;
			case POP_TYPE_SLAVE:
				salary = 0.01f;
				break;
			default:
				salary = 0.f;
				break;
			}
			
			// TODO: Make this dynamic
			pop->budget += salary;
			
			size_t alloc_budget;
			
			// Use 25% of our budget to buy edibles and life needed stuff
			alloc_budget = pop->budget / 4;
			for(ProductId j = 0; j < province->stockpile.size(); j++) {
				// Province must have stockpile
				if(!province->stockpile[j])
					continue;
				
				// Must be an edible
				if(world.goods[world.products[j]->good_id]->is_edible != true)
					continue;
				
				// We can only spend our allocated budget
				// TODO: Base spending on literacy, more literacy == less spending
				size_t bought = alloc_budget / world.products[j]->price;
				bought %= province->stockpile[j];
				if(!bought)
					continue;
				
				// Satisfaction of needs is proportional to bought items
				pop->life_needs_met += (float)pop->size / (float)bought;
				
				// Demand is incremented proportional to items bought
				world.products[j]->demand += bought;
				
				// Reduce supply
				if(world.products[j]->supply) {
					world.products[j]->supply -= bought % world.products[j]->supply;
				}
				
				// Deduct from budget, and remove item from stockpile
				pop->budget -= bought * world.products[j]->price;
				alloc_budget -= bought * world.products[j]->price;
				province->stockpile[j] -= bought;
			}
			
			// Use 10% of our budget for buying uneeded commodities and shit
			// TODO: Should lower spending with higher literacy, and higher
			// TODO: Higher the fullfilment per unit with higher literacy
			alloc_budget = pop->budget / 10;
			for(size_t j = 0; j < province->stockpile.size(); j++) {
				// Province must have stockpile
				if(!province->stockpile[j])
					continue;
				
				// Must be a non-edible; aka. not essential
				if(world.goods[world.products[j]->good_id]->is_edible != false)
					continue;
				
				// We can only spend our allocated budget
				// TODO: Base spending on literacy, more literacy == less spending
				size_t bought = alloc_budget / world.products[j]->price;
				bought %= province->stockpile[j];
				if(!bought)
					continue;
				
				// Satisfaction of needs is proportional to bought items
				pop->everyday_needs_met += (float)pop->size / (float)bought;
				
				// Demand is incremented proportional to items bought
				world.products[j]->demand += bought;
				
				// Reduce supply
				if(world.products[j]->supply) {
					world.products[j]->supply -= bought % world.products[j]->supply;
				}
				
				// Deduct from budget, and remove item from stockpile
				pop->budget -= bought * world.products[j]->price;
				alloc_budget -= bought * world.products[j]->price;
				province->stockpile[j] -= bought;
			}
			
			// x1.5 life needs met modifier, that is the max allowed
			pop->life_needs_met = std::min<float>(1.5f, pop->life_needs_met);
			pop->life_needs_met = std::max<float>(-1.5f, pop->life_needs_met);

			// POPs cannot shrink below 10<
			if(pop->size <= 10) {
				// Population cannot be 0
				pop->size = std::max<size_t>(1, pop->size);
				pop->size += rand() % std::min<size_t>(500, std::max<size_t>(1, (pop->size / 10000)));
			} else {
				// Higher literacy will mean there will be less births due to sex education
				// and will also mean that - there would be less deaths due to higher knewledge
				ssize_t growth = pop->life_needs_met / (pop->literacy * 10.f);
				if(growth < 0 && (size_t)abs(growth) >= pop->size) {
					pop->size = 1;
					continue;
				} else {
					pop->size += growth;
				}
			}
			
			// Met life needs means less militancy
			if(pop->life_needs_met >= 1.f) {
				pop->life_needs_met = 1.f;
				if(pop->militancy > 0.f) {
					pop->militancy -= 0.0002f;
					pop->consciousness -= 0.0001f;
				}
			} else {
				pop->militancy += 0.001f;
				pop->consciousness += 0.001f;
			}
			
			pop->life_needs_met -= 0.5f;
			
			province->worker_pool += pop->size;
		}
		
		// Stockpiles cleaned
		for(ProductId i = 0; i < province->stockpile.size(); i++) {
			province->stockpile[i] = 0;
		}
	}
}

void Economy::do_phase_4(World& world) {
	// Preparations for the next tick

	// Reset production costs
	const ProvinceId n_provinces = world.provinces.size();
	for(ProvinceId i = 0; i < n_provinces; i++) {
		const IndustryId n_industries = world.provinces[i]->industries.size();
		for(IndustryId j = 0; j < n_industries; j++) {
			Industry * industry = world.provinces[i]->industries[j];
			industry->production_cost = 0.f;
		}
	}

	// Close today's price with a change according to demand - supply
	const ProductId n_products = world.products.size();
	for(ProductId i = 0; i < n_products; i++) {
		Product * product = world.products[i];
		if(product->demand > product->supply) {
			product->price_vel += 0.0001f * (product->demand - product->supply);
		} else if(product->demand < product->supply) {
			product->price_vel -= 0.0001f * (product->supply - product->demand);
		} else {
			if(product->price_vel > 0.1f) {
				product->price_vel -= 0.001f;
			} else if(product->price_vel < -0.1f) {
				product->price_vel += 0.001f;
			} else {
				product->price_vel = -0.001f;
			}
		}
		product->price += product->price_vel;
		product->price = std::max<float>(0.01f, product->price);
			
		// Save prices and stuff onto history (for the charts!)
		product->demand_history.push_back(product->demand);
		if(product->demand_history.size() > 60)
			product->demand_history.pop_front();
		
		product->supply_history.push_back(product->supply);
		if(product->supply_history.size() > 60)
			product->supply_history.pop_front();

		product->price_history.push_back(product->price);
		if(product->price_history.size() > 60)
			product->price_history.pop_front();
			
		// Re-count worldwide supply
		product->supply = 0;
		for(const auto& province: world.provinces) {
			product->supply += province->stockpile[i];
		}
		product->demand = 0;
	}
}