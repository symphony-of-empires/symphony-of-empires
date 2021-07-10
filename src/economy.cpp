#include <algorithm>
#include <execution>

#include "economy.hpp"
#include "world.hpp"
#include "print.hpp"
#include "pathfinding.hpp"

#include "serializer.hpp"
#include "io_impl.hpp"
#include "network.hpp"

/**
* Checks if the industry can produce output (if it has enough input)
 */
bool Industry::can_do_output(const World& world) {
	// Except when no products are produced at all
	if(!type->outputs.size() || !this->output_products.size())
		return false;
	
	// Always can produce if RGO
	if(!type->inputs.size())
		return true;

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
void Industry::add_to_stock(const World& world, const Good* good, const size_t add) {
	for(size_t i = 0; i < stockpile.size(); i++) {
		if(world.get_id(type->inputs[i]) == world.get_id(good)) {
			stockpile[i] += add;
			break;
		}
	}
}

/**
* Phase 1 of economy: Delivers & Orders are sent from all factories in the world
 */
void Economy::do_phase_1(World& world) {
	// Outposts who have fullfilled requirements to build stuff will spawna  lil' unit/boat
	for(auto& outpost: world.outposts) {
		bool can_build = true;
		for(const auto& req: outpost->req_goods_for_unit) {
			if(req.second) {
				can_build = false;
				break;
			}
		}

		if(outpost->working_unit_type != nullptr) {
			// Spawn a unit
			Unit* unit = new Unit();
			unit->x = outpost->x;
			unit->y = outpost->y;
			unit->type = outpost->working_unit_type;
			unit->tx = unit->x;
			unit->ty = unit->y;
			unit->owner = outpost->owner;
			unit->budget = 5000.f;
			unit->experience = 1.f;
			unit->morale = 1.f;
			unit->supply = 1.f;
			unit->defensive_ticks = 0;
			unit->size = unit->type->max_health;
			unit->base = unit->size;
			
			// Notify all clients of the server about this
			g_world->units_mutex.lock();
			g_world->units.push_back(unit);
			Packet packet = Packet();
			Archive ar = Archive();
			enum ActionType action = ACTION_UNIT_ADD;
			::serialize(ar, &action); // ActionInt
			::serialize(ar, &unit); // UnitRef
			packet.data(ar.get_buffer(), ar.size());
			g_server->broadcast(packet);
			g_world->units_mutex.unlock();

			outpost->working_unit_type = nullptr;
		} else if(outpost->working_boat_type != nullptr) {
			// Spawn a boat
			/*Boat boat;
			boat.x = outpost->x;
			boat.y = outpost->y;*/

			outpost->working_boat_type = nullptr;
		}
	}

	// All factories will place their orders for their inputs
	// All RGOs will do deliver requests
	for(auto& province: world.provinces) {
		// Reset remaining supplies
		province->supply_rem = province->supply_limit;

		if(province->owner == nullptr) {
			continue;
		}
		
		// This new tick, we will start the chain starting with RGOs producing deliver
		// orders and the factories that require inputs will put their orders in the
		// table so they are served by transport companies.
		// This is the start of the factory chain.
		for(auto& industry: province->industries) {
			if(!province->worker_pool) {
				industry.days_unoperational++;
				industry.min_quality = 0;

				// TODO: We should tax industry daily income instead of by it's total budget
				// Budget of the industry decreases due to taxes, and the goverments keeps those taxes
				const float loss_by_tax = industry.budget * province->owner->current_policy.industry_tax;
				industry.budget -= loss_by_tax;
				province->owner->budget += loss_by_tax;
				continue;
			}
			
			industry.days_unoperational = 0;
			
			size_t needed_manpower = 1500;
			size_t available_manpower = std::min<size_t>(needed_manpower, province->worker_pool);
			province->worker_pool -= available_manpower;
			if(!available_manpower)
				continue;
			
			industry.workers = available_manpower;
			
			const IndustryType& it = *industry.type;
			for(const auto& input: it.inputs) {
				if(input == nullptr)
					continue;
				
				OrderGoods order;
				order.quantity = (available_manpower / needed_manpower) * 1500;
				order.payment = industry.willing_payment;
				order.good = input;
				order.industry = &industry;
				order.province = province;
				order.type = ORDER_INDUSTRIAL;
				world.orders.push_back(order);
			}

			if(industry.can_do_output(world) == false)
				continue;

			// Now produce anything as we can!
			// Place deliver orders (we are a RGO)
			for(size_t k = 0; k < it.outputs.size(); k++) {
				DeliverGoods deliver;
				deliver.quantity = (available_manpower / needed_manpower) * 2000;
				deliver.payment = industry.willing_payment;
				deliver.good = it.outputs[k];
				deliver.industry = &industry;
				deliver.province = province;
				deliver.product = industry.output_products[k];

				// Do not overproduce
				Product& product = *deliver.product;
				if(product.supply > product.demand) {
					continue;
				}

				// Cannot be below production cost, so we can be profitable
				if(product.price < industry.production_cost * 1.2f) {
					product.price = industry.production_cost * 1.2f;
				}
				
				if(!deliver.quantity)
					continue;
				
				world.delivers.push_back(deliver);
			}

			// Willing payment is made for next day ;)
			industry.willing_payment = industry.budget / it.inputs.size() + it.outputs.size();
		}
	}

	// Outposts working on units and boats will also request materials
	for(const auto& outpost: world.outposts) {
		// Building the outpost itself
		for(const auto& good: outpost->req_goods) {
			if(!good.second)
				continue;

			OrderGoods order;
			order.quantity = good.second;
			// TODO: Make this dynamic
			order.payment = good.second * 5.f;
			order.good = good.first;
			order.outpost = outpost;
			order.type = ORDER_OUTPOST_BUILD;
			world.orders.push_back(order);
		}

		// TODO: We should deduct and set willing payment from military spendings
		// Building the unit
		for(const auto& good: outpost->req_goods_for_unit) {
			if(!good.second)
				continue;

			OrderGoods order;
			order.quantity = good.second;
			// TODO: Make this dynamic
			order.payment = good.second * 5.f;
			order.good = good.first;
			order.outpost = outpost;
			order.type = ORDER_UNIT_BUILD;
			world.orders.push_back(order);
		}
	}

	// Sort on best payment first
	std::sort(world.delivers.begin(), world.delivers.end(),
		[](const DeliverGoods& lhs, const DeliverGoods& rhs) {
		return lhs.payment > rhs.payment;
	});

	std::sort(world.orders.begin(), world.orders.end(),
		[](const OrderGoods& lhs, const OrderGoods& rhs) {
		return lhs.payment > rhs.payment;
	});
}

/**
* Phase 2 of the economy: Goods are transported all around the world, generating commerce and making them
* be ready for POPs to buy
 */
void Economy::do_phase_2(World& world) {
	while(world.delivers.size() && world.orders.size()) {
		// We will check all transport companies; they will transport in a first-come first-serve fashion
		for(auto& company: world.companies) {
			if(!company->is_transport)
				continue;
			
			// Check all delivers
			for(size_t i = 0; i < world.delivers.size(); i++) {
				DeliverGoods& deliver = world.delivers[i];

				// Is the transport company able to transport from this province?
				if(!company->in_range(deliver.province))
					continue;
				
				const Province* deliver_province = deliver.province;
				const Policies& deliver_policy = deliver_province->owner->current_policy;
				Industry* deliver_industry = deliver.industry;

				// Check all orders
				for(size_t j = 0; j < world.orders.size(); j++) {
					OrderGoods& order = world.orders[j];
					
					// Do they want these goods?
					if(order.good != deliver.good)
						continue;
					
					// Is this transport company able to transport to this province?
					if(!company->in_range(order.province))
						continue;
					
					Province* order_province = order.province;
					const Policies& order_policy = order_province->owner->current_policy;

					// If foreign trade is not allowed, then order owner === sender owner
					if(deliver_policy.foreign_trade == false
					|| order_policy.foreign_trade == false) {
						// Trade not allowed
						if(order_province->owner != deliver_province->owner) {
							continue;
						}
					}
					
					const float order_cost = deliver.product->price * std::min(order.quantity, deliver.quantity);
					const float deliver_cost = deliver.product->price * std::min(order.quantity, deliver.quantity);
					
					float total_order_cost, total_deliver_cost;
					
					// International trade
					if(order.province->owner != deliver.province->owner) {
						total_order_cost = order_cost * order_policy.import_tax;
						total_deliver_cost = deliver_cost * order_policy.export_tax;
					}
					// Domestic trade
					else {
						total_order_cost = order_cost * order_policy.domestic_import_tax;
						total_deliver_cost = deliver_cost * order_policy.domestic_export_tax;
					}
					
					// Orders payment should also cover the import tax and a deliver payment should also cover the export
					// tax too. Otherwise we can't deliver
					if(order.payment < total_order_cost && total_order_cost > 0.f) {
						if(order.type == ORDER_INDUSTRIAL) {
							order.industry->willing_payment = total_order_cost;
						}
						continue;
					} else if(deliver.payment < total_deliver_cost && total_deliver_cost > 0.f) {
						deliver_industry->willing_payment = total_deliver_cost;
						continue;
					}

					// Must have above minimum quality to be accepted
					if(order.type == ORDER_INDUSTRIAL && deliver.product->quality < order.industry->min_quality) {
						continue;
					}
					
					// Give both goverments their part of the tax (when tax is 1.0< then the goverment pays for it)
					order_province->owner->budget += total_order_cost - order_cost;
					deliver_province->owner->budget += total_deliver_cost - deliver_cost;
					
					// Province receives a small (military) supply buff from commerce
					order_province->supply_rem += 5.f;
					order_province->supply_rem = std::min(order_province->supply_limit, order_province->supply_rem);
					
					// Obtain number of goods that we can satisfy
					size_t count = std::min<size_t>(order.quantity, deliver.quantity);
					deliver.quantity -= count;
					order.quantity -= count;
					
					// Increment demand of the product, and decrement supply when the demand is fullfilled
					deliver.product->demand += count;
					if(deliver.product->supply) {
						const size_t satisfied = std::min<size_t>(count, deliver.product->supply);
						deliver.product->supply -= satisfied;
						deliver.product->demand += satisfied;
					}

					if(order.type == ORDER_INDUSTRIAL) {
						// Duplicate products and put them into the province's stock (a commerce buff)
						order.industry->add_to_stock(world, order.good, count);

						// Increment the production cost of this industry which is used
						// so we sell our product at a profit instead  of at a loss
						order.industry->production_cost += deliver.product->price;

						// Set quality to the max from this product
						order.industry->min_quality = std::max(order.industry->min_quality, deliver.product->quality);
					} else if(order.type == ORDER_OUTPOST_BUILD) {
						// The outpost will take the production materials
						// and use them for building the unit
						order.outpost->owner->budget -= total_order_cost;
						for(auto& p: order.outpost->req_goods) {
							if(p.first != deliver.good) {
								continue;
							}
							p.second -= std::min(deliver.quantity, p.second);
						}
					} else if(order.type == ORDER_UNIT_BUILD) {
						// TODO: We should deduct and set willing payment from military spendings
						order.outpost->owner->budget -= total_order_cost;
						for(auto& p: order.outpost->req_goods) {
							if(p.first != deliver.good) {
								continue;
							}
							p.second -= std::min(deliver.quantity, p.second);
						}
					}
					
					// Delete this deliver and order tickets from the system since
					// they are now fullfilled (only delete when no quanity left)
					if(!order.quantity) {
						world.orders.erase(world.orders.begin() + j);
					} if(!deliver.quantity) {
						world.delivers.erase(world.delivers.begin() + i);
						i--;
					}
					break;
				}
			}
		}
		
		// Drop all rejected delivers who didn't got transported
		for(size_t i = 0; i < world.delivers.size(); i++) {
			const DeliverGoods* deliver = &world.delivers[i];
			Product* product = deliver->product;
				
			// Add up to province stockpile
			deliver->province->stockpile[world.get_id(product)] += deliver->quantity;
			
			// Increment supply because of incremented stockpile
			product->supply += deliver->quantity;
			if(product->demand) {
				product->demand -= std::min<float>(deliver->quantity, product->demand);
			}
		}
		world.delivers.clear();

		// Industries who did not got anything will get desesperate

		break;
	}
	world.delivers.clear();
	world.orders.clear();
}

class Emigrated {
public:
	Province* target;
	Pop* emigred;
	size_t size;
};
/**
* Phase 3 of economy: POPs buy the aforementioned products and take from the province's stockpile
 */
void Economy::do_phase_3(World& world) {
	// Now, it's like 1 am here, but i will try to write a very nice economic system
	// TODO: There is a lot to fix here, first the economy system commits inverse great depression and goes way too happy
	
	std::mutex emigration_lock;
	std::vector<Emigrated> emigration;
	emigration.clear();
	
	std::for_each(std::execution::par_unseq, world.provinces.begin(), world.provinces.end(),
	[&emigration_lock, &emigration, &world](auto& province) {
		if(province->owner == nullptr) {
			return;
		}
		
		// Vector containing available products in the province (for faster calculations)
		std::vector<Product *> province_products;
		province_products.clear();
		province_products.reserve(world.products.size());
		for(const auto& product: world.products) {
			// Only valid indices
			if(world.get_id(product) == (ProductId)-1)
				continue;

			// Province must have stockpile
			if(!province->stockpile[world.get_id(product)])
				continue;
			
			province_products.push_back(product);
		}
		
		float current_attractive = province->base_attractive;
		current_attractive += province->owner->base_literacy;
		current_attractive += province->owner->gdp / 1000.f;

		// Reset worker pool
		province->worker_pool = 0;
		for(size_t i = 0; i < province->pops.size(); i++) {
			Pop& pop = province->pops[i];
			
			float salary;
			
			// TODO: This is very stupid
			switch(pop.type_id) {
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
				for(auto& taught_pops: province->pops) {
					taught_pops.literacy += std::min<float>(0.001f, (pop.literacy* pop.size) / 100000.f);
					taught_pops.literacy = std::min<float>(0.001f, taught_pops.literacy);
				}
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
				salary = 0.5f;
				break;
			}
			
			// TODO: Make this dynamic
			pop.budget += salary* 90.f;

			// Use 33% of our budget to buy edibles and life needed stuff
			float life_alloc_budget = pop.budget / 3;

			// Use 10% of our budget for buying uneeded commodities and shit
			// TODO: Should lower spending with higher literacy, and higher
			// TODO: Higher the fullfilment per unit with higher literacy
			float everyday_alloc_budget = pop.budget / 10;
			for(const auto& product: province_products) {
				// Province must have stockpile
				if(!province->stockpile[world.get_id(product)]) {
					continue;
				}
				
				size_t bought = 0;
				if(product->good->is_edible) {
					// We can only spend our allocated budget
					// TODO: Base spending on literacy, more literacy == less spending
					bought = life_alloc_budget / product->price;
				} else {
					bought = everyday_alloc_budget / product->price;
				}
				
				bought = std::min<float>(bought, province->stockpile[world.get_id(product)]);
				if(!bought)
					continue;
				
				// Demand is incremented proportional to items bought
				product->demand += bought;
				
				// Reduce supply
				if(product->supply) {
					product->supply -= std::min<float>(bought, product->supply);
				}
				
				// Deduct from budget, and remove item from stockpile
				pop.budget -= bought* product->price;
				province->stockpile[world.get_id(product)] -= bought;

				if(product->good->is_edible) {
					life_alloc_budget -= bought* product->price;
					pop.life_needs_met += (float)pop.size / (float)bought;
				} else {
					everyday_alloc_budget -= bought* product->price;
					pop.everyday_needs_met += (float)pop.size / (float)bought;
				}
			}

			// x1.5 life needs met modifier, that is the max allowed
			pop.life_needs_met = std::min<float>(0.1f, pop.life_needs_met);
			pop.life_needs_met = std::max<float>(-0.1f, pop.life_needs_met);

			// POPs cannot shrink below 10<
			if(pop.size <= 10) {
				// Population cannot be 0
				pop.size = std::max<size_t>(1, pop.size);
				pop.size += rand() % std::min<size_t>(500, std::max<size_t>(1, (pop.size / 10000)));
			} else {
				// Higher literacy will mean there will be less births due to sex education
				// and will also mean that - there would be less deaths due to higher knewledge
				ssize_t growth = pop.life_needs_met / (pop.literacy* 10.f);
				if(growth < 0 && (size_t)abs(growth) >= pop.size) {
					pop.size = 1;
					continue;
				} else {
					pop.size += growth;
				}
			}
			
			// Met life needs means less militancy
			if(pop.life_needs_met >= 1.f) {
				pop.life_needs_met = 1.f;
				if(pop.militancy > 0.f) {
					pop.militancy -= 0.0002f;
					pop.consciousness -= 0.0001f;
				}
			} else {
				pop.militancy += 0.01f;
				pop.consciousness += 0.01f;
			}
			
			// Depending on how much not our life needs are being met is how many we
			// want to get out of here
			// And literacy determines "best" spot, for example a low literacy will
			// choose a slightly less desirable location
			const float emigration_willing = 1.f / std::min(pop.life_needs_met, -1.0f);
			size_t emigreers = (float)(((float)pop.size / 1000.f)* emigration_willing);
			if(emigreers < pop.size && emigreers) {
				// Find best province
				Province* best_province = nullptr;
				for(size_t j = 0; j < world.provinces.size(); j += std::max<size_t>((rand() % (world.provinces.size() - j)) / 10, 1)) {
					Province* target_province = world.provinces[j];
					float attractive = 0.f;
					
					if(target_province->owner == nullptr) {
						continue;
					}
					
					// Account for literacy difference
					attractive -= province->owner->base_literacy;
					
					// Account for GDP difference
					attractive -= province->owner->gdp / 1000.f;
					
					// For the lower class, lower taxes is good, and so on for other POPs
					if(target_province->owner->current_policy.poor_flat_tax < province->owner->current_policy.poor_flat_tax
					&& (pop.type_id == POP_TYPE_FARMER
					|| pop.type_id == POP_TYPE_SOLDIER
					|| pop.type_id == POP_TYPE_LABORER
					|| pop.type_id == POP_TYPE_SLAVE)) {
						attractive = target_province->owner->current_policy.poor_flat_tax - province->owner->current_policy.poor_flat_tax;
					}
					// For the medium class
					else if(target_province->owner->current_policy.med_flat_tax < province->owner->current_policy.med_flat_tax
					&& (pop.type_id == POP_TYPE_ARTISAN
					|| pop.type_id == POP_TYPE_BUREAUCRAT
					|| pop.type_id == POP_TYPE_CLERGYMEN
					|| pop.type_id == POP_TYPE_OFFICER)) {
						attractive = target_province->owner->current_policy.med_flat_tax - province->owner->current_policy.med_flat_tax;
					}
					// For the high class
					else if(target_province->owner->current_policy.rich_flat_tax < province->owner->current_policy.rich_flat_tax
					&& (pop.type_id == POP_TYPE_ENTRPRENEUR
					|| pop.type_id == POP_TYPE_ARISTOCRAT)) {
						attractive = target_province->owner->current_policy.rich_flat_tax - province->owner->current_policy.rich_flat_tax;
					}
					
					if(attractive > current_attractive) {
						if(target_province->owner->current_policy.immigration == ALLOW_NOBODY) {
							// Nobody is allowed in
							continue;
						} else if(target_province->owner->current_policy.immigration == ALLOW_ACCEPTED_CULTURES) {
							// See if we are accepted culture
							bool is_accepted = false;
							for(const auto& culture: target_province->owner->accepted_cultures) {
								if(culture == g_world->cultures[pop.culture_id]) {
									is_accepted = true;
									break;
								}
							}
							
							if(is_accepted == false) {
								continue;
							}
						} else if(target_province->owner->current_policy.immigration == ALLOW_ALL_PAYMENT) {
							// See if we can afford the tax
							if(pop.budget < ((pop.budget / 1000.f)* target_province->owner->current_policy.import_tax)) {
								continue;
							}
						} else if(target_province->owner->current_policy.immigration == ALLOW_ALL) {
							// Everyone is allowed in
						}
						
						current_attractive = attractive;
						best_province = target_province;
					}
				}
				
				// If best not found then we don't go to anywhere
				if(best_province == nullptr) {
					goto skip_emigration;
				}
				
				printf("Emigrating %s -> %s, about %zu\n", province->name.c_str(), best_province->name.c_str(), emigreers);
				
				Emigrated emigrated;
				emigrated.target = best_province;
				emigrated.emigred = &pop;
				emigrated.size = emigreers;
				
				emigration_lock.lock();
				emigration.push_back(emigrated);
				emigration_lock.unlock();
			}
			
		skip_emigration:
			pop.life_needs_met -= 0.7f* std::min<float>(0.5f, 1.f - pop.literacy);

			province->worker_pool += pop.size / (1 + ((pop.militancy * pop.life_needs_met) * pop.consciousness));
		}
		
		// Stockpiles cleared
		memset(&province->stockpile[0], 0, province->stockpile.size()* sizeof(province->stockpile[0]));
	});
	
	// Now time to do the emigration
	for(const auto& target: emigration) {
		Province* province = target.target;
		Pop* pop = target.emigred;
		size_t size = target.size;
		
		// Reduce size of POP, so we don't have duplicates
		pop->size -= size;
		
		// A new pop, this is the representation of the POP in the target
		// province, if no pop of same culture, same religion and same
		// employment exists then we create a new one
		Pop* new_pop = nullptr;
		for(auto& p_pop: province->pops) {
			if(p_pop.culture_id == pop->culture_id
			&& p_pop.religion_id == pop->religion_id
			&& p_pop.type_id == pop->type_id) {
				new_pop = &p_pop;
				break;
			}
		}
		
		// Compatible POP does not exist on target province, so we create
		// a new one (copy the original POP) and add it to the province
		if(new_pop == nullptr) {
			province->pops.push_back(*pop);
		}
		new_pop->size += size;
	}
}

/**
* Phase 4 of the economy: After all POPs buy their products and all factories have transported their goods (or not)
* the price of each product is calculated and all markets are closed until the next day
 */
void Economy::do_phase_4(World& world) {
	// Preparations for the next tick

	// Reset production costs
	for(const auto& province: world.provinces) {
		for(auto& industry: province->industries) {
			industry.production_cost = 0.f;
		}
	}

	// Close today's price with a change according to demand - supply
	for(const auto& product: world.products) {
		if(product->demand > product->supply) {
			product->price_vel += 0.0001f* (product->demand - product->supply);
		} else if(product->demand < product->supply) {
			product->price_vel -= 0.0001f* (product->supply - product->demand);
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
			product->supply += province->stockpile[world.get_id(product)];
		}
		product->demand = 0;
	}
}
