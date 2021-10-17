#include "../../diplomacy.hpp"
#include "../../policy.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <set>
#ifndef _MSC_VER
#	include <sys/cdefs.h>
#endif
#include <libintl.h>
#include <locale.h>

#include "../../province.hpp"
#include "../economy.hpp"
#include "../../world.hpp"
#include "../../binary_image.hpp"
#include "../lua_api.hpp"
#include "../../path.hpp"
#include "../../print.hpp"
#include "../../serializer.hpp"
#include "../../io_impl.hpp"
#include "../server_network.hpp"

Good* ai_get_potential_good(Nation* nation, World* world) {
	// Analyze the market probability of sucess of a product
	// this is determined by (demand * price), we calculate
	// this from the average of all products on our provinces we currently own

	// Obtain the average probability of profitability per good
	// we will only however, take in account products which are produced on provinces
	// we own, hereafter we can obtain the average of the national good industry

	// So our formula would be:
	// Sucess = Sum(Demand / (Supply + 1) * Price)
	std::vector<float> avg_prob = std::vector<float>(world->goods.size(), 0.f);
	for(const auto& product : world->products) {
		if(product->origin->owner != nation) {
			continue;
		}

		avg_prob[world->get_id(product->good)] += product->demand / (product->supply + 1) * product->price;
	}

	// Now, we will take in account 2 factors:
	// - First, an industry which takes-in a good with high sucess and outputs something
	// should have that something have a sucess probability equal or higher to the taked
	// good
	// - Second, we preferably want the A.I to complete the supply chain of all industries
	// so doing the method described above should increase the priority for filling out higher
	// level industries
	for(const auto& building : world->buildings) {
		const Province* province = building->get_province(*world);
		if(province == nullptr || province->owner != nation) {
			continue;
		}

		// We will only take in account industrial buildings
		// TODO: Take in account other buildings as well
		if(building->type->is_factory == false) {
			continue;
		}

		for(const auto& input : building->type->inputs) {
			// Apply the higher-probability with outputs of this factory
			for(const auto& output : building->type->outputs) {
				avg_prob[world->get_id(output)] += avg_prob[world->get_id(input)];
			}
		}
	}

	for(size_t i = 0; i < world->goods.size(); i++) {
		print_info("%s: Good %s has %f probability", nation->name.c_str(), world->goods[i]->name.c_str(), avg_prob[i]);
	}

	// Obtain the index of the highest element (the one with more sucess rate)
	return world->goods.at(std::distance(avg_prob.begin(), std::max_element(avg_prob.begin(), avg_prob.end())));
}

