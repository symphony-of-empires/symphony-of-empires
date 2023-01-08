// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      server/emigration.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <algorithm>
#include <cstdio>
#include <cstddef>
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/combinable.h>

#include "eng3d/disc_dist.hpp"

#include "server/emigration.hpp"
#include "world.hpp"

// "Fuzzers" for emigration chances
static DiscreteDistribution<float> rng_multipliers({ 0.01f, 0.05f, 0.25f, 0.3f, 0.5f, 1.f }, { 1.f, 0.5f, 0.25f, 0.01f, 0.2f, 0.1f });

static inline void conlonial_migration(World& world);
static inline void internal_migration(World& world);
static inline void external_migration(World& world);

void do_emigration(World& world) {
    external_migration(world);
    internal_migration(world);
    conlonial_migration(world);
}

static inline void conlonial_migration(World&) {

}

static inline void internal_migration(World&) {

}

// Basic
static inline float nation_attraction(Nation& nation, Language& language) {
    const auto attraction = nation.language_acceptance[language];
    return attraction + rng_multipliers.get_item();
}

static inline float province_attraction(const Province& province) {
    auto rand_attractive = province.base_attractive + rng_multipliers.get_item();
    rand_attractive /= g_world.terrain_types[province.terrain_type_id].penalty;
    rand_attractive *= 1.f - province.average_militancy(); // from 0 to 1
    return rand_attractive;
}

static inline void external_migration(World& world) {
    std::vector<DiscreteDistribution<Province*>> province_distributions;
    province_distributions.reserve(world.provinces.size());
    for(auto& nation : world.nations) {
        std::vector<float> attractions;
        std::vector<Province*> viable_provinces;
        for(const auto province_id : nation.controlled_provinces) {
            auto& province = world.provinces[province_id];
            if(world.terrain_types[province.terrain_type_id].is_water_body)
                continue;
            auto attraction = province_attraction(province);
            if(attraction <= 0.f)
                continue;
            attractions.push_back(attraction);
            viable_provinces.push_back(&province);
        }
        if(viable_provinces.empty()) {
            attractions.push_back(1.f);
            viable_provinces.push_back(nullptr);
        }
        province_distributions.emplace_back(viable_provinces, attractions);
    }
    assert(!province_distributions.empty());

    std::vector<DiscreteDistribution<Nation*>> nation_distributions;
    nation_distributions.reserve(world.nations.size());
    for(auto& language : world.languages) {
        std::vector<float> attractions;
        std::vector<Nation*> viable_nations;
        for(auto& nation : world.nations) {
            auto attraction = nation_attraction(nation, language);
            if(attraction <= 0.f)
                continue;
            attractions.push_back(attraction);
            viable_nations.push_back(&nation);
        }
        if(viable_nations.empty()) {
            attractions.push_back(1.f);
            viable_nations.push_back(nullptr);
        }
        nation_distributions.emplace_back(viable_nations, attractions);
    }
    assert(!nation_distributions.empty());

    struct EmigrationData {
        ProvinceId origin_id;
        ProvinceId target_id;
        float size = 0.f;
        Pop emigred;
    };
    tbb::combinable<std::vector<EmigrationData>> emigration;
    tbb::parallel_for(tbb::blocked_range(world.provinces.begin(), world.provinces.end()), [&emigration, &nation_distributions, &province_distributions, &world](const auto& provinces_range) {
        for(auto& province : provinces_range) {
            if(world.terrain_types[province.terrain_type_id].is_water_body)
                continue;

            const auto language_id = std::distance(province.languages.begin(), std::max_element(province.languages.begin(), province.languages.end()));
            // Randomness factor to emulate a pseudo-imperfect economy
            for(auto& pop : province.pops) {
                if(pop.size <= 1.f)
                    continue;

                // Depending on how much not our life needs are being met is how many we
                // want to get out of here
                // And literacy determines "best" spot, for example a low literacy will
                // choose a slightly less desirable location
                const auto emigration_desire = glm::max(pop.militancy * -pop.life_needs_met, 1.f);
                auto emigrants = glm::min(pop.size * emigration_desire * rng_multipliers.get_item(), pop.size);
                if(emigrants > 0.f) {
                    auto& nation_distribution = nation_distributions[language_id];
                    const auto* random_nation = nation_distribution.get_item();
                    if(random_nation == nullptr)
                        continue;

                    auto& province_distribution = province_distributions[random_nation->get_id()];
                    auto* choosen_province = province_distribution.get_item();
                    if(choosen_province == nullptr || world.terrain_types[choosen_province->terrain_type_id].is_water_body)
                        continue;

                    emigration.local().push_back(EmigrationData{
                        province.get_id(),
                        choosen_province->get_id(),
                        emigrants,
                        pop
                    });

                    pop.size -= emigrants;
                    assert(!(pop.size < 0.f));
                }
            }
        }
    });

    // Emigrate pop to another province
    emigration.combine_each([&world](const auto& list) {
        for(const auto& e : list) {
            auto& target = world.provinces[e.target_id];
            const auto it = std::find(target.pops.begin(), target.pops.end(), e.emigred);
            assert(it != target.pops.end());
            it->size += e.size;
            //it->budget += e.emigred.budget;
        }
    });
}
