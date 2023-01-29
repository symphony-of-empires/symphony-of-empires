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
//      indpobj.hpp
//
// Abstract:
//      Objects/classes that can be on it's own and that don't have many
//      particular relationships with each other
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <cstdint>
#include "eng3d/string.hpp"
#include "eng3d/color.hpp"
#include "objects.hpp"

struct Policies {
    // Economic
    float min_wage = 1.f;

    float industry_profit_tax = 0.1f;
    float burgeoise_tax = 0.1f;
    float pop_tax = 0.1f;

    float private_ownership = 1.f; // 0 - Ownership diallowed, ... - Max % ownable, 1 - Ownership allowed
    float foreign_ownership = 1.f;
    float collective_ownership = 1.f;
    float individual_ownership = 1.f;

    float admin_funding = 1.f; // 100% = 1 minimum wage paid to brueaucrats
    float military_funding = 1.f; // 100% = 1 minimum wage paid to soldiers

    float education_budget = 0.f; // State sponsored education
    float industry_subsidies_budget = 0.f; // Subsidized factories
    
    // Not controllable by the player (should be?)
    struct Economic {
        float distributism = 0.f;   // -1 = Concentrated
                                    // 0  = neutral
                                    // 1  = distributism
        
        float mercantilist = 0.f;   // -1 = Mercantile
                                    // 0  = neutral
                                    // 1  = Mercantile
        
        float capitalism = 0.f; // -1 = socialism
                                // 0  = neutral
                                // 1  = capitalist
    } economic;
    struct Political {
        float individualism = 0.f;  // -1 = Corporatism
                                    // 0  = neutral
                                    // 1  = individualist
        
        float state_power = 0.f;    // -1 = democracy
                                    // 0  = neutral
                                    // 1  = autocracy
        
        float equalitarianism = 0.f;    // -1 = elitism
                                        // 0  = neutral
                                        // 1  = egalitarian
        
        float secular = 0.f;    // -1 = non-secular
                                // 0  = neutral
                                // 1  = secular
        float pluralism = 0.f;  // -1 = not pluralist
                                // 0  = neutral
                                // 1  = plural
    } political;

    inline float difference(const Policies& rhs) const {
        auto diff = 0.f;
        diff += std::abs(rhs.min_wage - this->min_wage);
        diff += std::abs(rhs.industry_profit_tax - this->industry_profit_tax);
        diff += std::abs(rhs.burgeoise_tax - this->burgeoise_tax);
        diff += std::abs(rhs.pop_tax - this->pop_tax);
        diff += std::abs(rhs.private_ownership - this->private_ownership);
        diff += std::abs(rhs.foreign_ownership - this->foreign_ownership);
        diff += std::abs(rhs.collective_ownership - this->collective_ownership);
        diff += std::abs(rhs.individual_ownership - this->individual_ownership);
        diff += std::abs(rhs.education_budget - this->education_budget);
        diff += std::abs(rhs.industry_subsidies_budget - this->industry_subsidies_budget);
        return diff;
    }
};
template<>
struct Eng3D::Deser::Serializer<Policies::Economic> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Policies::Economic>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.distributism);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.mercantilist);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.capitalism);
    }
};
template<>
struct Eng3D::Deser::Serializer<Policies::Political> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Policies::Political>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.individualism);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.state_power);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.equalitarianism);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.secular);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.pluralism);
    }
};
template<>
struct Eng3D::Deser::Serializer<Policies> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Policies>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.min_wage);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.industry_profit_tax);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.burgeoise_tax);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.pop_tax);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.private_ownership);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.foreign_ownership);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.collective_ownership);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.individual_ownership);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.admin_funding);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.military_funding);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.education_budget);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.industry_subsidies_budget);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.economic);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.political);
    }
};

struct Ideology : RefnameEntity<IdeologyId> {
    std::uint32_t color;
    Eng3D::StringRef name;
    std::string get_icon_path() const {
        return string_format("gfx/ideology/%s.png", ref_name.c_str());
    }

