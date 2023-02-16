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
//      world.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <mutex>
#include <string>
#include <memory>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "eng3d/entity.hpp"
#include "eng3d/profiler.hpp"
#include "eng3d/string.hpp"
#include "eng3d/log.hpp"
#include "eng3d/luavm.hpp"
#include "eng3d/color.hpp"

struct CommodityId : EntityId<uint8_t> {
    CommodityId() = default;
    CommodityId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<CommodityId> : Eng3D::Deser::SerializerMemcpy<CommodityId> {};
struct ProductId : EntityId<uint8_t> {
    ProductId() = default;
    ProductId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<ProductId> : Eng3D::Deser::SerializerMemcpy<ProductId> {};
struct LanguageId : EntityId<uint16_t> {
    LanguageId() = default;
    LanguageId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<LanguageId> : Eng3D::Deser::SerializerMemcpy<LanguageId> {};
struct ReligionId : EntityId<uint8_t> {
    ReligionId() = default;
    ReligionId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<ReligionId> : Eng3D::Deser::SerializerMemcpy<ReligionId> {};
struct PopTypeId : EntityId<uint8_t> {
    PopTypeId() = default;
    PopTypeId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<PopTypeId> : Eng3D::Deser::SerializerMemcpy<PopTypeId> {};
struct PopId : EntityId<uint8_t> {
    PopId() = default;
    PopId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<PopId> : Eng3D::Deser::SerializerMemcpy<PopId> {};
struct BuildingTypeId : EntityId<uint8_t> {
    BuildingTypeId() = default;
    BuildingTypeId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<BuildingTypeId> : Eng3D::Deser::SerializerMemcpy<BuildingTypeId> {};
struct WarId : EntityId<uint8_t> {
    WarId() = default;
    WarId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<WarId> : Eng3D::Deser::SerializerMemcpy<WarId> {};
struct DecisionId : EntityId<uint8_t> {
    DecisionId() = default;
    DecisionId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<DecisionId> : Eng3D::Deser::SerializerMemcpy<DecisionId> {};
struct EventId : EntityId<uint16_t> {
    EventId() = default;
    EventId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<EventId> : Eng3D::Deser::SerializerMemcpy<EventId> {};
struct NationId : EntityId<uint16_t> {
    NationId() = default;
    NationId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<NationId> : Eng3D::Deser::SerializerMemcpy<NationId> {};
struct ProvinceId : EntityId<uint16_t> {
    ProvinceId() = default;
    ProvinceId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<ProvinceId> : Eng3D::Deser::SerializerMemcpy<ProvinceId> {};
struct TechnologyId : EntityId<uint8_t> {
    TechnologyId() = default;
    TechnologyId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<TechnologyId> : Eng3D::Deser::SerializerMemcpy<TechnologyId> {};
struct TerrainTypeId : EntityId<uint8_t> {
    TerrainTypeId() = default;
    TerrainTypeId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<TerrainTypeId> : Eng3D::Deser::SerializerMemcpy<TerrainTypeId> {};
struct TreatyId : EntityId<uint16_t> {
    TreatyId() = default;
    TreatyId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<TreatyId> : Eng3D::Deser::SerializerMemcpy<TreatyId> {};
struct UnitTypeId : EntityId<uint8_t> {
    UnitTypeId() = default;
    UnitTypeId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<UnitTypeId> : Eng3D::Deser::SerializerMemcpy<UnitTypeId> {};
struct UnitId : EntityId<uint16_t> {
    UnitId() = default;
    UnitId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<UnitId> : Eng3D::Deser::SerializerMemcpy<UnitId> {};
struct IdeologyId : EntityId<uint8_t> {
    IdeologyId() = default;
    IdeologyId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<IdeologyId> : Eng3D::Deser::SerializerMemcpy<IdeologyId> {};
struct SubideologyId : EntityId<uint8_t> {
    SubideologyId() = default;
    SubideologyId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<SubideologyId> : Eng3D::Deser::SerializerMemcpy<SubideologyId> {};
struct BuildingId : EntityId<uint8_t> {
    BuildingId() = default;
    BuildingId(size_t _id) : EntityId(_id) {}
};
template<>
struct Eng3D::Deser::Serializer<BuildingId> : Eng3D::Deser::SerializerMemcpy<BuildingId> {};
class World;

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

