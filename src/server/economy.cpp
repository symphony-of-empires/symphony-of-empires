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
#include "company.hpp"

// Visual Studio does not define ssize_t because it's a POSIX-only type
#ifdef _MSC_VER
typedef signed int ssize_t;
#endif

// Structure that represents a person emigrating from a province to another
struct Emigrated {
public:
    Emigrated() {};
    ~Emigrated() {};

    Province* origin;
    Province* target;
    Pop emigred;
    size_t size;
};

struct Workers {
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

struct AvailableWorkers {
public:
    std::vector<Workers> entrepreneurs{};
    std::vector<Workers> farmers{};
    std::vector<Workers> laborers{};
    // AvailableWorkers() {};
};
// Phase 1 of economy: Delivers & Orders are sent from all factories in the world
void Economy::do_tick(World& world) {
    std::vector<AvailableWorkers> available_workers;
    for(const auto& province : world.provinces) {
        // Province must have an owner
        if(province->owner == nullptr) {
            available_workers.push_back(AvailableWorkers{});
            continue;
        }

        AvailableWorkers province_workers{};
        for(auto& pop : province->pops) {
            if(pop.type->group == PopGroup::Slave || pop.type->group == PopGroup::Other) {
                continue;
            }

            Workers workers{ pop };
            if(province->owner->is_accepted_culture(pop) == false) {
                // POPs of non-accepted cultures on exterminate mode cannot get jobs
                if(province->owner->current_policy.treatment == TREATMENT_EXTERMINATE) {
                    continue;
                } else if(province->owner->current_policy.treatment == TREATMENT_ONLY_ACCEPTED) {
                    workers.amount /= 2;
                }
            }

            switch(pop.type->group) {
            case PopGroup::Entrepreneur:
                province_workers.entrepreneurs.push_back(workers);
                break;
            case PopGroup::Farmer:
                province_workers.farmers.push_back(workers);
                break;
            case PopGroup::Laborer:
                province_workers.laborers.push_back(workers);
                break;
            default:
                break;
            }
        }
        available_workers.push_back(province_workers);
    }
    // Buildings who have fullfilled requirements to build stuff will spawn a unit
    for(size_t j = 0; j < world.buildings.size(); j++) {
        auto& building = world.buildings[j];
        if(building == nullptr) {
            print_error("Building: %zu, is null!", j);
            continue;
        }

        auto province = building->get_province();
        if(province == nullptr) {
            print_error("Building: %zu of type: %s, province is null!", j, building->type->ref_name.c_str());
            continue;
        }
        if(province->owner == nullptr) continue;

        bool can_build = true;
        for(const auto& req : building->req_goods_for_unit) {
            // Increment demand for all products with same required good type
            for(auto& product : world.products) {
                if(product->good != req.first) continue;

                // Government-required supplies are super important for companies
                product->demand += req.second * 1.5f;
            }

            if(req.second) can_build = false;
        }

        // TODO: Make a proper supply chain system with the whole working economy thing :)
        // NOTE: Uncomment when it's ready :)
        //if(!can_build) break;

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
                const size_t employed = std::max<size_t>(
                    std::pow(10.f * (output->supply - output->demand) / std::max<size_t>(output->demand, 1), 2),
                    building->employees_needed_per_output[i]);

                if(output->good->is_edible) {
                    needed_farmers += employed;
                } else {
                    needed_laborers += employed;
                }
                needed_entrepreneurs += employed / 100;
                ++i;
            }
        } else {
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
        // - Entrepreneur: They help "organize" the factory
        for(size_t i = 0; i < province_workers.farmers.size(); i++) {
            if(available_farmers >= needed_farmers) break;

            Workers& workers = province_workers.farmers[i];

            size_t employed = std::min(needed_farmers - available_farmers, workers.amount);
            available_farmers += employed;

            // Give pay to the POP
            float payment = employed * province->owner->current_policy.min_wage;
            workers.pop.budget += payment * building->get_owner()->get_salary_paid_mod();
            building->budget -= payment;
            workers.amount -= employed;

            // Delete job request when it has 0 amount
            if(!workers.amount) {
                province_workers.farmers.erase(province_workers.farmers.begin() + i);
                --i;
            }
        }
        for(size_t i = 0; i < province_workers.laborers.size(); i++) {
            if(available_laborers >= needed_laborers) break;
            Workers& workers = province_workers.laborers[i];

            size_t employed = std::min(needed_laborers - available_laborers, workers.amount);
            available_laborers += employed;

            // Give pay to the POP
            float payment = employed * province->owner->current_policy.min_wage;
            workers.pop.budget += payment * building->get_owner()->get_salary_paid_mod();
            building->budget -= payment;
            workers.amount -= employed;

            // Delete job request when it has 0 amount
            if(!workers.amount) {
                province_workers.laborers.erase(province_workers.laborers.begin() + i);
                --i;
            }
        }
        for(size_t i = 0; i < province_workers.entrepreneurs.size(); i++) {
            if(available_entrepreneurs >= needed_entrepreneurs) break;
            Workers& workers = province_workers.entrepreneurs[i];

            size_t employed = std::min(needed_entrepreneurs - available_entrepreneurs, workers.amount);
            available_entrepreneurs += employed;

            // Give pay to the POP
            float payment = employed * province->owner->current_policy.min_wage;
            workers.pop.budget += payment * building->get_owner()->get_salary_paid_mod();
            building->budget -= payment;
            workers.amount -= employed;

            // Delete job request when it has 0 amount
            if(!workers.amount) {
                province_workers.entrepreneurs.erase(province_workers.entrepreneurs.begin() + i);
                --i;
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

            Packet packet = Packet();
            Archive ar = Archive();
            ActionType action = ActionType::UNIT_ADD;
            ::serialize(ar, &action); // ActionInt
            ::serialize(ar, unit); // UnitObj
            packet.data(ar.get_buffer(), ar.size());
            g_server->broadcast(packet);
        }

        if(building->type->is_factory == true) {
            if(building->budget < 0.f) {
                {
                    Packet packet = Packet();
                    Archive ar = Archive();
                    ActionType action = ActionType::BUILDING_REMOVE;
                    ::serialize(ar, &action);
                    ::serialize(ar, &building);
                    packet.data(ar.get_buffer(), ar.size());
                    g_server->broadcast(packet);
                }

                for(const auto& product : building->output_products) {
                    Packet packet = Packet();
                    Archive ar = Archive();
                    ActionType action = ActionType::PRODUCT_REMOVE;
                    ::serialize(ar, &action);
                    ::serialize(ar, &product);
                    packet.data(ar.get_buffer(), ar.size());
                    g_server->broadcast(packet);
                }

                print_info("Building of %s in %s has closed down!", building->type->name.c_str(), province->name.c_str());

                world.remove(building);
                delete building;
                --j;
                continue;
            }

            building->workers = available_farmers + available_entrepreneurs;
            //print_info("Building of %s in %s has %zu workers", building->type->name.c_str(), province->name.c_str(), building->workers);
            if(!building->workers) {
                building->days_unoperational++;

                // TODO: We should tax building daily income instead of by it's total budget
                const float loss_by_tax = building->budget * province->owner->current_policy.industry_tax;
                building->budget -= loss_by_tax;
                province->owner->budget += loss_by_tax;
                continue;
            }
            building->days_unoperational = 0;

            world.orders_mutex.lock();
            for(const auto& input : building->type->inputs) {
                OrderGoods order ={};

                // Farmers can only work with edibles and laborers can only work for edibles
                if(input->is_edible) {
                    if(available_farmers) {
                        order.quantity = (available_farmers / needed_farmers) * 5000;
                    }
                } else {
                    if(available_laborers) {
                        order.quantity = (available_laborers / needed_laborers) * 5000;
                    }
                }

                if(!order.quantity) continue;
                order.quantity *= building->get_owner()->get_industry_input_mod();

                order.payment = building->willing_payment;
                order.good = input;
                order.building = building;
                order.province = province;
                order.type = OrderType::INDUSTRIAL;
                world.orders.push_back(order);

                // Increase demand for all products with same good type as ordered
                // (incentivizing companies to create more of this)
                for(auto& product : world.products) {
                    if(product->good != order.good) continue;
                    product->demand += order.quantity;
                }
            }
            world.orders_mutex.unlock();

            if(!building->can_do_output()) continue;

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
                if(!deliver.quantity) continue;
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
            if(!good.second) continue;

            OrderGoods order;
            order.quantity = good.second;
            order.quantity *= building->get_owner()->get_industry_input_mod();
            // TODO: Make this dynamic
            order.payment = good.second * 5.f;
            order.good = good.first;
            order.building = building;
            order.type = OrderType::BUILDING;
            world.orders.push_back(order);
        }

        // TODO: We should deduct and set willing payment from military spendings
        // Building an unit
        for(const auto& good : building->req_goods_for_unit) {
            if(!good.second) continue;

            OrderGoods order;
            order.quantity = good.second;
            order.quantity *= building->get_owner()->get_industry_input_mod();
            // TODO: Make this dynamic
            order.payment = good.second * 5.f;
            order.good = good.first;
            order.building = building;
            order.type = OrderType::UNIT;
            world.orders.push_back(order);
        }
    }

    {
        // Take opportunity to also send an update about our buildings
        Packet packet = Packet();
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

    // Phase 2 of the economy: Goods are transported all around the world, generating commerce and making them
    // be ready for POPs to buy

    // We will check all transport companies; they will transport in a first-come first-serve fashion
    for(auto& company : world.companies) {
        if(!company->is_transport) continue;

        // Check all delivers
        for(size_t i = 0; i < world.delivers.size(); i++) {
            DeliverGoods& deliver = world.delivers[i];

            // Is the transport company able to transport from this province?
            if(!company->in_range(deliver.province)) continue;

            const Province* deliver_province = deliver.province;
            const Policies& deliver_policy = deliver_province->owner->current_policy;
            Building* deliver_building = deliver.building;

            // Check all orders
            for(size_t j = 0; j < world.orders.size(); j++) {
                OrderGoods& order = world.orders[j];

                // Do they want these goods?
                if(order.good != deliver.good) continue;

                // Is this transport company able to transport to this province?
                if(!company->in_range(order.province)) continue;

                Province* order_province = order.province;
                const Policies& order_policy = order_province->owner->current_policy;

                // If foreign trade is not allowed, then order owner === sender owner
                if(!deliver_policy.foreign_trade || !order_policy.foreign_trade) {
                    // Trade not allowed
                    if(order_province->owner != deliver_province->owner) continue;
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
                }
                else if(deliver.payment < total_deliver_cost && total_deliver_cost > 0.f) {
                    deliver.building->willing_payment = total_deliver_cost;
                    continue;
                }

                if(order.type == OrderType::INDUSTRIAL) continue;

                // Give both goverments their part of the tax (when tax is 1.0< then the goverment pays for it)
                order_province->owner->budget += total_order_cost - order_cost;
                deliver_province->owner->budget += total_deliver_cost - deliver_cost;

                // Province receives a small (military) supply buff from commerce
                order_province->supply_rem += 5.f;
                order_province->supply_rem = std::min(order_province->supply_limit, order_province->supply_rem);

                // Obtain number of goods that we can satisfy
                const size_t count = std::min<size_t>(order.quantity, deliver.quantity);
                // Add to stockpile (duplicate items) to the province at each transporting
                order.province->stockpile[world.get_id(deliver.product)] += count;

                deliver.quantity -= count;
                order.quantity -= count;

                // Increment demand of the product, and decrement supply when the demand is fullfilled
                deliver.product->demand += count;

                if(order.type == OrderType::INDUSTRIAL) {
                    // Duplicate products and put them into the province's stock (a commerce buff)
                    order.building->add_to_stock(order.good, count);

                    // Increment the production cost of this building which is used
                    // so we sell our product at a profit instead  of at a loss
                    order.building->production_cost += deliver.product->price;
                }
                else if(order.type == OrderType::BUILDING) {
                    // The building will take the production materials
                    // and use them for building the unit
                    order.building->get_owner()->budget -= total_order_cost;
                    for(auto& p : order.building->req_goods) {
                        if(p.first != deliver.good) continue;
                        p.second -= std::min(deliver.quantity, p.second);
                    }
                }
                else if(order.type == OrderType::UNIT) {
                    // TODO: We should deduct and set willing payment from military spendings
                    order.building->get_owner()->budget -= total_order_cost;
                    for(auto& p : order.building->req_goods) {
                        if(p.first == deliver.good) {
                            p.second -= std::min(deliver.quantity, p.second);
                            break;
                        }
                    }
                }
                // Nobody is to be billed!
                else if(order.type == OrderType::POP) {
                    // ... transport company still obtains their money & delivers to the province
                    order.province->stockpile[world.get_id(deliver.product)] += order.quantity;
                    print_info("Pop requested stuff");
                }

                // Increment the production cost of this building which is used
                // so we sell our product at a profit instead  of at a loss
                order.building->production_cost += deliver.product->price;

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


    // The remaining delivers gets dropped and just simply add up the province's stockpile
    // Drop all rejected delivers who didn't got transported
    for(size_t i = 0; i < world.delivers.size(); i++) {
        const DeliverGoods* deliver = &world.delivers[i];
        Product* product = deliver->product;

        // Add up to province stockpile
        deliver->province->stockpile[world.get_id(product)] += deliver->quantity;

        // Increment supply because of incremented stockpile
        product->supply += deliver->quantity;
    }
    world.delivers.clear();
    world.orders.clear();

    // Uncomment to see stockpiles
    for(const auto& province : world.provinces) {
        //for(size_t i = 0; i < province->stockpile.size(); i++) {
        //    if(!province->stockpile[i]) continue;
        //    print_info("%zu of %s produced in %s - stockpiled by %s", province->stockpile[i], world.products[i]->good->name.c_str(), world.products[i]->origin->name.c_str(), province->name.c_str());
        //}

        //size_t pop_count = 0;
        //for(const auto& pop : province->pops) {
        //    pop_count += pop.size;
        //}
        //print_info("%s has %zu people", province->name.c_str(), pop_count);
    }

    // Phase 3 of economy: POPs buy the aforementioned products and take from the province's stockpile

    // Now, it's like 1 am here, but i will try to write a very nice economic system
    // TODO: There is a lot to fix here, first the economy system commits inverse great depression and goes way too happy
    std::vector<Emigrated> emigration = std::vector<Emigrated>();
    std::mutex emigration_lock;

    std::for_each(world.provinces.begin(), world.provinces.end(), [&emigration_lock, &emigration, &world](auto& province) {
        if(province->owner == nullptr) return;

        std::vector<Product*> province_products = province->get_products();

        // Randomness factor to emulate a pseudo-imperfect economy
        const float fuzz = std::fmod((float)(std::rand() + 1) / 1000.f, 2.f) + 1.f;

        for(size_t i = 0; i < province->pops.size(); i++) {
            Pop& pop = province->pops[i];

            // Use 33% of our budget to buy edibles and life needed stuff
            float life_alloc_budget = pop.budget / 3.f;

            // Use 10% of our budget for buying uneeded commodities and shit
            // TODO: Should lower spending with higher literacy, and higher
            // TODO: Higher the fullfilment per unit with higher literacy
            float everyday_alloc_budget = pop.budget / 10.f;
            for(const auto& product : province_products) {
                const Product::Id product_id = world.get_id(product);

                // Province must have stockpile
                if(!province->stockpile[product_id]) {
                    // POPs will order it so hopefully transport companies send it
                    OrderGoods order = OrderGoods();
                    order.quantity = pop.size;
                    order.province = province;
                    world.orders.push_back(order);

                    // Desesperation for food leads to higher demand
                    if(product->good->is_edible && pop.life_needs_met <= 0.f) {
                        product->demand += pop.size * 5.f;
                    }
                    continue;
                }

                size_t bought;
                if(product->good->is_edible) {
                    // We can only spend our allocated budget
                    bought = life_alloc_budget / product->price;
                    if(life_alloc_budget <= 0.f) continue;
                }
                else {
                    bought = everyday_alloc_budget / product->price;

                    // Slaves cannot buy commodities
                    if(pop.type->group == PopGroup::Slave) continue;
                    if(everyday_alloc_budget <= 0.f) continue;
                }

                // Only buy the needed stuff
                bought = std::min<float>(bought, province->stockpile[product_id]);
                if(!bought) continue;

                float cost_of_transaction = bought * product->price;

                // Take in account taxes for the product
                // TODO: Have something affect tax efficiency! - complexity at it's finest :)
                province->owner->budget += (cost_of_transaction * province->owner->get_tax(pop)) - cost_of_transaction;
                cost_of_transaction *= province->owner->get_tax(pop);
                pop.budget -= cost_of_transaction;

                // Demand is incremented proportional to items bought and remove item from stockpile
                product->demand += bought * fuzz;
                province->stockpile[product_id] -= std::min<size_t>(province->stockpile[product_id], bought);

                // Uncomment to see buyers
                //print_info("Pop with budget %f bought %zu %s", pop.budget, (size_t)bought, product->good->name.c_str());

                if(product->good->is_edible) {
                    life_alloc_budget -= bought * product->price;
                    pop.life_needs_met += (float)pop.size / (float)bought;
                }
                else {
                    everyday_alloc_budget -= bought * product->price;
                    pop.everyday_needs_met += (float)pop.size / (float)bought;
                }
            }

            // Humans can survive 8 days without eating anything
            pop.life_needs_met -= 0.01f;

            // x2.5 life needs met modifier, that is the max allowed
            pop.life_needs_met = std::min<float>(1.2f, std::max<float>(pop.life_needs_met, -5.f));
            pop.everyday_needs_met = std::min<float>(1.5f, std::max<float>(pop.everyday_needs_met, -5.f));

            // POPs cannot shrink below 10
            if(pop.size <= 10) {
                pop.size = 1;
            }
            else {
                // Higher literacy will mean there will be less births due to sex education
                // and will also mean that - there would be less deaths due to higher knewledge
                int growth;

                if(pop.life_needs_met >= -2.5f) {
                    // Starvation in -1 or 0 or >1 are amortized by literacy
                    growth = pop.life_needs_met / pop.literacy;
                }
                // Neither literacy nor anything else can save humans from
                // dying due starvation
                else {
                    growth = -((int)(std::rand() % pop.size));
                }

                if(growth < 0 && (size_t)std::abs(growth) > pop.size) {
                    growth = -pop.size;
                }

                if(growth < 0) {
                    growth *= province->owner->get_death_mod();
                } else {
                    growth *= province->owner->get_reproduction_mod();
                }

                pop.size += growth;
            }

            // Add some RNG to shake things up and make gameplay more dynamic and less deterministic :)
            pop.size += std::rand() % std::min<size_t>(5, std::max<size_t>(1, (pop.size / 10000)));
            pop.size = std::max<size_t>(1, pop.size);

            // Met life needs means less militancy
            if(pop.life_needs_met >= 1.f) {
                if(pop.militancy > 0.f) {
                    pop.militancy -= 0.0002f;
                    pop.con -= 0.0001f;
                }
            } else {
                pop.militancy += 0.01f;
                pop.con += 0.01f;
            }

            pop.militancy += 0.01f * province->owner->get_militancy_mod();
            pop.con += 0.01f * province->owner->get_militancy_mod();

            // Depending on how much not our life needs are being met is how many we
            // want to get out of here
            // And literacy determines "best" spot, for example a low literacy will
            // choose a slightly less desirable location
            const float emigration_willing = std::max<float>(-pop.life_needs_met * std::fmod(fuzz, 10), 0);
            const long long int emigreers = std::fmod(pop.size * emigration_willing + std::rand(), pop.size);
            if(emigreers > 0) {
                float current_attractive = province->get_attractiveness(pop);

                // Check that laws on the province we are in allows for emigration
                if(province->owner->current_policy.migration == ALLOW_NOBODY) {
                    goto skip_emigration;
                }
                else if(province->owner->current_policy.migration == ALLOW_ACCEPTED_CULTURES) {
                    if(province->owner->is_accepted_culture(pop) == false) {
                        goto skip_emigration;
                    }
                }
                else if(province->owner->current_policy.migration == ALLOW_ALL_PAYMENT) {
                    // See if we can afford the tax
                    if(pop.budget < ((pop.budget / 1000.f) * province->owner->current_policy.export_tax)) {
                        continue;
                    }
                }

                //print_info("%zu pops wanting to emigrate!", emigreers);

                // Find best province (that neighbours us)
                Province* best_province = nullptr;
                for(auto target_province : province->neighbours) {
                    // Don't go to owner-less provinces
                    if(target_province->owner == nullptr) continue;

                    const float attractive = target_province->get_attractiveness(pop) * (std::rand() % 16);
                    if(attractive < current_attractive) continue;

                    // Nobody is allowed in
                    if(target_province->owner->current_policy.immigration == ALLOW_NOBODY) {
                        continue;
                    }
                    // Only if we are accepted culture/religion
                    else if(target_province->owner->current_policy.immigration == ALLOW_ACCEPTED_CULTURES) {
                        if(!target_province->owner->is_accepted_culture(pop)) continue;
                    }
                    // Allowed but only if we have money (and we are treated as "imported" good)
                    else if(target_province->owner->current_policy.immigration == ALLOW_ALL_PAYMENT) {
                        if(pop.budget < ((pop.budget / 1000.f) * target_province->owner->current_policy.import_tax)) {
                            continue;
                        }
                    }
                    
                    // Otherwise everyone is allowed in
                    current_attractive = attractive;
                    best_province = target_province;
                }

                // If best not found/same as we are at then we don't go to anywhere
                if(best_province == nullptr || best_province == province) {
                    // Or we do, but just randomly
                    best_province = world.provinces[std::rand() % world.provinces.size()];
                }

                //print_info("Emigrating %s -> %s, about %lli", province->name.c_str(), best_province->name.c_str(), emigreers);

                Emigrated emigrated = Emigrated();
                emigrated.target = best_province;
                emigrated.emigred = pop;
                emigrated.size = emigreers;
                emigrated.origin = province;

                std::scoped_lock lock(emigration_lock);
                emigration.push_back(emigrated);
            }
        skip_emigration:;
        }

        //std::fill(province->stockpile.begin(), province->stockpile.end(), 0);
    });

    // Now time to do the emigration - we will create a new POP on the province
    // if a POP with similar culture, religion and type does not exist - and we
    // will also subtract the amount of emigrated from the original POP to not
    // create clones
    for(const auto& target : emigration) {
        auto pop = std::find(target.origin->pops.begin(), target.origin->pops.end(), target.emigred);
        pop->size -= target.size;

        auto new_pop = std::find(target.target->pops.begin(), target.target->pops.end(), *pop);
        if(new_pop == target.target->pops.end()) {
            Pop i_pop(*pop);
            i_pop.size = target.size;
            target.target->pops.push_back(i_pop);
        } else {
            new_pop->size += target.size;
            new_pop->budget += target.emigred.budget;
        }
    }
    emigration.clear();

    // Chances of a coup increment for the global militancy
    for(auto& nation : world.nations) {
        // Nation must actually exist
        if(nation->exists() == false) continue;

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
            for(const auto& pop : province->pops) {
                // TODO: Ok, look, the justification is that educated people
                // almost never do coups - in comparasion to uneducated
                // peseants, rich people don't need to protest!
                const float anger = (pop.militancy * pop.con) / std::max(pop.literacy, 1.f) / std::max(pop.life_needs_met, 0.f);
                total_anger += anger;

                for(const auto& ideology : world.ideologies) {
                    unsigned int idx = world.get_id(ideology);

                    // TODO: Some pops lack the ideology array for some reason i dont know
                    //ideology_anger[idx] += (pop.ideology_approval[idx] * anger) * (pop.size / 1000.f);
                }
            }
        }

        // Roll a dice! (more probability with more anger!)
        if(fmod(rand(), std::max(coup_chances, coup_chances - total_anger)) <= 1.f) {
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
        if(product == nullptr) continue;

        // Uncomment to see supply-demand
        //if(product->price_vel && product->price > 0.01f)
        //    print_info("%s; Supply: %zu, Demand: %zu, Price %.2f", product->good->name.c_str(), product->supply, product->demand, product->price);

        product->close_market();

        // Re-count worldwide supply
        product->supply = 0;
        for(const auto& province : world.provinces) {
            product->supply += province->stockpile[world.get_id(product)];
        }
        product->demand = 0;
    }
}
