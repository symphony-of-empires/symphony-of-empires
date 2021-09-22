#include <algorithm>
#include <cstdio>

#include "../actions.hpp"
#include "economy.hpp"
#include "../world.hpp"
#include "../print.hpp"
#include "../serializer.hpp"
#include "../io_impl.hpp"
#include "server_network.hpp"
#include "../thread_pool.hpp"
#include "../product.hpp"
#include "../good.hpp"
#include "../company.hpp"

#if (__cplusplus < 201703L)
namespace std {
    template<typename T>
    constexpr inline T clamp(T n, T min, T max) {
        return std::min<T>(max, std::max<T>(n, min));
    }
}
#endif

/* Visual Studio does not define ssize_t because it's a POSIX-only type */
#ifdef _MSC_VER
typedef signed int ssize_t;
#endif

/**
* Checks if the building can produce output (if it has enough input)
 */
bool Building::can_do_output(const World& world) {
    // No output products?
    if(type->outputs.empty() || output_products.empty())
        return false;

    // Always can produce if RGO
    if(type->inputs.empty())
        return true;

    // Check that we have enough stockpile
    for(const auto& stock: this->stockpile) {
        if(!stock)
            return false;
    }
    return true;
}

/**
* Adds a good by id to a building stockpile
 */
void Building::add_to_stock(const World& world, const Good* good, const size_t add) {
    for(size_t i = 0; i < stockpile.size(); i++) {
        if(world.get_id(type->inputs.at(i)) == world.get_id(good)) {
            stockpile.at(i) += add;
            break;
        }
    }
}

Province* Building::get_province(const World& world) {
    if(this->type->is_plot_on_land == false) {
        return nullptr;
    }
    return world.provinces.at(world.get_tile(this->x, this->y).province_id);
}

/**
* Phase 1 of economy: Delivers & Orders are sent from all factories in the world
 */