    float difference(const Policies& rhs) const noexcept {
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
    std::string get_icon_path() const noexcept {
        return string_format("gfx/ideology/%s.png", ref_name.data());
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
    std::string get_icon_path() const noexcept {
        return string_format("gfx/religion/%s.png", ref_name.data());
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
    constexpr bool operator==(const Pop& rhs) const noexcept {
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
    std::string get_icon_path() const noexcept {
        return string_format("gfx/terraintype/%s.png", ref_name.data());
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

struct Decision : RefnameEntity<DecisionId> {
    Eng3D::StringRef name;
    Eng3D::StringRef effects;
    Eng3D::StringRef extra_data; // Extra data for the decision
    /// @todo Reload these when serializing
    int do_decision_function = 0;
};
template<>
struct Eng3D::Deser::Serializer<Decision> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Decision>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.effects);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.extra_data);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.do_decision_function);
    }
};

class Nation;
struct Event : RefnameEntity<EventId> {
    void take_decision(Nation& sender, Decision& dec);
    Eng3D::StringRef name;
    std::vector<NationId> receiver_ids;
    std::vector<Decision> decisions;
    Eng3D::StringRef text;
    Eng3D::StringRef title;
    Eng3D::StringRef extra_data; // Extra data for the event
    bool checked = false;
    /// @todo Reload these when serializing
    int conditions_function = 0;
    int do_event_function = 0;
};
template<>
struct Eng3D::Deser::Serializer<::Event> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, ::Event>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.receiver_ids);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.decisions);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.text);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.title);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.extra_data);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.checked);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.conditions_function);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.do_event_function);
    }
};

class Province;
struct Technology;
class Pop;
namespace TreatyClause {
    class BaseClause;
}
struct Technology;
class Nation : public RefnameEntity<NationId> {
    void do_diplomacy() {
        /// @todo Fix this formula which is currently broken
        //diplomatic_timer = glm::max((60 * 48) - glm::min(10.f * 48.f, prestige / 100.f), 4.f);
        diplomatic_timer = 48;
    }

    bool can_do_diplomacy() const {
        return diplomatic_timer == 0;
    }
    Nation& operator=(const Nation&) = default;
public:
    /// @brief Diplomatic relations between two nations
    struct Relation {
        bool is_allied() const noexcept {
            return alliance > 0.f && !has_war;
        }

        bool is_customs_union() const noexcept {
            return alliance > 0.5f;
        }

        bool has_landpass() const noexcept {
            return relation > 0.5f || alliance > 0.f || has_war;
        }

        float relation = 0.f;
        bool has_war = false;
        float alliance = 0.f; // From 0 to 1; 0 = diplomatic alliance, tariiff excemption
                              // 1 = political alliance, after this they can form a single country
    };

    /// @brief Hints for the client on how to display the nation
    struct ClientHint {
        uint32_t color;
        Eng3D::StringRef name;
        IdeologyId ideology_id;
    };

    void declare_war(Nation& nation, std::vector<TreatyClause::BaseClause*> clauses = std::vector<TreatyClause::BaseClause*>());
    bool is_ally(const Nation& nation) const;
    bool is_enemy(const Nation& nation) const;
    void auto_relocate_capital();
    void set_policy(const Policies& policies);
    bool is_accepted_language(const Language& language) const;
    bool is_accepted_religion(const Religion& relgion) const;
    float get_tax(const Pop& pop) const;
    void give_province(Province& province);
    void control_province(Province& province);
    const Nation::ClientHint& get_client_hint() const;
    float get_research_points() const;
    bool can_research(const Technology& tech) const;
    void change_research_focus(const Technology& tech);
    void get_allies(std::function<void(const Nation&)> fn) const;

