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
#ifdef E3D_TARGET_UNIX
#	include <sys/types.h>
#else
#if defined _MSC_VER
#	include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif
#endif
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

class Emigrated {
public:
    Emigrated(Pop& pop)
        : emigred{ pop }
    {

    }
    ~Emigrated() {}

    Province* origin;
    Province* target;
    Pop emigred;
    size_t size;
};

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
    if(!nation.exists())
        return -1.f;
    float attraction = nation.language_acceptance[language];
    return attraction + rng_multipliers.get_item();
}

static inline float province_attraction(const Province& province) {
    return province.base_attractive + rng_multipliers.get_item();
}

static inline void external_migration(World& world) {
    std::vector<DiscreteDistribution<Province*>> province_distributions;
    province_distributions.reserve(world.provinces.size());
    for(auto& nation : world.nations) {
        std::vector<float> attractions;
        std::vector<Province*> viable_provinces;
        for(const auto province_id : nation.owned_provinces) {
            auto& province = world.provinces[province_id];
            auto attraction = province_attraction(province);
            if(attraction <= 0.f) continue;
            attractions.push_back(attraction);
            viable_provinces.push_back(&province);
        }
        if(viable_provinces.empty()) {
            attractions.push_back(1.f);
            viable_provinces.push_back(nullptr);
        }
        province_distributions.emplace_back(viable_provinces, attractions);
    }

    std::vector<DiscreteDistribution<Nation*>> nation_distributions;
    nation_distributions.reserve(world.nations.size());
    for(auto& language : world.languages) {
        std::vector<float> attractions;
        std::vector<Nation*> viable_nations;
        for(auto& nation : world.nations) {
            if(!nation.exists()) continue;
            auto attraction = nation_attraction(nation, language);
            if(attraction <= 0.f) continue;
            attractions.push_back(attraction);
            viable_nations.push_back(&nation);
        }
        if(viable_nations.empty()) {
            attractions.push_back(1.f);
            viable_nations.push_back(nullptr);
        }
        nation_distributions.emplace_back(viable_nations, attractions);
    }

    // Collect list of nations that exist
    std::vector<Nation*> eval_nations;
    for(auto& nation : world.nations)
        if(nation.exists())
            eval_nations.push_back(&nation);

    tbb::combinable<std::vector<Emigrated>> emigration;
    tbb::parallel_for(tbb::blocked_range(eval_nations.begin(), eval_nations.end()), [&emigration, &nation_distributions, &province_distributions, &world](const auto& nations_range) {
        for(const auto& nation : nations_range) {
            for(const auto province_id : nation->controlled_provinces) {
                auto& province = world.provinces[province_id];
                const auto language_id = std::distance(province.languages.begin(), std::max_element(province.languages.begin(), province.languages.end()));
                // Randomness factor to emulate a pseudo-imperfect economy
                for(auto& pop : province.pops) {
                    // Depending on how much not our life needs are being met is how many we
                    // want to get out of here
                    // And literacy determines "best" spot, for example a low literacy will
                    // choose a slightly less desirable location
                    const auto emigration_desire = glm::max(pop.militancy * -pop.life_needs_met, 1.f);
                    const auto emigrants = glm::min(pop.size * emigration_desire * rng_multipliers.get_item(), pop.size);
                    if(emigrants > 0) {
                        auto& nation_distribution = nation_distributions[language_id];
                        const auto* random_nation = nation_distribution.get_item();
                        if(random_nation == nullptr) continue;

                        auto& province_distribution = province_distributions[random_nation->get_id()];
                        auto* choosen_province = province_distribution.get_item();
                        if(choosen_province == nullptr) continue;

                        Emigrated emigrated(pop);
                        emigrated.target = choosen_province;
                        emigrated.size = emigrants;
                        emigrated.origin = &province;
                        emigration.local().push_back(emigrated);
                        pop.size -= emigrants;
                    }
                }
            }
        }
    });

    // Now time to do the emigration - we will create a new POP on the province
    // if a POP with similar type does not exist - and we will also subtract the
    // amount of emigrated from the original POP to not
    // create clones
    emigration.combine_each([&world](auto& target_list) {
        for(auto& target : target_list) {
            auto new_pop = std::find(target.target->pops.begin(), target.target->pops.end(), target.emigred);
            if(new_pop == target.target->pops.end()) continue;

            const auto ratio = target.emigred.size / target.size;
            assert(target.target->languages.size() == target.origin->languages.size());
            for(size_t i = 0; i < target.target->languages.size(); i++)
                target.target->languages[i] += glm::clamp(target.origin->languages[i] * ratio, 0.f, 1.f);
            assert(target.target->religions.size() == target.origin->religions.size());
            for(size_t i = 0; i < target.target->religions.size(); i++)
                target.target->religions[i] += glm::clamp(target.origin->religions[i] * ratio, 0.f, 1.f);
            
            new_pop->size += target.size;
            new_pop->budget += target.emigred.budget;
        }
    });
}