void ai_do_tick(Nation* nation, World* world) {
	//print_info("%d", i++);
	if(nation->exists() == false)
		return;

	if(nation->diplomatic_timer != 0) {
		nation->diplomatic_timer--;
	}

	// Increase relations
	if(!(std::rand() % 500)) {
		Nation* target = nullptr;
		while(target == nullptr || target->exists() == false) {
			target = world->nations[rand() % world->nations.size()];
		}
		nation->increase_relation(*target);
	}
	// Decrease relations
	if(!(std::rand() % 500)) {
		Nation* target = nullptr;
		while(target == nullptr || target->exists() == false) {
			target = world->nations[rand() % world->nations.size()];
		}
		nation->decrease_relation(*target);
	}
	// Rarely nations will change policies
	if(!(std::rand() % 500)) {
		Policies new_policy = nation->current_policy;

		if(rand() % 100 > 50.f) {
			new_policy.import_tax += 0.1f * (rand() % 10);
		}
		else if(rand() % 100 > 50.f) {
			new_policy.import_tax -= 0.1f * (rand() % 10);
		}

		if(rand() % 100 > 50.f) {
			new_policy.export_tax += 0.1f * (rand() % 10);
		}
		else if(rand() % 100 > 50.f) {
			new_policy.export_tax -= 0.1f * (rand() % 10);
		}

		if(rand() % 100 > 50.f) {
			new_policy.domestic_export_tax += 0.1f * (rand() % 10);
		}
		else if(rand() % 100 > 50.f) {
			new_policy.domestic_export_tax -= 0.1f * (rand() % 10);
		}

		if(rand() % 100 > 50.f) {
			new_policy.domestic_import_tax += 0.1f * (rand() % 10);
		}
		else if(rand() % 100 > 50.f) {
			new_policy.domestic_import_tax -= 0.1f * (rand() % 10);
		}

		if(rand() % 100 > 50.f) {
			new_policy.industry_tax += 0.1f * (rand() % 10);
		}
		else if(rand() % 100 > 50.f) {
			new_policy.industry_tax -= 0.1f * (rand() % 10);
		}

		nation->set_policy(new_policy);
	}
	// Colonize a province
	if(!(std::rand() % 500)) {
		Province* target = world->provinces[rand() % world->provinces.size()];
		if(target->owner == nullptr) {
			Packet packet = Packet();
			Archive ar = Archive();
			ActionType action = ActionType::PROVINCE_COLONIZE;
			::serialize(ar, &action);
			::serialize(ar, &target);
			::serialize(ar, target);
			packet.data(ar.get_buffer(), ar.size());
			g_server->broadcast(packet);

			nation->give_province(*world, *target);
			print_info("Conquering %s for %s", target->name.c_str(), nation->name.c_str());
		}
	}
	// Accepting/rejecting treaties
	 if(!(std::rand() % 500)) {
		for(auto& treaty : world->treaties) {
			for(auto& part : treaty->approval_status) {
				if(part.first == nation) {
					if(part.second == TreatyApproval::ACCEPTED
						|| part.second == TreatyApproval::DENIED) {
						break;
					}

					if(std::rand() % 50 >= 25) {
						print_info("We, %s, deny the treaty of %s", treaty->name.c_str());
						part.second = TreatyApproval::DENIED;
					}
					else {
						print_info("We, %s, accept the treaty of %s", treaty->name.c_str());
						part.second = TreatyApproval::ACCEPTED;
					}
				}
			}
		}
	}
	// Build defenses
	if(!(std::rand() % 500)) {

	}
	// Build a commercially related building
	if(std::rand() % 50 == 0) {
		Good *target_good;
		target_good = ai_get_potential_good(nation, world);
		if(target_good == nullptr) return;

		print_info("Building a commercially related building");

		// Find an industry type which outputs this good
		BuildingType* type = nullptr;
		for(const auto& building_type : world->building_types) {
			if(building_type->is_factory == false) {
				continue;
			}

			for(const auto& input : building_type->inputs) {
				if(input == target_good) {
					type = (BuildingType*)building_type;
					break;
				}
			}

			for(const auto& output : building_type->outputs) {
				if(output == target_good) {
					type = (BuildingType*)building_type;
					break;
				}
			}
		}

		print_info("%s: Good %s seems to be on a high-trend - building industry %s which makes that good", nation->name.c_str(), target_good->name.c_str(), type->name.c_str());

		// Otherwise -- do not build anything since the highest valued good cannot be produced
		if(type == nullptr) {
			return;
		}

		// Now build the building
		Building* building = new Building();

		building->owner = nation;

		// We need something better tbh
		while(building->get_province(*world) == nullptr) {
			building->x = rand() % world->width;
			building->y = rand() % world->height;
		}

		building->working_unit_type = nullptr;
		building->req_goods_for_unit = std::vector<std::pair<Good*, size_t>>();
		building->req_goods = std::vector<std::pair<Good*, size_t>>();
		building->type = type;
		if(building->type->is_factory == true) {
			building->budget = 100.f;
			building->corporate_owner = world->companies.at(std::rand() % world->companies.size());
			building->create_factory(*world);

			building->corporate_owner->operating_provinces.insert(building->get_province(*world));

			for(const auto& product : building->output_products) {
				Packet packet = Packet();
				Archive ar = Archive();
				ActionType action = ActionType::PRODUCT_ADD;
				::serialize(ar, &action);
				::serialize(ar, product);
				packet.data(ar.get_buffer(), ar.size());
				g_server->broadcast(packet);
			}
		}
		world->buildings.push_back(building);

		// Broadcast the addition of the building to the clients
		{
			Packet packet = Packet();
			Archive ar = Archive();
			ActionType action = ActionType::BUILDING_ADD;
			::serialize(ar, &action);
			::serialize(ar, building);
			packet.data(ar.get_buffer(), ar.size());
			g_server->broadcast(packet);
		}
		print_info("Building of %s(%i), from %s built on %s", building->type->name.c_str(), (int)world->get_id(building->type), nation->name.c_str(), building->get_province(*world)->name.c_str());
	}
}