    /// @brief Whetever the nation exists at all - we cannot add nations in-game so we just check
    /// if the nation "exists" at all, this means that it has a presence and a goverment
    /// must own atleast 1 province
    bool exists() const noexcept {
        return !(controlled_provinces.empty());
    }

    const Ideology::Subideology& get_subideology() const;

    // Policies
    bool can_directly_control_factories() const;

    void make_puppet(const Nation& master) noexcept {
        this->is_puppeted = true;
        this->puppet_master_id = master.get_id();
    }

    bool is_puppeted_by(const Nation& master) const noexcept {
        return this->is_puppeted && this->puppet_master_id == master.get_id();
    }

    Eng3D::StringRef name;
    float prestige = 0.1f; // Amount of prestige
    // Total budget of the nation (money in ark), this is not equal to GDP, the GDP is the total sum of the price
    // of all products in the nation, which are volatile unless they are sold
    float budget = 100'000.f;
    // Public loans from national banks for example
    float public_loan_pool = 0.f;
    float public_loan_interest = 0.f;
    // Default and can be disabled by the player
    bool ai_controlled = true;
    bool ai_do_cmd_troops = true;

    bool is_puppeted = false;
    NationId puppet_master_id; // Pupeeter of this nation (if any)
    ProvinceId capital_id; // The capital of this nation (can be invalid id)
    IdeologyId ideology_id; // Current ideology of the nation
    SubideologyId subideology_id; // Current subideology
    TechnologyId focus_tech_id; // Current tech being researched
    Policies current_policy; // Current policy of this nation
    std::vector<float> commodity_production; // Commodity production for each commodity
    uint16_t diplomatic_timer; // Time until a diplomacy can be done
    // Accepted languages in this nation, the accepted languages may have some bonuses on provinces *totally*
    // owned by this nation
    std::vector<float> language_acceptance;
    std::vector<float> religion_discrim;
    // List of provinces which are owned by this nation (including partial ownership)
    std::vector<ProvinceId> owned_provinces;
    std::vector<ProvinceId> controlled_provinces;
    std::deque<Event> inbox; // Inbox of the nation; events that require our attention / should be processed
    std::vector<float> research; // Progress on technologies (1:1)
    std::vector<Nation::ClientHint> client_hints; // Hints for the client on how to draw a nation on the client
    std::unordered_map<std::string, float> flags; // Flags that can be manipulated by events
    std::string client_username; // Used by clients to store usernames from nations - not saved
};
template<>
struct Eng3D::Deser::Serializer<Nation::Relation> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Nation::Relation>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.alliance);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.has_war);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.relation);
    }
};
template<>
struct Eng3D::Deser::Serializer<Nation::ClientHint> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Nation::ClientHint>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.color);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ideology_id);
    }
};
template<>
struct Eng3D::Deser::Serializer<Nation> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Nation>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.is_puppeted);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.puppet_master_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.prestige);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.budget);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.capital_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.commodity_production);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.language_acceptance);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.religion_discrim);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.owned_provinces);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.controlled_provinces);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.current_policy);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.diplomatic_timer);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.inbox);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.client_hints);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ideology_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.subideology_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.research);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.focus_tech_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.flags);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.public_loan_pool);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.public_loan_interest);
    }
};

/// @brief A commodity, mostly serves as a "product type"
struct Commodity : RefnameEntity<CommodityId> {
    Eng3D::StringRef name;
    std::string get_icon_path() const {
        return string_format("gfx/commodity/%s.png", ref_name.data());
    }
};
template<>
struct Eng3D::Deser::Serializer<Commodity> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Commodity>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
    }
};

/// @brief A product (based off a Commodity) which can be bought by POPs, converted by factories and transported
struct Product : Entity<ProductId> {
    static constexpr float min_price = 0.01f;
    static constexpr float max_price = 999'999.99f;

