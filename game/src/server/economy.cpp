// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      server/economy.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <cstdio>

#include "action.hpp"
#include "server/economy.hpp"
#include "world.hpp"
#include "unified_render/print.hpp"
#include "unified_render/serializer.hpp"
#include "io_impl.hpp"
#include "server/server_network.hpp"
#include "unified_render/thread_pool.hpp"
#include "product.hpp"
#include "good.hpp"
#include "emigration.hpp"

#undef min
#undef max

// Visual Studio does not define ssize_t because it's a POSIX-only type
#ifdef _MSC_VER
typedef signed int ssize_t;
#endif

// Structure that represents a person emigrating from a province to another

class Workers {
public:
    Pop& pop;
    size_t amount;
    Workers(Pop& _pop): pop{ _pop }, amount{ _pop.size } {};

    Workers& operator=(const Workers& workers) {
        pop = workers.pop;
        amount = workers.amount;
        return *this;
    }
};

class AvailableWorkers {
public:
    std::vector<Workers> entrepreneurs{};
    std::vector<Workers> farmers{};
    std::vector<Workers> laborers{};
    // AvailableWorkers() {};
};

static inline std::vector<AvailableWorkers> get_available_workers(World& world) {
    std::vector<AvailableWorkers> available_workers;
    for(const auto& province : world.provinces) {
        // Province must have a controller
        if(province->controller == nullptr) {
            available_workers.push_back(AvailableWorkers{});
            continue;
        }

        AvailableWorkers province_workers{};
        for(auto& pop : province->pops) {
            if(pop.type->group == PopGroup::Slave || pop.type->group == PopGroup::Other) {
                continue;
            }

            Workers workers{ pop };
            if(!province->controller->is_accepted_culture(pop)) {
                // POPs of non-accepted cultures on exterminate mode cannot get jobs
                if(province->controller->current_policy.treatment == TREATMENT_EXTERMINATE) {
                    continue;
                }
                else if(province->controller->current_policy.treatment == TREATMENT_ONLY_ACCEPTED) {
                    workers.amount /= 2;
                }
            }

            switch(pop.type->group) {
            case PopGroup::BURGEOISE:
                province_workers.entrepreneurs.push_back(workers);
                break;
            case PopGroup::FARMER:
                province_workers.farmers.push_back(workers);
                break;
            case PopGroup::LABORER:
                province_workers.laborers.push_back(workers);
                break;
            default:
                break;
            }
        }
        available_workers.push_back(province_workers);
    }
    return available_workers;
}