    struct Subideology : RefnameEntity<SubideologyId> {
        Eng3D::StringRef name;
        std::uint32_t color;
        Policies::Economic economic;
        Policies::Political political;
    };
    std::vector<Subideology> subideologies;
};
template<>
struct Eng3D::Deser::Serializer<Ideology::Subideology> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Ideology::Subideology>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.economic);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.political);
    }
};

template<>
struct Eng3D::Deser::Serializer<Ideology> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Ideology>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.color);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.subideologies);
    }
};

struct Language : RefnameEntity<LanguageId> {
    std::uint32_t color;
    Eng3D::StringRef name;
    Eng3D::StringRef adjective;
    Eng3D::StringRef noun;
    Eng3D::StringRef combo_form;
};
template<>
struct Eng3D::Deser::Serializer<Language> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Language>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.adjective);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.noun);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.combo_form);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.color);
    }
};

struct Religion : RefnameEntity<ReligionId> {
    Eng3D::StringRef name;
    std::uint32_t color;
    std::string get_icon_path() const {
        return string_format("gfx/religion/%s.png", ref_name.c_str());
    }
};
template<>
struct Eng3D::Deser::Serializer<Religion> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Religion>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.color);
    }
};

enum class PopGroup : int {
    BURGEOISE = 0,
    ARTISAN = 1,
    BUREAUCRAT = 2,
    SOLDIER = 3,
    LABORER = 4,
    SLAVE = 5,
};
struct PopType : RefnameEntity<PopTypeId> {
    Eng3D::StringRef name;
    float social_value = 1.f;
    std::vector<float> basic_needs_amount; // Amount of commodities needed to satisfy basic needs
    std::vector<float> luxury_needs_satisfaction; // Amount of satisfaction each luxury commodity gives
    std::vector<float> luxury_needs_deminishing_factor; // Deminishing returns factor of the luxury commodity satisfaction
};
template<>
struct Eng3D::Deser::Serializer<PopType> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, PopType>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.social_value);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.basic_needs_amount);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.luxury_needs_satisfaction);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.luxury_needs_deminishing_factor);
    }
};

class Province;
class Pop : public Entity<PopId> {
    Pop& operator=(const Pop&) = default;
    friend class Province;
public:
    constexpr bool operator==(const Pop& rhs) const {
        return this->type_id == rhs.type_id;
    }

    PopTypeId type_id;
    float size = 1.f;
    float life_needs_met = 0.f;
    float literacy = 0.f;
    float militancy = 0.f;
    float budget = 0.f;
    float private_debt = 0.f;
    float public_debt = 0.f;
    Policies wanted_policies;
};
template<>
struct Eng3D::Deser::Serializer<Pop> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Pop>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.type_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.size);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.life_needs_met);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.literacy);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.militancy);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.budget);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.private_debt);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.public_debt);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.wanted_policies);
    }
};

enum TechnologyType {
    STRATEGIC = 0x01,
    MILITARY = 0x02,
    NAVY = 0x04,
    SOCIAL = 0x08,
    ECONOMIC = 0x10,
    POLITICS = 0x20,
};
struct Technology : public RefnameEntity<TechnologyId> {
    Eng3D::StringRef name;
    Eng3D::StringRef description;
    float cost;
    enum TechnologyType type;
    std::vector<TechnologyId> req_technologies;
};
template<>
struct Eng3D::Deser::Serializer<Technology> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Technology>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.description);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cost);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_technologies);
    }
};

struct TerrainType : RefnameEntity<TerrainTypeId> {
    Eng3D::StringRef name;
    uint32_t color; // Associative color (with bitmap)
    bool is_water_body = false; // Should this terrain be treated as sea?
    float penalty = 1.f; // Penalty imposed to various contexts
    std::string get_icon_path() const {
        return string_format("gfx/terraintype/%s.png", ref_name.c_str());
    }
};
template<>
struct Eng3D::Deser::Serializer<TerrainType> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, TerrainType>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.color);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.penalty);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.is_water_body);
    }
};