    float get_price_delta() const noexcept {
        constexpr float min_delta = 0.9f; // -10%
        constexpr float max_delta = 1.1; // +10%
        if(demand == 0.f && supply == 0.f)
            return min_delta;
        if(produced == 0.f)
            return min_delta;
        // Produts actually brought, versus the sold amount
        return glm::clamp(bought / produced, min_delta, max_delta);
    }

    float sd_ratio() const noexcept {
        return demand <= 0.f ? supply : supply / demand;
    }

    float ds_ratio() const noexcept {
        return supply <= 0.f ? demand : demand / supply;
    }

    void close_market() noexcept {
        // Increase price with more demand
        this->price_delta = this->get_price_delta();
        assert(this->price_delta >= 0.f);
        // Set the new price
        this->price = glm::clamp(this->price * this->price_delta, min_price, max_price);
        if(glm::epsilonEqual(this->price, 0.f, glm::epsilon<float>()))
            this->price_delta = 0.f;
        
        this->supply -= this->bought;
        this->demand = this->produced = this->bought = 0.f;
    }

    /// @brief Buy a portion of the item
    /// @param wanted_amount Total wanted amount to buy (to add to demand)
    /// @param amount Amount actually bought
    /// @return float Total cost of purchase
    float buy(float wanted_amount, float& amount) noexcept {
        if(this->supply < this->bought) return 0.f;
        assert(amount >= 0.f && wanted_amount >= 0.f);

        // Increment demand (how much desire it is for a product)
        this->demand += wanted_amount;

        // Buy the product in question
        amount = glm::clamp(wanted_amount, 0.f, this->supply - this->bought);
        this->bought += amount;
        return this->price * amount;
    }

    /// @brief Produce the product
    /// @param amount Amount to produce
    /// @return float Total gains from production
    float produce(float amount) noexcept {
        assert(amount >= 0.f);
        this->produced += amount;
        this->supply += amount;
        return this->price * amount;
    }

    float price = 1.f;
    float price_delta = 0.f;
    float supply = 0.f;
    float demand = 0.f;
    float bought = 0.f;
    float produced = 0.f;
    float global_demand = 0.f;
};
template<>
struct Eng3D::Deser::Serializer<Product> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Product>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.price);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.price_delta);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.supply);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.demand);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.bought);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.produced);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.global_demand);
    }
};
struct Technology;
class Unit;
struct UnitType;

/// @brief Type for military outposts
struct BuildingType : RefnameEntity<BuildingTypeId> {
    bool can_plot_on_sea() const noexcept { return flags[0]; }
    bool can_plot_on_land() const noexcept { return flags[1]; }
    bool can_build_land_units() const noexcept { return flags[2]; }
    bool can_build_naval_units() const noexcept { return flags[3]; }
    bool can_build_air_units() const noexcept { return flags[4]; }

    /// @brief Can this building type build a military unit
    bool can_build_military() const noexcept {
        return can_build_land_units() || can_build_air_units() || can_build_naval_units();
    }

    void can_plot_on_sea(bool b) noexcept { flags[0] = b; }
    void can_plot_on_land(bool b) noexcept { flags[1] = b; }
    void can_build_land_units(bool b) noexcept { flags[2] = b; }
    void can_build_naval_units(bool b) noexcept { flags[3] = b; }
    void can_build_air_units(bool b) noexcept { flags[4] = b; }

    Eng3D::StringRef name;
    std::bitset<4> flags;
    // We used to calculate these per each economical tick but now we can just store them
    // and multiply it by the level of the industry - this is the **minimum** amount of employed
    // people we should have at a time
    float num_req_workers = 0.f;
    std::optional<CommodityId> output_id; // Commodity that this building creates
    std::vector<CommodityId> input_ids; // Goods required to create output
    // Required commodities, first describes the id of the commodity and the second describes how many
    std::vector<std::pair<CommodityId, float>> req_goods;
    std::vector<TechnologyId> req_technologies; // Required technologies to build
};
template<>
struct Eng3D::Deser::Serializer<BuildingType> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, BuildingType>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.flags);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.input_ids);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.output_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_goods);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_technologies);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.num_req_workers);
    }
};