void Economy::do_phase_1(World& world) {
    // Buildings who have fullfilled requirements to build stuff will spawna  lil' unit/boat
    for(size_t i = 0; i < world.buildings.size(); i++) {
        auto& building = world.buildings.at(i);
        auto province = building->get_province(world);

        bool can_build = true;
        for(const auto& req: building->req_goods_for_unit) {
            // Increment demand for all products with same required good type
            for(auto& product: world.products) {
                if(product->good != req.first)
                    continue;
                
                // Government-required supplies are super important for companies
                product->demand += req.second * 1.5f;
            }

            if(req.second) {
                can_build = false;
            }
        }

        if(!can_build)
        	break;
        
        size_t needed_manpower = 1000;
        size_t available_manpower = 0;
        for(size_t j = 0; j < world.job_requests.size(); j++) {
            JobRequest& job_request = world.job_requests.at(j);
            // Buildings on water-desertland
            if(province == nullptr) {
                // Accept anything
            }
            // Buildings on land must have same province as the job request
            else if(job_request.province != province) {
                continue;
            }

            // Give pay to the POP
            job_request.pop->budget += 10.f;
            building->owner->budget -= 10.f * job_request.amount;
                
            available_manpower += std::min(needed_manpower, job_request.amount);
            job_request.amount -= std::min(needed_manpower, job_request.amount);

            // Delete job request when it has 0 amount
            if(!job_request.amount) {
                world.job_requests.erase(world.job_requests.begin() + j);
                j--;
                continue;
            }
        }

        if(building->working_unit_type != nullptr) {
            // Spawn a unit
            Unit* unit = new Unit();
            unit->x = building->x;
            unit->y = building->y;
            unit->type = building->working_unit_type;
            unit->tx = unit->x;
            unit->ty = unit->y;
            unit->owner = building->owner;
            unit->budget = 5000.f;
            unit->experience = 1.f;
            unit->morale = 1.f;
            unit->supply = 1.f;
            unit->defensive_ticks = 0;
            unit->size = unit->type->max_health;
            unit->base = unit->size;
            
            // Notify all clients of the server about this new unit
            g_world->units.push_back(unit);
            building->working_unit_type = nullptr;

            Packet packet = Packet();
            Archive ar = Archive();
            ActionType action = ActionType::UNIT_ADD;
            ::serialize(ar, &action); // ActionInt
            ::serialize(ar, unit); // UnitObj
            packet.data(ar.get_buffer(), ar.size());
            g_server->broadcast(packet);
        }
        if(building->working_boat_type != nullptr) {
            // Spawn a boat
            Boat* boat = new Boat();
            boat->x = building->x;
            boat->y = building->y;
            boat->type = building->working_boat_type;
            boat->tx = boat->x;
            boat->ty = boat->y;
            boat->owner = building->owner;
            boat->experience = 1.f;
            boat->morale = 1.f;
            boat->supply = 1.f;
            boat->defensive_ticks = 0;
            boat->size = boat->type->max_health;
            boat->base = boat->size;

            // Notify all clients of the server about this new boat
            g_world->boats.push_back(boat);
            building->working_boat_type = nullptr;

            Packet packet = Packet();
            Archive ar = Archive();
            ActionType action = ActionType::BOAT_ADD;
            ::serialize(ar, &action); // ActionInt
            ::serialize(ar, boat); // BoatObj
            packet.data(ar.get_buffer(), ar.size());
            g_server->broadcast(packet);
        }
        if(building->type->is_factory == true && province != nullptr) {
            if(building->budget < 0.f) {
                print_info("Building of %s in %s has closed down!", building->type->name.c_str(), province->name.c_str());



                --i;
                continue;
            }

            // TODO: Needed manpower be calculated by total priority of outputs?
            size_t needed_laborers = 0, available_laborers = 0;
            size_t needed_farmers = 0, available_farmers = 0;
            size_t needed_entrepreneurs = 0, available_entrepreneurs = 0;

            // Each output adds a required farmer or laborer depending on the type
            // of output, it also requires entrepreneurs to "manage" the operations
            // of the factory
            size_t i = 0;
            for(const auto& output: building->output_products) {
                // 100 minimum workers per output
                const size_t employed = std::max<size_t>(std::pow(10.f * (output->supply - output->demand) / std::max<size_t>(output->demand, 1), 2), 100);
                building->employees_needed_per_output.at(i) = employed;

                if(output->good->is_edible) {
                    needed_farmers += employed;
                } else {
                    needed_laborers += employed;
                }
                needed_entrepreneurs += employed / 100;
                ++i;
            }
            
            // Search through all the job requests
            world.job_requests_mutex.lock();
            for(size_t i = 0; i < world.job_requests.size(); i++) {
                JobRequest& job_request = world.job_requests.at(i);
                size_t employed;

                // Industries require 2 (or 3) types of POPs to correctly function
                // - Laborers: They are needed to produce non-edible food
                // - Farmers: They are needed to produce edibles
                // - Entrepreneur: They help "organize" the factory
                if(available_laborers < needed_farmers && (job_request.pop->type_id == POP_TYPE_LABORER
                || job_request.pop->type_id == POP_TYPE_SLAVE)) {
                    employed = std::min(needed_laborers, job_request.amount);
                    available_laborers += employed;
                } else if(available_farmers < needed_farmers && (job_request.pop->type_id == POP_TYPE_FARMER
                || job_request.pop->type_id == POP_TYPE_SLAVE)) {
                    employed = std::min(needed_farmers, job_request.amount);
                    available_farmers += employed;
                } else if(available_entrepreneurs < needed_entrepreneurs && job_request.pop->type_id == POP_TYPE_ENTRPRENEUR) {
                    employed = std::min(needed_entrepreneurs, job_request.amount);
                    available_entrepreneurs += employed;
                } else {
                    // Do not accept anyone else
                    continue;
                }

                // Give pay to the POP
                float payment = employed * province->owner->current_policy.minimum_wage;
                job_request.pop->budget += payment;
                building->budget -= payment;
                job_request.amount -= employed;

                // Delete job request when it has 0 amount
                if(!job_request.amount) {
                    world.job_requests.erase(world.job_requests.begin() + i);
                    --i;
                    continue;
                }
            }
            world.job_requests_mutex.unlock();

            print_info("Building of %s in %s has %zu workers", building->type->name.c_str(), province->name.c_str(), available_manpower);

            building->workers = available_farmers + available_entrepreneurs;
            if(!building->workers) {
                building->days_unoperational++;
                building->min_quality = 0;

                // TODO: We should tax building daily income instead of by it's total budget
                const float loss_by_tax = building->budget * province->owner->current_policy.industry_tax;
                building->budget -= loss_by_tax;
                province->owner->budget += loss_by_tax;
                continue;
            }
            building->days_unoperational = 0;
            
            world.orders_mutex.lock();
            for(const auto& input: building->type->inputs) {
                OrderGoods order;

                // Farmers can only work with edibles and laborers can only work for edibles
                if(input->is_edible) {
                    order.quantity = (available_farmers / needed_farmers) * 5000;
                } else {
                    order.quantity = (available_laborers / needed_laborers) * 5000;
                }
                if(!order.quantity)
                    continue;
                order.payment = building->willing_payment;
                order.good = input;
                order.building = building;
                order.province = province;
                order.type = OrderType::INDUSTRIAL;
                world.orders.push_back(order);

                // Increase demand for all products with same good type as ordered
                // (incentivizing companies to create more of this)
                for(auto& product: world.products) {
                    if(product->good != order.good)
                        continue;
                    
                    product->demand += order.quantity;
                }
            }
            world.orders_mutex.unlock();

            if(!building->can_do_output(world))
                continue;
            
            // Now produce anything as we can!
            // Place deliver orders (we are a RGO)
            world.delivers_mutex.lock();
            for(size_t k = 0; k < building->type->outputs.size(); k++) {
                DeliverGoods deliver;
                
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
                if(!deliver.quantity)
                    continue;

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
        for(const auto& good: building->req_goods) {
            if(!good.second)
                continue;

            OrderGoods order;
            order.quantity = good.second;
            // TODO: Make this dynamic
            order.payment = good.second * 5.f;
            order.good = good.first;
            order.building = building;
            order.type = OrderType::BUILDING;
            world.orders.push_back(order);
        }

        // TODO: We should deduct and set willing payment from military spendings
        // Building an unit
        for(const auto& good: building->req_goods_for_unit) {
            if(!good.second)
                continue;

            OrderGoods order;
            order.quantity = good.second;
            // TODO: Make this dynamic
            order.payment = good.second * 5.f;
            order.good = good.first;
            order.building = building;
            order.type = OrderType::UNIT;
            world.orders.push_back(order);
        }
        // And also for boats :)
        for(const auto& good: building->req_goods_for_boat) {
            if(!good.second)
                continue;

            OrderGoods order;
            order.quantity = good.second;
            // TODO: Make this dynamic
            order.payment = good.second * 5.f;
            order.good = good.first;
            order.building = building;
            order.type = OrderType::UNIT;
            world.orders.push_back(order);
        }
        
        // Take opportunity to also send an update about our buildings
        Packet packet = Packet();
        Archive ar = Archive();
        ActionType action = ActionType::BUILDING_UPDATE;
        ::serialize(ar, &action); // ActionInt
        ::serialize(ar, &building); // BuildingRef
        ::serialize(ar, building); // BuildingObj
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
}

// Phase 2 of the economy: Goods are transported all around the world, generating commerce and making them
// be ready for POPs to buy
void Economy::do_phase_2(World& world) {
    while(world.delivers.size() && world.orders.size()) {
        // We will check all transport companies; they will transport in a first-come first-serve fashion
        for(auto& company: world.companies) {
            if(!company->is_transport)
                continue;
            
            // Check all delivers
            for(size_t i = 0; i < world.delivers.size(); i++) {
                DeliverGoods& deliver = world.delivers.at(i);

                // Is the transport company able to transport from this province?
                if(!company->in_range(deliver.province))
                    continue;
                
                const Province* deliver_province = deliver.province;
                const Policies& deliver_policy = deliver_province->owner->current_policy;
                Building* deliver_building = deliver.building;

                // Check all orders
                for(size_t j = 0; j < world.orders.size(); j++) {
                    OrderGoods& order = world.orders.at(j);
                    
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
                        if(order.type == OrderType::INDUSTRIAL) {
                            order.building->willing_payment = total_order_cost;
                        }
                        continue;
                    } else if(deliver.payment < total_deliver_cost && total_deliver_cost > 0.f) {
                        deliver.building->willing_payment = total_deliver_cost;
                        continue;
                    }

                    // Must have above minimum quality to be accepted
                    if(order.type == OrderType::INDUSTRIAL && deliver.product->quality < order.building->min_quality) {
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
                    // Add to stockpile (duplicate items) to the province at each transporting
                    order.province->stockpile[world.get_id(deliver.product)] += count;
                    deliver.quantity -= count;
                    order.quantity -= count;
                    
                    // Increment demand of the product, and decrement supply when the demand is fullfilled
                    deliver.product->demand += count;

                    if(order.type == OrderType::INDUSTRIAL) {
                        // Duplicate products and put them into the province's stock (a commerce buff)
                        order.building->add_to_stock(world, order.good, count);

                        // Increment the production cost of this building which is used
                        // so we sell our product at a profit instead  of at a loss
                        order.building->production_cost += deliver.product->price;

                        // Set quality to the max from this product
                        order.building->min_quality = std::max(order.building->min_quality, deliver.product->quality);
                    } else if(order.type == OrderType::BUILDING) {
                        // The building will take the production materials
                        // and use them for building the unit
                        order.building->owner->budget -= total_order_cost;
                        for(auto& p: order.building->req_goods) {
                            if(p.first != deliver.good)
                                continue;
                            p.second -= std::min(deliver.quantity, p.second);
                        }
                    } else if(order.type == OrderType::UNIT) {
                        // TODO: We should deduct and set willing payment from military spendings
                        order.building->owner->budget -= total_order_cost;
                        for(auto& p: order.building->req_goods) {
                            if(p.first != deliver.good)
                                continue;
                            p.second -= std::min(deliver.quantity, p.second);
                        }
                    }

                    deliver.product->supply += deliver.quantity;
                    
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

        // Industries who did not got anything will get desesperate
        break;
    }

    // The remaining delivers gets dropped and just simply add up the province's stockpile
    // Drop all rejected delivers who didn't got transported
    for(size_t i = 0; i < world.delivers.size(); i++) {
        const DeliverGoods* deliver = &world.delivers.at(i);
        Product* product = deliver->product;
                
        // Add up to province stockpile
        deliver->province->stockpile[world.get_id(product)] += deliver->quantity;
            
        // Increment supply because of incremented stockpile
        product->supply += deliver->quantity;
    }
    world.delivers.clear();
    world.orders.clear();

    // Uncomment to see stockpiles
    for(const auto& province: world.provinces) {
        for(size_t i = 0; i < province->stockpile.size(); i++) {
            if(!province->stockpile.at(i))
                continue;
                
            print_info("%zu of %s produced in %s - stockpiled by %s", province->stockpile.at(i), world.products.at(i)->good->name.c_str(), world.products.at(i)->origin->name.c_str(), province->name.c_str());
        }
    }
}

// Structure that represents a person emigrating from a province to another
class Emigrated {
public:
    Emigrated() {};
    ~Emigrated() {};
    Province* origin;
    Province* target;
    Pop emigred;
    size_t size;
};

// Phase 3 of economy: POPs buy the aforementioned products and take from the province's stockpile
void Economy::do_phase_3(World& world) {
    // Now, it's like 1 am here, but i will try to write a very nice economic system
    // TODO: There is a lot to fix here, first the economy system commits inverse great depression and goes way too happy
    std::vector<Emigrated> emigration = std::vector<Emigrated>();
    std::mutex emigration_lock;

    std::srand(std::time(nullptr));
    std::for_each(world.provinces.begin(), world.provinces.end(), [&emigration_lock, &emigration, &world](auto& province) {
        if(province->owner == nullptr)
            return;
        
        std::vector<Product *> province_products = province->get_products(world);
        
        float current_attractive = province->base_attractive;
        current_attractive += province->owner->base_literacy;
        current_attractive += province->owner->gdp / 1000.f;

        // Reset worker pool
        province->worker_pool = 0;
        for(size_t i = 0; i < province->pops.size(); i++) {
            Pop& pop = province->pops.at(i);

            // Use 33% of our budget to buy edibles and life needed stuff
            float life_alloc_budget = pop.budget / 3;

            // Use 10% of our budget for buying uneeded commodities and shit
            // TODO: Should lower spending with higher literacy, and higher
            // TODO: Higher the fullfilment per unit with higher literacy
            // TODO: Should sort "product" by priority (i.e with highest quality and best marketing)
            float everyday_alloc_budget = pop.budget / 10;
            for(const auto& product: province_products) {
                // Province must have stockpile
                if(!province->stockpile[world.get_id(product)]) {
                    // Desesperation for food leads to higher demand
                    if(product->good->is_edible && pop.life_needs_met <= 0.f) {
                        product->demand += pop.size * 5.f;
                    }
                    continue;
                }
                
                size_t bought = 0;
                if(product->good->is_edible) {
                    // We can only spend our allocated budget
                    bought = life_alloc_budget / product->price;
                } else {
                    bought = everyday_alloc_budget / product->price;

                    // Slaves cannot buy commodities
                    if(pop.type_id == POP_TYPE_SLAVE) {
                        continue;
                    }
                }
                
                bought = std::min<float>(bought, province->stockpile[world.get_id(product)]);
                if(!bought)
                    continue;

                float cost_of_transaction = bought * product->price;

                // Take in account taxes for the product
                // TODO: Have something affect tax efficiency! - complexity at it's finest :)
                province->owner->budget += (cost_of_transaction * province->owner->get_tax(pop)) - cost_of_transaction;
                cost_of_transaction *= province->owner->get_tax(pop);
                pop.budget -= cost_of_transaction;

                // Demand is incremented proportional to items bought and remove item from stockpile
                // we will also add some "randomness" factor to simulate a pseudo-imperfect economy
                float errdata = std::fmod((float)(std::rand() + 1) / 1000.f, 2.f) + 1.f;
                product->demand += bought * 2.5f * errdata;
                province->stockpile[world.get_id(product)] -= std::min<size_t>(province->stockpile[world.get_id(product)], bought);

                // Uncomment to see buyers
                //print_info("Pop with budget %f bought %zu %s", pop.budget, (size_t)bought, product->good->name.c_str());

                if(product->good->is_edible) {
                    life_alloc_budget -= bought * product->price;
                    pop.life_needs_met += (float)pop.size / (float)bought;
                } else {
                    everyday_alloc_budget -= bought * product->price;
                    pop.everyday_needs_met += (float)pop.size / (float)bought;
                }
            }

            // Humans can survive 8 days without eating anything
            pop.life_needs_met -= 0.01f;

            // x2.5 life needs met modifier, that is the max allowed
            pop.life_needs_met = std::clamp<float>(pop.life_needs_met, -3.f, 5.f);
            pop.everyday_needs_met = std::clamp<float>(pop.everyday_needs_met, -3.f, 5.f);

            // POPs cannot shrink below 10<
            if(pop.size <= 10) {
                pop.size = 1;
            } else {
                // Higher literacy will mean there will be less births due to sex education
                // and will also mean that - there would be less deaths due to higher knewledge
                int growth;
                if(pop.life_needs_met >= -0.15f) {
                    // Starvation in -1 or 0 or >1 are amortized by literacy
                    growth = pop.life_needs_met / (pop.literacy * 0.01f);
                } else {
                    // Neither literacy nor anything else can save humans from
                    // dying due starvation
                    growth = -((int)(std::rand() % pop.size));
                }
                if(growth < 0 && (size_t)std::abs(growth) > pop.size) {
                    growth = -((int)pop.size);
                }
                pop.size += growth;
            }

            // Add some RNG to shake things up and make gameplay more dynamic and less deterministic :)
            pop.size += std::rand() % std::min<size_t>(5, std::max<size_t>(1, (pop.size / 10000)));

            // Population cannot be 0
            pop.size = std::max<size_t>(1, pop.size);
            
            // Met life needs means less militancy
            if(pop.life_needs_met >= 1.f) {
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
            const float emigration_willing = 1.f / std::min(pop.life_needs_met, 0.f);
            long long int emigreers = (pop.size * emigration_willing) + std::rand() % pop.size;
            if(emigreers > 0) {
                // Check that laws on the province we are in allows for emigration
                if(province->owner->current_policy.migration == ALLOW_NOBODY) {
                    goto skip_emigration;
                } else if(province->owner->current_policy.migration == ALLOW_ACCEPTED_CULTURES) {
                    if(province->owner->is_accepted_culture(pop) == false) {
                        goto skip_emigration;
                    }
                } else if(province->owner->current_policy.migration == ALLOW_ALL_PAYMENT) {
                    // See if we can afford the tax
                    if(pop.budget < ((pop.budget / 1000.f) * province->owner->current_policy.export_tax)) {
                        continue;
                    }
                }

                // Find best province
                Province* best_province = nullptr;
                for(size_t j = 0; j < world.provinces.size(); j += std::max<size_t>((std::rand() % (world.provinces.size() - j)) / 10, 1)) {
                    Province* target_province = world.provinces.at(j);
                    float attractive = 0.f;
                    
                    // Don't go to owner-less provinces
                    if(target_province->owner == nullptr) {
                        continue;
                    }

                    // We dont want to be exterminated or enslaved... do we?
                    if(target_province->owner->current_policy.treatment == TREATMENT_ENSLAVED) {
                        attractive -= 2.5f;
                    } else if(target_province->owner->current_policy.treatment == TREATMENT_EXTERMINATE) {
                        attractive -= 5.f;
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
                            bool is_accepted = target_province->owner->is_accepted_culture(pop);
                            if(!is_accepted) {
                                continue;
                            }
                        } else if(target_province->owner->current_policy.immigration == ALLOW_ALL_PAYMENT) {
                            // See if we can afford the tax
                            if(pop.budget < ((pop.budget / 1000.f) * target_province->owner->current_policy.import_tax)) {
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
                    // Or we do, but just randomly
                    best_province = world.provinces[std::rand() % world.provinces.size()];
                    //goto skip_emigration;
                }
                
                //print_info("Emigrating %s -> %s, about %lli", province->name.c_str(), best_province->name.c_str(), emigreers);
                
                Emigrated emigrated = {};
                emigrated.target = best_province;
                emigrated.emigred = pop;
                emigrated.size = emigreers;
                emigrated.origin = province;
                
                emigration_lock.lock();
                emigration.push_back(emigrated);
                emigration_lock.unlock();
            }
        skip_emigration:
            ;
        }

        std::fill(province->stockpile.begin(), province->stockpile.end(), 0);
    });

    // Now time to do the emigration - we will create a new POP on the province
    // if a POP with similar culture, religion and type does not exist - and we
    // will also subtract the amount of emigrated from the original POP to not
    // create clones
    for(const auto& target: emigration) {
        auto pop = std::find(target.origin->pops.begin(), target.origin->pops.end(), target.emigred);
        pop->size -= target.size;

        auto new_pop = std::find(target.target->pops.begin(), target.target->pops.end(), *pop);
        if(new_pop == target.target->pops.end()) {
            target.target->pops.push_back(*pop);
            new_pop = target.target->pops.end();
            new_pop->size = target.size;
        } else {
            new_pop->size += target.size;
        }
    }
    emigration.clear();

    // Please do not modify the POPs vector in provinces after this, otherwise the pointers
    // can get invalidated which would result in disaster

    // We will now post a job request so the next economic tick will be able to "link buildings"
    // with their workers and make a somewhat realistic economy
    world.job_requests.clear();
    for(const auto& province: world.provinces) {
        // Province must have an owner
        if(province->owner == nullptr)
            continue;
        
        for(auto& pop: province->pops) {
            // Post a job request
            JobRequest request = {};
            request.amount = pop.size;
            request.pop = &pop;
            request.province = province;

            // Are there any discriminative policies?
            if(province->owner->current_policy.treatment == TREATMENT_EXTERMINATE) {
                // POPs of non-accepted cultures on exterminate mode cannot get jobs
                if(province->owner->is_accepted_culture(pop) == false) {
                    continue;
                }
            } else if(province->owner->current_policy.treatment == TREATMENT_ONLY_ACCEPTED) {
                // Same as above except we roll a dice
                if(province->owner->is_accepted_culture(pop) == false) {
                    request.amount /= (size_t)std::fmod(std::rand() + 1.f, 32.f) + 1;
                }
            }

            world.job_requests.push_back(request);
        }
    }
}

/**
* Phase 4 of the economy: After all POPs buy their products and all factories have transported their goods (or not)
* the price of each product is calculated and all markets are closed until the next day
 */
void Economy::do_phase_4(World& world) {
    // Preparations for the next tick

    // Reset production costs
    for(auto& building: world.buildings) {
        building->production_cost = 0.f;
    }

    // Close today's price with a change according to demand - supply
    for(const auto& product: world.products) {
        if(product == nullptr)
            continue;

        // Uncomment to see supply-demand
        if(product->price_vel && product->price > 0.01f)
            print_info("%s; Supply: %zu, Demand: %zu, Price %.2f", product->good->name.c_str(), product->supply, product->demand, product->price);

        if(product->demand > product->supply) {
            product->price_vel += 0.0001f * (product->demand - product->supply);
        } else if(product->demand < product->supply) {
            product->price_vel -= 0.0001f * (product->supply - product->demand);
        } else {
            // Gravitate towards absolute zero due to volatility decay
            // (i.e, product price becomes stable without market activity)
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