// Phase 1 of economy: Delivers & Orders are sent from all factories in the world
void Economy::do_tick(World& world) {
    auto available_workers = get_available_workers(world);
    auto orders_good = std::vector<std::vector<OrderGoods>>(world.goods.size());

    // Buildings who have fullfilled requirements to build stuff will spawn a unit
    for(size_t j = 0; j < world.buildings.size(); j++) {
        auto& building = world.buildings[j];

        auto* province = building->get_province();
        if(province->controller == nullptr) continue;

        size_t needed_laborers = 0, available_laborers = 0;
        size_t needed_farmers = 0, available_farmers = 0;
        size_t needed_entrepreneurs = 0, available_entrepreneurs = 0;
        if(building->type->is_factory) {
            // Each output adds a required farmer or laborer depending on the type
            // of output, it also requires entrepreneurs to "manage" the operations
            // of the factory
            size_t i = 0;
            for(const auto& output : building->output_products) {
                // The minimum amount of workers needed is given by the employees_needed_per_output vector :)
                //const size_t employed = std::min<size_t>(std::pow(10.f * (output->supply - output->demand) / std::max<size_t>(output->demand, 1), 2), building->employees_needed_per_output[i]);
                const size_t employed = 500;
                if(output->good->is_edible) {
                    needed_farmers += employed;
                }
                else {
                    needed_laborers += employed;
                }
                needed_entrepreneurs += employed / 100;
                ++i;
            }
        }
        else {
            needed_laborers = 50;
            needed_farmers = 50;
            needed_entrepreneurs = 50;
        }

        const Province::Id province_id = world.get_id(province);
        auto province_workers = available_workers[province_id];
        // Search through all the job requests

        // Industries require 2 (or 3) types of POPs to correctly function
        // - Laborers: They are needed to produce non-edible food
        // - Farmers: They are needed to produce edibles
        // - BURGEOISE: They help "organize" the factory
        for(size_t i = 0; i < province_workers.farmers.size(); i++) {
            if(available_farmers >= needed_farmers) break;

            Workers& workers = province_workers.farmers[i];

            const size_t employed = std::min(needed_farmers - available_farmers, workers.amount);
            available_farmers += employed;

            // Give pay to the POP
            const float payment = employed * province->controller->current_policy.min_wage;
            workers.pop.budget += payment * building->get_owner()->get_salary_paid_mod();
            building->budget -= payment;
            workers.amount -= employed;

            // Delete job request when it has 0 amount
            if(!workers.amount) {
                province_workers.farmers.erase(province_workers.farmers.begin() + i);
                --i;
                continue;
            }
        }

        for(size_t i = 0; i < province_workers.laborers.size(); i++) {
            if(available_laborers >= needed_laborers) break;
            Workers& workers = province_workers.laborers[i];

            const size_t employed = std::min(needed_laborers - available_laborers, workers.amount);
            available_laborers += employed;

            // Give pay to the POP
            const float payment = employed * province->controller->current_policy.min_wage;
            workers.pop.budget += payment * building->get_owner()->get_salary_paid_mod();
            building->budget -= payment;
            workers.amount -= employed;

            // Delete job request when it has 0 amount
            if(!workers.amount) {
                province_workers.laborers.erase(province_workers.laborers.begin() + i);
                --i;
                continue;
            }
        }

        for(size_t i = 0; i < province_workers.entrepreneurs.size(); i++) {
            if(available_entrepreneurs >= needed_entrepreneurs) break;
            Workers& workers = province_workers.entrepreneurs[i];

            const size_t employed = std::min(needed_entrepreneurs - available_entrepreneurs, workers.amount);
            available_entrepreneurs += employed;

            // Give pay to the POP
            const float payment = employed * province->controller->current_policy.min_wage;
            workers.pop.budget += payment * building->get_owner()->get_salary_paid_mod();
            building->budget -= payment;
            workers.amount -= employed;

            // Delete job request when it has 0 amount
            if(!workers.amount) {
                province_workers.entrepreneurs.erase(province_workers.entrepreneurs.begin() + i);
                --i;
                continue;
            }
        }

        if(building->working_unit_type != nullptr) {
            bool can_build_unit = true;
            for(const auto& req : building->req_goods_for_unit) {
                // Increment demand for all products with same required good type
                for(auto& product : world.products) {
                    if(product->good != req.first) {
                        continue;
                    }

                    // Government-required supplies are super important for companies
                    product->demand += std::max(100.f, req.second / 1000.f);
                }

                if(req.second) {
                    can_build_unit = false;
                    break;
                }
            }

            // TODO: Make a proper supply chain system with the whole working economy thing :)
            // NOTE: Uncomment when it's ready :)
            //can_build_unit = true;
            if(can_build_unit) {
                // Spawn a unit
                Unit* unit = new Unit();
                unit->set_province(*building->get_province());
                unit->type = building->working_unit_type;
                unit->owner = building->get_owner();
                unit->budget = 5000.f;
                unit->experience = 1.f;
                unit->morale = 1.f;
                unit->supply = 1.f;
                unit->defensive_ticks = 0;
                unit->size = unit->type->max_health;
                unit->base = unit->size;
                
                // Notify all clients of the server about this new unit
                building->working_unit_type = nullptr;
                world.insert(unit);

                UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
                Archive ar = Archive();
                ActionType action = ActionType::UNIT_ADD;
                ::serialize(ar, &action); // ActionInt
                ::serialize(ar, unit); // UnitObj
                packet.data(ar.get_buffer(), ar.size());
                g_server->broadcast(packet);

                print_info("[%s]: Has built an unit of [%s]", building->get_province()->ref_name.c_str(), unit->type->ref_name.c_str());
            }
        }

        if(building->type->is_factory) {
            if(building->budget < 0.f) {
                {
                    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
                    Archive ar = Archive();
                    ActionType action = ActionType::BUILDING_REMOVE;
                    ::serialize(ar, &action);
                    ::serialize(ar, &building);
                    packet.data(ar.get_buffer(), ar.size());
                    g_server->broadcast(packet);
                }

                print_info("Building of [%s] in [%s] has closed down!", building->type->ref_name.c_str(), province->ref_name.c_str());

                world.remove(building);
                delete building;
                j--;
                continue;
            }

            building->workers = available_farmers + available_laborers + available_entrepreneurs;
            /*print_info("[%s]: %zu workers on building of type [%s]", building->get_province()->ref_name.c_str(), building->workers, building->type->ref_name.c_str());
            print_info("- %zu farmers (%zu needed)", available_farmers, needed_farmers);
            print_info("- %zu laborers (%zu needed)", available_laborers, needed_laborers);
            print_info("- %zu entrepreneurs (%zu needed)", available_entrepreneurs, needed_entrepreneurs);*/
            if(!building->workers) {
                building->days_unoperational++;

                // TODO: We should tax building daily income instead of by it's total budget
                const float loss_by_tax = building->budget * province->controller->current_policy.industry_tax;
                building->budget -= loss_by_tax;
                province->controller->budget += loss_by_tax;

                print_info("[%s]: %zu workers on building of type [%s]", building->get_province()->ref_name.c_str(), 0, building->type->ref_name.c_str());
                continue;
            }
            building->days_unoperational = 0;

            world.orders_mutex.lock();
            for(const auto& input : building->type->inputs) {
                OrderGoods order = {};

                order.payment = building->willing_payment;
                order.good = input;
                order.building = building;
                order.province = province;
                order.type = OrderType::INDUSTRIAL;

                // Farmers can only work with edibles and laborers can only work for edibles
                if(input->is_edible) {
                    order.quantity = (available_farmers / needed_farmers) * 5000;
                } else {
                    order.quantity = (available_laborers / needed_laborers) * 5000;
                }
                
                if(!order.quantity) {
                    continue;
                }
                order.quantity *= building->get_owner()->get_industry_input_mod();

                world.orders.push_back(order);

                // Increase demand for all products with same good type as ordered (incentivizing companies to create more of this)
                for(auto& product : world.products) {
                    if(product->good != order.good) {
                        continue;
                    }

                    product->demand += std::min(100.f, order.quantity / 1000.f);
                }
            }
            world.orders_mutex.unlock();

            //if(!building->can_do_output()) continue;

            // Now produce anything as we can!
            // Place deliver orders (we are a RGO)
            world.delivers_mutex.lock();
            for(size_t k = 0; k < building->type->outputs.size(); k++) {
                DeliverGoods deliver = {};

                deliver.payment = building->willing_payment;
                deliver.good = building->type->outputs[k];
                deliver.building = building;
                deliver.province = province;
                deliver.product = building->output_products[k];

                if(deliver.good->is_edible) {
                    deliver.quantity = (available_farmers / needed_farmers) * 5000;
                } else {
                    deliver.quantity = (available_laborers / needed_laborers) * 5000;
                }

                if(!deliver.quantity) {
                    continue;
                }

                deliver.quantity *= building->get_owner()->get_industry_output_mod();

                // Cannot be below production cost, so we can be profitable and we need
                // to raise prices
                if(deliver.product->price < building->production_cost * 1.2f) {
                    deliver.product->price = building->production_cost * 1.2f;
                }

                world.delivers.push_back(deliver);
            }
            world.delivers_mutex.unlock();

            // Willing payment is made for next day ;)
            building->willing_payment = building->budget / building->type->inputs.size() + building->type->outputs.size();
        }

        // Building the building itself
        for(const auto& good : building->req_goods) {
            if(!good.second) {
                continue;
            }

            OrderGoods order = {};
            order.quantity = good.second;
            order.quantity *= building->get_owner()->get_industry_input_mod();
            // TODO: Make this dynamic
            order.payment = building->willing_payment;
            order.good = good.first;
            order.building = building;
            order.province = building->get_province();
            order.type = OrderType::BUILDING;
            world.orders.push_back(order);
        }

        // TODO: We should deduct and set willing payment from military spendings
        // Building an unit
        for(const auto& good : building->req_goods_for_unit) {
            if(!good.second) {
                continue;
            }

            OrderGoods order = {};
            order.quantity = good.second;
            order.quantity *= building->get_owner()->get_industry_input_mod();
            // TODO: Make this dynamic
            order.payment = building->willing_payment;
            order.good = good.first;
            order.building = building;
            order.province = building->get_province();
            order.type = OrderType::UNIT;
            world.orders.push_back(order);
        }
    }

    {
        // Take opportunity to also send an update about our buildings
        UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
        Archive ar = Archive();
        ActionType action = ActionType::BUILDING_UPDATE;
        ::serialize(ar, &action); // ActionInt
        Building::Id size = world.buildings.size();
        ::serialize(ar, &size);
        for(const auto& building : world.buildings) {
            ::serialize(ar, &building); // BuildingRef
            ::serialize(ar, building); // BuildingObj
        }
        packet.data(ar.get_buffer(), ar.size());
        g_server->broadcast(packet);
    }

    // Sort on best payment first for orders and delivers
    std::sort(world.delivers.begin(), world.delivers.end(), [](const DeliverGoods& lhs, const DeliverGoods& rhs) {
        return lhs.payment > rhs.payment;
    });
    std::sort(world.orders.begin(), world.orders.end(), [](const OrderGoods& lhs, const OrderGoods& rhs) {
        return lhs.payment > rhs.payment;
    });

    orders_good.shrink_to_fit();
    for(const auto& order : world.orders) {
        // TODO: This should NOT happen!!!
        if(order.good == nullptr) {
            continue;
        }
        orders_good[world.get_id(order.good)].push_back(order);
    }

    // Phase 2 of the economy: Goods are transported all around the world, generating commerce and making them
    // be ready for POPs to buy

    // We will check all transport companies; they will transport in a first-come first-serve fashion
    // Check all delivers
    for(size_t i = 0; i < world.delivers.size(); i++) {
        DeliverGoods& deliver = world.delivers[i];

        const Policies& deliver_policy = deliver.province->controller->current_policy;
        // Building* deliver_building = deliver.building;

        // Check all orders
        auto& order_list = orders_good[world.get_id(deliver.good)];
        //for(size_t j = 0; j < world.orders.size(); j++) {
        for(size_t j = 0; j < order_list.size(); j++) {
            OrderGoods& order = order_list[j];
            const Policies& order_policy = order.province->controller->current_policy;

            // If foreign trade is not allowed, then order controller === sender controller
            if(!deliver_policy.foreign_trade || !order_policy.foreign_trade) {
                // Trade not allowed
                if(order.province->controller != deliver.province->controller) {
                    continue;
                }
            }

            const float order_cost = deliver.product->price * std::min(order.quantity, deliver.quantity);
            const float deliver_cost = deliver.product->price * std::min(order.quantity, deliver.quantity);

            float total_order_cost, total_deliver_cost;
            if(order.province->controller != deliver.province->controller) {
                // International trade
                total_order_cost = order_cost * order_policy.import_tax;
                total_deliver_cost = deliver_cost * order_policy.export_tax;
            } else {
                // Domestic trade
                total_order_cost = order_cost * order_policy.domestic_import_tax;
                total_deliver_cost = deliver_cost * order_policy.domestic_export_tax;
            }

            // Orders payment should also cover the import tax and a deliver payment should also cover the export
            // tax too. Otherwise we can't deliver
            if(order.payment < total_order_cost && total_order_cost > 0.f) {
                if(order.type == OrderType::INDUSTRIAL) {
                    order.building->willing_payment = total_order_cost;
                }
                continue;
            } else if(deliver.payment < total_deliver_cost && total_deliver_cost > 0.f) {
                deliver.building->willing_payment = total_deliver_cost;
                continue;
            }

            // if(order.type == OrderType::INDUSTRIAL) continue;

            // Give both goverments their part of the tax (when tax is 1.0< then the goverment pays for it)
            order.province->controller->budget += total_order_cost - order_cost;
            deliver.province->controller->budget += total_deliver_cost - deliver_cost;

            // Province receives a small (military) supply buff from commerce
            order.province->supply_rem += 5.f;
            order.province->supply_rem = std::min(order.province->supply_limit, order.province->supply_rem);

            // Obtain number of goods that we can satisfy
            size_t count = std::min<size_t>(order.quantity, deliver.quantity);
            if(order.type == OrderType::INDUSTRIAL) {
                // Duplicate products and put them into the province's stock (a commerce buff)
                order.building->add_to_stock(*order.good, count);

                // Increment the production cost of this building which is used
                // so we sell our product at a profit instead  of at a loss
                order.building->production_cost += deliver.product->price;
            } else if(order.type == OrderType::BUILDING) {
                // The building will take the production materials
                // and use them for building the unit
                order.building->get_owner()->budget -= total_order_cost;
                for(auto& p : order.building->req_goods) {
                    if(p.first != deliver.good) {
                        continue;
                    }

                    p.second -= std::min(p.second, count);
                }
            } else if(order.type == OrderType::UNIT) {
                // TODO: We should deduct and set willing payment from military spendings
                order.building->get_owner()->budget -= total_order_cost;
                for(auto& p : order.building->req_goods) {
                    if(p.first != deliver.good) {
                        continue;
                    }

                    p.second -= std::min(p.second, count);
                    print_info("Delivered %zu goods (%zu remaining!), of type %s", count, p.second, order.good->ref_name.c_str());
                }
            } else if(order.type == OrderType::POP) {
                // Nobody is to be billed ... transport company still obtains their money & delivers to the province
                //order.province->stockpile[world.get_id(deliver.product)] += order.quantity;
            }

            // Add to stockpile (duplicate items) to the province at each transporting
            deliver.quantity -= count;
            order.quantity -= count;
            order.province->stockpile[world.get_id(deliver.product->good)] += count;

            deliver.product->supply += deliver.quantity;

            // Delete this deliver and order tickets from the system since
            // they are now fullfilled (only delete when no quanity left)
            if(!order.quantity) {
                order_list.erase(order_list.begin() + j);
            } if(!deliver.quantity) {
                world.delivers.erase(world.delivers.begin() + i);
                i--;
            }
            break;
        }
    }

    // Delete the remaining orders
    orders_good.clear();
    world.orders.clear();

    // The remaining delivers gets dropped and just simply add up the province's stockpile
    // Drop all rejected delivers who didn't got transported
    for(size_t i = 0; i < world.delivers.size(); i++) {
        const DeliverGoods& deliver = world.delivers[i];

        // Add up to province stockpile
        deliver.province->stockpile[world.get_id(deliver.product->good)] += deliver.quantity;

        // Increment supply because of incremented stockpile
        deliver.product->supply += deliver.quantity;
    }
    world.delivers.clear();

    // TODO: Place stockpiles

    // Phase 3 of economy: POPs buy the aforementioned products and take from the province's stockpile

    // Now, it's like 1 am here, but i will try to write a very nice economic system
    // TODO: There is a lot to fix here, first the economy system commits inverse great depression and goes way too happy
    std::for_each(world.provinces.begin(), world.provinces.end(), [&world](auto& province) {
        if(province->controller == nullptr) {
            return;
        }

        if(province->terrain_type->is_water_body) {
            return;
        }

        //std::vector<Product*> province_products = province->get_products();
        for(size_t i = 0; i < province->pops.size(); i++) {
            Pop& pop = province->pops[i];
            // We want to get rid of many POPs as possible
            if(!pop.size) {
                province->pops.erase(province->pops.begin() + i);
                i--;
                continue;
            }

            // Use 10% of our budget for buying uneeded commodities and shit
            // TODO: Should lower spending with higher literacy, and higher
            // TODO: Higher the fullfilment per unit with higher literacy
            // TODO: DO NOT MAKE POP BUY FROM STOCKPILE, INSTEAD MAKE THEM BUY FROM ORDERS
            for(const auto& good : world.goods) {
                // Province must have stockpile
                if(!province->stockpile[world.get_id(good)]) {
                    // POPs will order it so hopefully transport companies send it
                    OrderGoods order = OrderGoods{};
                    order.quantity = pop.size;
                    order.good = good;
                    order.payment = pop.budget;
                    order.province = province;
                    order.quantity = 1000;
                    order.type = OrderType::POP;
                    world.orders.push_back(order);
                    continue;
                }

                unsigned int bought;
                bought = std::rand() % pop.size;
                if(!good->is_edible) {
                    // Slaves cannot buy commodities
                    if(pop.type->group == PopGroup::Slave) {
                        continue;
                    }
                }

                // Only buy the available stuff
                bought = std::min<unsigned int>(bought, province->stockpile[world.get_id(good)]);
                if(!bought) {
                    continue;
                }

                // Delete items from stockpile
                province->stockpile[world.get_id(good)] -= std::min<unsigned int>(province->stockpile[world.get_id(good)], bought);

                // Uncomment to see buyers
                //print_info("Pop with budget %f bought %zu %s", pop.budget, (size_t)bought, product->good->name.c_str());

                if(good->is_edible) {
                    pop.life_needs_met += (float)pop.size / (float)bought;
                } else {
                    pop.everyday_needs_met += (float)pop.size / (float)bought;
                }
            }

            // Humans can survive 8 days without eating anything
            pop.life_needs_met -= 0.01f;

            // x2.5 life needs met modifier, that is the max allowed
            pop.life_needs_met = std::min<float>(1.2f, std::max<float>(pop.life_needs_met, -5.f));
            pop.everyday_needs_met = std::min<float>(1.5f, std::max<float>(pop.everyday_needs_met, -5.f));

            // Current liking of the party is influenced by the life_needs_met
            pop.ideology_approval[world.get_id(province->controller->ideology)] += (pop.life_needs_met + 1.f) / 10.f;

            // Higher literacy will mean there will be less births due to sex education
            // and will also mean that - there would be less deaths due to higher knewledge
            int growth;

            // NOTE: We used to have this thing where anything below 2.5 meant everyone dies
            // and this was removed because it's such an unescesary detail that consumes precious
            // CPU branching prediction... and we can't afford that!

            // Starvation in -1 or 0 or >1 are amortized by literacy
            growth = pop.life_needs_met / pop.literacy;

            if(growth < 0 && (size_t)std::abs(growth) > pop.size) {
                growth = -pop.size;
            }

            growth *= (growth > 0) ? province->controller->get_reproduction_mod() : province->controller->get_death_mod();
            pop.size += growth;

            // Add some RNG to shake things up and make gameplay more dynamic and less deterministic :)
            pop.size += std::rand() % std::min<size_t>(5, std::max<size_t>(1, pop.size / 10000));
            pop.size = std::max<size_t>(1, pop.size);

            // Met life needs means less militancy
            // For example, having 1.0 life needs means that we obtain -0.01 militancy per ecotick
            // and the opposite happens with negative life needs
            pop.militancy += 0.01f * (-pop.life_needs_met);
            pop.con += 0.01f * (-pop.life_needs_met);

            pop.militancy += 0.01f * province->controller->get_militancy_mod();
            pop.con += 0.01f * province->controller->get_militancy_mod();
        }
    });
    do_emigration(world);

    // Chances of a coup/rebellion increment for the global militancy
    for(auto& nation : world.nations) {
        // Nation must actually exist
        if(!nation->exists()) {
            continue;
        }

        // Do research on focused research
        if(nation->focus_tech != nullptr) {
            const float research = nation->get_research_points() / nation->focus_tech->cost;
            nation->research[world.get_id(nation->focus_tech)] += research;
        }

        // Total anger in population (global)
        float total_anger = 0.f;
        // Anger per ideology (how much we hate the current ideology)
        std::vector<float> ideology_anger(world.ideologies.size(), 0.f);
        const float coup_chances = 1000.f;
        for(const auto& province : nation->owned_provinces) {
            // Nobody can coup from occupied territory!
            if(province->controller != nation) {
                continue;
            }

            for(const auto& pop : province->pops) {
                // TODO: Ok, look, the justification is that educated people
                // almost never do coups - in comparasion to uneducated
                // peseants, rich people don't need to protest!
                const float anger = (pop.militancy * pop.con) / std::max(pop.literacy, 1.f) / std::max(pop.life_needs_met, 0.f);
                total_anger += anger;

                for(const auto& ideology : world.ideologies) {
                    ideology_anger[world.get_id(ideology)] += (pop.ideology_approval[world.get_id(ideology)] * anger) * (pop.size / 1000.f);
                }
            }
        }

        // Rebellions!
        // TODO: Broadcast this event to other people, maybe a REBEL_UPRISE action with a list of uprising provinces?
        if(std::fmod(rand(), std::max(coup_chances, coup_chances - total_anger)) <= 100.f) {
            // Compile list of uprising provinces
            std::vector<Province*> uprising_provinces;
            for(const auto& province : nation->owned_provinces) {
                float province_anger = 0.f;
                float province_threshold = 0.f;
                for(const auto& pop : province->pops) {
                    province_anger += pop.militancy * pop.con;
                    province_threshold += pop.literacy * pop.life_needs_met;
                }

                if(province_anger > province_threshold) {
                    uprising_provinces.push_back(province);
                }
            }

            // Make the most angry provinces revolt!
            for(auto& province : uprising_provinces) {
                // TODO: We should make a copy of the `rebel` nation for every rebellion!!!
                // TODO: We should also give them an unique ideology!!!
                world.nations[0]->give_province(*province);
                for(auto& building : province->get_buildings()) {
                    building->owner = world.nations[0];
                }

                for(auto& unit : province->get_units()) {
                    unit->owner = world.nations[0];
                }
            }
        }

        // Roll a dice! (more probability with more anger!)
        if(std::fmod(rand(), std::max(coup_chances, coup_chances - total_anger)) <= 100.f) {
            // Choose the ideology with most "anger" (the one more probable to coup d'
            // etat) - amgry radicals will surely throw off the current administration
            // while peaceful people won't
            int idx = std::distance(ideology_anger.begin(), std::max_element(ideology_anger.begin(), ideology_anger.end()));

            // Ideology_anger and ideologies are mapped 1:1 - so we just pick up the associated ideology
            // Apply the policies of the ideology
            nation->current_policy = world.ideologies[idx]->policies;

            // Switch ideologies of nation
            nation->ideology = world.ideologies[idx];

            // People who are aligned to the ideology are VERY happy now
            for(const auto& province : nation->owned_provinces) {
                for(auto& pop : province->pops) {
                    pop.militancy = -50.f;
                }
            }

            print_info("Coup d' etat on [%s]! [%s] has taken over!", nation->ref_name.c_str(), nation->ideology->ref_name.c_str());
        }
    }

    // Phase 4 of the economy: After all POPs buy their products and all factories have transported their goods (or not)
    // the price of each product is calculated and all markets are closed until the next day

    // Reset production costs
    for(auto& building : world.buildings) {
        building->production_cost = 0.f;
    }

    // Close today's price with a change according to demand - supply
    for(const auto& product : world.products) {
        if(product == nullptr) {
            continue;
        }

        // Uncomment to see supply-demand
        //if(product->price_vel && product->price > 0.01f)
        //    print_info("%s; Supply: %zu, Demand: %zu, Price %.2f", product->good->name.c_str(), product->supply, product->demand, product->price);

        product->close_market();
        product->demand = 0;
    }
}