class Province;
/// @brief A military outpost, on land serves as a "spawn" place for units
/// When adjacent to a water tile this serves as a shipyard for spawning naval units
struct Building : Entity<BuildingId> {
    bool can_do_output(const Province& province, const std::vector<CommodityId>& inputs) const;

    bool can_build_unit() const noexcept {
        for(const auto& [k, v] : req_goods_for_unit)
            if(v)
                return false;
        return this->level > 0.f;
    }

    float get_upgrade_cost() const noexcept {
        constexpr auto base_cost = 1000.f;
        return base_cost;
    }

    float get_profit() const noexcept {
        return this->revenue.get_total() - this->expenses.get_total();
    }

    float get_operating_ratio() const noexcept {
        const auto total_revenue = this->revenue.get_total();
        const auto total_expenses = this->expenses.get_total();
        assert(total_revenue >= 0.f && total_expenses >= 0.f);
        if(total_expenses == 0.f) return 1.1f;
        assert(total_expenses >= 0.f && total_revenue >= 0.f);
        return total_revenue / total_expenses;
    }

    static constexpr auto industry_production_rate = 2.3f;
    float get_output_amount() const noexcept {
        return this->production_scale * this->workers * industry_production_rate;
    }

    float get_max_output_amount(float max_workers) const noexcept {
        return this->level * max_workers * industry_production_rate;
    }

    void work_on_unit(const UnitType& unit_type);

    struct Investment {
        float total = 0.f;
        float today_funds = 0.f;

        void invest(float amount) noexcept {
            total += amount;
            today_funds += amount;
        }

        float get_ownership(float total_investments) const noexcept {
            if(total == 0.f || total_investments == 0.f) return 0.f;
            return total_investments / total;
        }

        float get_dividends(float profit, float ownership) const noexcept {
            assert(ownership >= 0.f && ownership <= 1.f);
            return profit * ownership;
        }
    };
    Investment estate_private;
    Investment estate_state;
    Investment estate_collective;
    Investment estate_individual;
    std::vector<Investment> estate_foreign;
    float get_total_investment() const noexcept {
        auto sum = estate_private.total;
        sum += estate_state.total;
        sum += estate_collective.total;
        sum += estate_individual.total;
        for(const auto& foreign : estate_foreign)
            sum += foreign.total;
        return sum;
    }

    float budget = 0.f; // Total money that the industry has
    float level = 0.f; // Level/Capacity scale of the building
    float workers = 1.f; // Amount of workers
    float production_scale = 1.f; // How much of the industry is being used. From 0-1
    std::optional<UnitTypeId> working_unit_type_id; // Unit that is currently being built here (nullptr indicates no unit)
    // Required commodities for building the working unit
    // change this to a struct instead of a pair for readablity
    std::vector<std::pair<CommodityId, float>> req_goods_for_unit;
    // Required commodities for construction or for repairs
    std::vector<std::pair<CommodityId, float>> req_goods;

    // Bookkeeping
    struct {
        float outputs = 0.f;
        float get_total() const noexcept {
            return outputs;
        }
    } revenue;
    struct {
        float wages = 0.f;
        float inputs_cost = 0.f;
        float state_taxes = 0.f;
        float state_dividends = 0.f;
        float pop_dividends = 0.f;
        float private_dividends = 0.f;

        float get_dividends() const noexcept {
            return state_dividends + private_dividends + pop_dividends;
        }

        float get_total() const noexcept {
            return wages + inputs_cost + state_taxes + get_dividends();
        }
    } expenses;
};
template<>
struct Eng3D::Deser::Serializer<Building::Investment> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Building::Investment>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.total);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.today_funds);
    }
};

template<>
struct Eng3D::Deser::Serializer<Building> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Building>::type;

    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.estate_private);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.estate_state);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.estate_collective);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.estate_individual);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.estate_foreign);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.budget);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.level);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.production_scale);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.workers);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_goods);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.req_goods_for_unit);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.revenue.outputs);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.wages);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.inputs_cost);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.state_taxes);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.state_dividends);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.pop_dividends);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.expenses.private_dividends);
    }
};

/// @brief A single province, which is used to simulate economy in a "bulk-tiles" way
/// instead of doing economical operations on every single tile
class Province : public RefnameEntity<ProvinceId> {
    Province& operator=(const Province&) = delete;
public:
    float total_pops() const noexcept {
        auto total = 0.f;
        for(const auto& pop : pops)
            total += pop.size;
        return total;
    }

    float average_life_needs() const noexcept {
        auto total = 0.f;
        for(const auto& pop : pops)
            total += pop.life_needs_met;
        return total / pops.size();
    }

    float average_militancy() const noexcept {
        auto total = 0.f;
        for(const auto& pop : pops)
            total += pop.militancy;
        return total / pops.size();
    }

    float get_attractiveness(const Pop& pop) const;
    void add_building(const BuildingType& building_type);
    void cancel_construction_project();
    bool is_neighbour(const Province& province) const;
    std::pair<float, float> borrow_loan(float amount, float& borrowed);

    glm::vec2 get_pos() const noexcept {
        return glm::vec2(box_area.left + ((box_area.right - box_area.left) / 2.f), box_area.top + ((box_area.bottom - box_area.top) / 2.f));
    }

    float euclidean_distance(const Province& other_province, glm::vec2 world_size, float radius) const noexcept {
        return Eng3D::euclidean_distance(this->get_pos(), other_province.get_pos(), world_size, radius);
    }

    const std::vector<Building>& get_buildings() const noexcept {
        return buildings;
    }

    std::vector<Building>& get_buildings() noexcept {
        return buildings;
    }

    bool is_populated() const noexcept {
        for(const auto& pop : pops)
            if(pop.size > 0.f)
                return true;
        return false;
    }

    void unpopulate() noexcept {
        for(auto& pop : pops)
            pop.size = 0.f;
    }

    Eng3D::StringRef name;
    std::uint32_t color = 0; // Color of the province, used for mapping the province's shape from the map_div.png file
    bool is_coastal = false;
    float base_attractive = 0.f; // Attractiveness of province
    // Rectangle coordinates (x,y - x,y) for "area" scanning a province when needed
    // (for example, when changing owners)
    Eng3D::Rect box_area = Eng3D::Rect(0.f, 0.f, 0.f, 0.f);
    NationId owner_id; // The owner of this province
    NationId controller_id;
    TerrainTypeId terrain_type_id;
    std::vector<uint32_t> rgo_size; // How much of each rgo that can be extracted
    std::array<Pop, 7> pops;
    // Private company funded loans
    float private_loan_pool = 0.f;
    float private_loan_interest = 0.f;
    std::vector<Product> products;
    std::vector<Building> buildings;
    struct Battle {
        Battle() = default;
        float attacker_casualties = 0.f;
        float defender_casualties = 0.f;
        std::vector<UnitId> unit_ids; // Units inside the battle
        std::vector<NationId> attacker_nations_ids;
        std::vector<NationId> defender_nations_ids;
        bool active = false;

        std::vector<UnitId> get_attacker_unit_ids() const;
        std::vector<UnitId> get_defender_unit_ids() const;
    } battle;
    std::vector<NationId> nuclei; // Nations who have a nuclei in this province
    std::vector<ProvinceId> neighbour_ids; // Neighbouring provinces
    /// @brief Percentage of each languages from 0 to 1
    std::vector<float> languages;
    /// @brief Percentage of each religion prescence on the pops, from 0 to 1
    std::vector<float> religions;
};
template<>
struct Eng3D::Deser::Serializer<Province::Battle> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Province::Battle>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.attacker_casualties);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.defender_casualties);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.unit_ids);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.attacker_nations_ids);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.defender_nations_ids);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.active);
    }
};
template<>
struct Eng3D::Deser::Serializer<Province> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, Province>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.color);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.box_area);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.owner_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.nuclei);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.rgo_size);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.neighbour_ids);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.products);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.pops);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.private_loan_pool);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.private_loan_interest);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.buildings);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.controller_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.base_attractive);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.languages);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.religions);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.terrain_type_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.battle);
    }
};
class ProvinceManager {
    ProvinceManager& operator=(const ProvinceManager&) = default;
    std::vector<ProvinceId> recently_changed_owner;
    std::vector<ProvinceId> recently_changed_control;
    bool changed;
public:
    ProvinceManager() = default;

    void mark_province_owner_changed(ProvinceId province_id) noexcept {
        recently_changed_owner.push_back(province_id);
    }

    void mark_province_control_changed(ProvinceId province_id) noexcept {
        recently_changed_control.push_back(province_id);
    }

    void clear() noexcept {
        recently_changed_owner.clear();
        recently_changed_control.clear();
    }

    const std::vector<ProvinceId>& get_changed_owner_provinces() const noexcept {
        return recently_changed_owner;
    }

    const std::vector<ProvinceId>& get_changed_control_provinces() const noexcept {
        return recently_changed_control;
    }

    bool is_provinces_changed() const noexcept {
        return !recently_changed_owner.empty() || !recently_changed_control.empty();
    }
};

// Create a new list from a type, with helper functions
#define CONST_LIST_FOR_LOCAL_TYPE(type, list, list_type)\
    const list_type<type>& get_list(const type* = nullptr) const noexcept { return list; };\
    list_type<type>& get_list(const type* = nullptr) noexcept { return list; };\
    list_type<type> list;
#define LIST_FOR_LOCAL_TYPE(type, list, list_type)\
    const list_type<type>& get_list(const type* = nullptr) const noexcept { return list; };\
    list_type<type>& get_list(const type* = nullptr) noexcept { return list; };\
    list_type<type> list;

// Contains the main world class object, containing all the data relevant for the simulation
class World {
public:
    constexpr static unsigned int ticks_per_month = 30;

    World() = default;
    World& operator=(const World&) = delete;
    ~World() = default;
    static World& get_instance() {
        extern World g_world;
        return g_world;
    }
    void do_tick();
    void init_lua();
    void load_initial();
    void load_mod();
    void fire_special_event(const std::string_view event_ref_name, const std::string_view nation_ref_name, const std::string_view other_nation_ref_name);
    Eng3D::Profiler profiler;

    LIST_FOR_LOCAL_TYPE(Commodity, commodities, std::vector)
    LIST_FOR_LOCAL_TYPE(Language, languages, std::vector)
    LIST_FOR_LOCAL_TYPE(PopType, pop_types, std::vector)
    LIST_FOR_LOCAL_TYPE(UnitType, unit_types, std::vector)
    LIST_FOR_LOCAL_TYPE(BuildingType, building_types, std::vector)
    LIST_FOR_LOCAL_TYPE(Ideology, ideologies, std::vector)
    LIST_FOR_LOCAL_TYPE(Religion, religions, std::vector)
    LIST_FOR_LOCAL_TYPE(Technology, technologies, std::vector)
    LIST_FOR_LOCAL_TYPE(TerrainType, terrain_types, std::vector)
    CONST_LIST_FOR_LOCAL_TYPE(Province, provinces, std::vector)
    CONST_LIST_FOR_LOCAL_TYPE(Nation, nations, std::vector)
    LIST_FOR_LOCAL_TYPE(Event, events, std::vector)
    LIST_FOR_LOCAL_TYPE(Treaty, treaties, std::vector)
    UnitManager unit_manager;
    ProvinceManager province_manager;
    Economy::EconomyState economy_state;

    void insert(auto& obj) noexcept {
        auto& list = this->get_list(&obj);
        list_mutex.lock();
        obj.cached_id = list.size();
        assert(obj.cached_id < static_cast<std::remove_cvref_t<decltype(obj)>::Id>(-2));
        list.push_back(obj);
        list_mutex.unlock();
    }

    void remove(auto& obj) noexcept {
        const auto cached_id = static_cast<size_t>(this->get_id(obj));
        auto& list = this->get_list(&obj);
        list_mutex.lock();
        for(size_t i = cached_id + 1; i < list.size(); i++)
            list[i].cached_id = std::remove_cvref_t<decltype(obj)>::Id(static_cast<size_t>(list[i].cached_id) - 1);
        // Remove the element itself
        list.erase(list.begin() + cached_id);
        list_mutex.unlock();
    }

    /// @brief Get the id of an object, this is a template for all types except
    /// for tiles and locally-stored types (we can do this because we can obtain the list
    /// from the type) with get_list helper functions. Please do not store the value of this
    /// function on a local variable because said variable could potentially get invalidated!
    /// @tparam T Type of object
    /// @param obj Reference to the object
    /// @return TId Id of the object
    auto get_id(const auto& obj) const noexcept {
        return obj.cached_id;
    }

    /// @brief Get the distance from the equator in respect to Y
    /// @param y Position to get distance from
    /// @return auto Distance from the equator
    auto get_dist_from_equator(auto y) const noexcept {
        return std::fabs(std::fabs(y) - (this->width / 2.0));
    }

    std::vector<Nation::Relation> relations;

    Nation::Relation& get_relation(NationId a, NationId b) noexcept {
        assert(a != b);
        if(b > a) std::swap(a, b);
        return relations[a + b * nations.size()];
    }

    const Nation::Relation& get_relation(NationId a, NationId b) const noexcept {
        assert(a != b);
        if(b > a) std::swap(a, b);
        return relations[a + b * nations.size()];
    }

    int get_year() const noexcept {
        return this->time / this->ticks_per_month / 12;
    }

    int get_month() const noexcept {
        return (this->time / this->ticks_per_month) % 12;
    }

    int get_day() const noexcept {
        return this->time % this->ticks_per_month;
    }

    Eng3D::LuaVM lua;

    // 2D Array of tiles
    std::unique_ptr<ProvinceId[]> tiles;
    size_t width, height;
    int time;

    /// @brief Used to signal the lua scripts of invalid operations (eg. adding a country midgame)
    bool needs_to_sync = false;
    std::mutex world_mutex;
    std::mutex list_mutex;
    std::mutex inbox_mutex;
    std::vector<std::pair<Decision, NationId>> taken_decisions;
};
template<>
struct Eng3D::Deser::Serializer<World> {
    template<bool is_const>
    using type = typename Eng3D::Deser::CondConstType<is_const, World>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.width);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.height);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.time);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.commodities);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.unit_types);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.religions);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.languages);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.pop_types);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.terrain_types);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.building_types);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ideologies);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.technologies);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.nations);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.provinces);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.events);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.treaties);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.unit_manager);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.relations);

        // Savefiles do not contain the tiles
        /// @todo Handle dynamic tiles (provinces changing shape for ex.)
        bool has_tiles = obj.tiles.get() != nullptr;
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, has_tiles);
        if(has_tiles) {
            if constexpr(is_serialize) {
                // Serialize all tiles
                ar.expand(obj.width * obj.height * sizeof(ProvinceId));
                ar.copy_from(obj.tiles.get(), obj.width * obj.height * sizeof(ProvinceId));
            } else {
                // In order to avoid post-deserialization relational patcher, we will simply allocate everything with "empty" objects,
                // then we will fill those spots as we deserialize
                obj.tiles.reset(new ProvinceId[obj.width * obj.height]);
                // Deserialize all tiles
                ar.copy_to(obj.tiles.get(), obj.width * obj.height * sizeof(ProvinceId));
            }
        }
    }
};

extern World g_world;
