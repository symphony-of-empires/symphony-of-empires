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
//      io_impl.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <string>

#include "unified_render/serializer.hpp"

#include "action.hpp"
#include "world.hpp"
#include "province.hpp"
#include "nation.hpp"
#include "product.hpp"
#include "good.hpp"
#include "pop.hpp"
#include "action.hpp"
#include "diplomacy.hpp"
#include "unified_render/print.hpp"

// TODO: Endianess compatibility
template<>
class Serializer<ActionType>: public SerializerMemcpy<ActionType> {};

template<>
class Serializer<Province*>: public SerializerReference<World, Province> {};
template<>
class Serializer<Nation*>: public SerializerReference<World, Nation> {};
template<>
class Serializer<Event*>: public SerializerReference<World, Event> {};
template<>
class Serializer<Treaty*>: public SerializerReference<World, Treaty> {};
template<>
class Serializer<Culture*>: public SerializerReference<World, Culture> {};
template<>
class Serializer<Good*>: public SerializerReference<World, Good> {};
template<>
class Serializer<Unit*>: public SerializerReference<World, Unit> {};
template<>
class Serializer<UnitType*>: public SerializerReference<World, UnitType> {};
template<>
class Serializer<UnitTrait*>: public SerializerReference<World, UnitTrait> {};
template<>
class Serializer<BuildingType*>: public SerializerReference<World, BuildingType> {};
template<>
class Serializer<Ideology*>: public SerializerReference<World, Ideology> {};
template<>
class Serializer<Technology*>: public SerializerReference<World, Technology> {};

template<>
class Serializer<PopGroup>: public SerializerMemcpy<PopGroup> {};

template<>
class Serializer<PopType*>: public SerializerReference<World, PopType> {};
template<>
class Serializer<Religion*>: public SerializerReference<World, Religion> {};
template<>
class Serializer<NationModifier*>: public SerializerReference<World, NationModifier> {};
template<>
class Serializer<TerrainType*>: public SerializerReference<World, TerrainType> {};

template<>
class Serializer<NationModifier> {
public:
    static inline void serialize(Archive& stream, const NationModifier* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->con_mod);
        ::serialize(stream, &obj->death_mod);
        ::serialize(stream, &obj->delivery_cost_mod);
        ::serialize(stream, &obj->everyday_needs_met_mod);
        ::serialize(stream, &obj->life_needs_met_mod);
        ::serialize(stream, &obj->literacy_learn_mod);
        ::serialize(stream, &obj->luxury_needs_met_mod);
        ::serialize(stream, &obj->militancy_mod);
        ::serialize(stream, &obj->reproduction_mod);
        ::serialize(stream, &obj->salary_paid_mod);
        ::serialize(stream, &obj->workers_needed_mod);
    }
    static inline void deserialize(Archive& stream, NationModifier* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->con_mod);
        ::deserialize(stream, &obj->death_mod);
        ::deserialize(stream, &obj->delivery_cost_mod);
        ::deserialize(stream, &obj->everyday_needs_met_mod);
        ::deserialize(stream, &obj->life_needs_met_mod);
        ::deserialize(stream, &obj->literacy_learn_mod);
        ::deserialize(stream, &obj->luxury_needs_met_mod);
        ::deserialize(stream, &obj->militancy_mod);
        ::deserialize(stream, &obj->reproduction_mod);
        ::deserialize(stream, &obj->salary_paid_mod);
        ::deserialize(stream, &obj->workers_needed_mod);
    }
};

template<>
class Serializer<NationRelation>: public SerializerMemcpy<NationRelation> {
public:
    static inline void serialize(Archive& stream, const NationRelation* obj) {
        ::serialize(stream, &obj->free_supplies);
        ::serialize(stream, &obj->has_alliance);
        ::serialize(stream, &obj->has_defensive_pact);
        ::serialize(stream, &obj->has_embargo);
        ::serialize(stream, &obj->has_embassy);
        ::serialize(stream, &obj->has_market_access);
        ::serialize(stream, &obj->has_military_access);
        ::serialize(stream, &obj->has_truce);
        ::serialize(stream, &obj->has_war);
        ::serialize(stream, &obj->interest);
        ::serialize(stream, &obj->relation);
    }
    static inline void deserialize(Archive& stream, NationRelation* obj) {
        ::deserialize(stream, &obj->free_supplies);
        ::deserialize(stream, &obj->has_alliance);
        ::deserialize(stream, &obj->has_defensive_pact);
        ::deserialize(stream, &obj->has_embargo);
        ::deserialize(stream, &obj->has_embassy);
        ::deserialize(stream, &obj->has_market_access);
        ::deserialize(stream, &obj->has_military_access);
        ::deserialize(stream, &obj->has_truce);
        ::deserialize(stream, &obj->has_war);
        ::deserialize(stream, &obj->interest);
        ::deserialize(stream, &obj->relation);
    }
};

template<>
class Serializer<enum AllowancePolicy>: public SerializerMemcpy<enum AllowancePolicy> {};
template<>
class Serializer<enum CensorshipPolicy>: public SerializerMemcpy<enum CensorshipPolicy> {};
template<>
class Serializer<enum AutoBuildPolicy>: public SerializerMemcpy<enum AutoBuildPolicy> {};
template<>
class Serializer<enum TreatmentPolicy>: public SerializerMemcpy<enum TreatmentPolicy> {};

template<>
class Serializer<Policies> {
public:
    static inline void serialize(Archive& stream, const Policies* obj) {
        ::serialize(stream, &obj->free_supplies);
        ::serialize(stream, &obj->immigration);
        ::serialize(stream, &obj->import_tax);
        ::serialize(stream, &obj->industry_tax);
        ::serialize(stream, &obj->legislative_parliament);
        ::serialize(stream, &obj->med_flat_tax);
        ::serialize(stream, &obj->men_labour);
        ::serialize(stream, &obj->men_suffrage);
        ::serialize(stream, &obj->migration);
        ::serialize(stream, &obj->military_spending);
        ::serialize(stream, &obj->national_id);
        ::serialize(stream, &obj->poor_flat_tax);
        ::serialize(stream, &obj->private_property);
        ::serialize(stream, &obj->public_education);
        ::serialize(stream, &obj->public_healthcare);
        ::serialize(stream, &obj->rich_flat_tax);
        ::serialize(stream, &obj->secular_education);
        ::serialize(stream, &obj->slavery);
        ::serialize(stream, &obj->social_security);
        ::serialize(stream, &obj->treatment);
        ::serialize(stream, &obj->women_labour);
        ::serialize(stream, &obj->women_suffrage);
        ::serialize(stream, &obj->min_wage);
        ::serialize(stream, &obj->min_sv_for_parliament);
    }
    static inline void deserialize(Archive& stream, Policies* obj) {
        ::deserialize(stream, &obj->free_supplies);
        ::deserialize(stream, &obj->immigration);
        ::deserialize(stream, &obj->import_tax);
        ::deserialize(stream, &obj->industry_tax);
        ::deserialize(stream, &obj->legislative_parliament);
        ::deserialize(stream, &obj->med_flat_tax);
        ::deserialize(stream, &obj->men_labour);
        ::deserialize(stream, &obj->men_suffrage);
        ::deserialize(stream, &obj->migration);
        ::deserialize(stream, &obj->military_spending);
        ::deserialize(stream, &obj->national_id);
        ::deserialize(stream, &obj->poor_flat_tax);
        ::deserialize(stream, &obj->private_property);
        ::deserialize(stream, &obj->public_education);
        ::deserialize(stream, &obj->public_healthcare);
        ::deserialize(stream, &obj->rich_flat_tax);
        ::deserialize(stream, &obj->secular_education);
        ::deserialize(stream, &obj->slavery);
        ::deserialize(stream, &obj->social_security);
        ::deserialize(stream, &obj->treatment);
        ::deserialize(stream, &obj->women_labour);
        ::deserialize(stream, &obj->women_suffrage);
        ::deserialize(stream, &obj->min_wage);
        ::deserialize(stream, &obj->min_sv_for_parliament);
    }
};

template<>
class Serializer<PopType> {
public:
    static inline void serialize(Archive& stream, const PopType* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->social_value);
        ::serialize(stream, &obj->group);
    }
    static inline void deserialize(Archive& stream, PopType* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->social_value);
        ::deserialize(stream, &obj->group);
    }
};

template<>
class Serializer<TerrainType> {
public:
    static inline void serialize(Archive& stream, const TerrainType* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->color);
        ::serialize(stream, &obj->movement_penalty);
        ::serialize(stream, &obj->is_water_body);
    }
    static inline void deserialize(Archive& stream, TerrainType* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->color);
        ::deserialize(stream, &obj->movement_penalty);
        ::deserialize(stream, &obj->is_water_body);
    }
};

template<>
class Serializer<Culture> {
public:
    static inline void serialize(Archive& stream, const Culture* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->adjective);
        ::serialize(stream, &obj->noun);
        ::serialize(stream, &obj->combo_form);
        ::serialize(stream, &obj->color);
    }
    static inline void deserialize(Archive& stream, Culture* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->adjective);
        ::deserialize(stream, &obj->noun);
        ::deserialize(stream, &obj->combo_form);
        ::deserialize(stream, &obj->color);
    }
};

template<>
class Serializer<Religion> {
public:
    static inline void serialize(Archive& stream, const Religion* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->color);
    }
    static inline void deserialize(Archive& stream, Religion* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->color);
    }
};

template<>
class Serializer<UnitTrait> {
public:
    static inline void serialize(Archive& stream, const UnitTrait* obj) {
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->defense_mod);
        ::serialize(stream, &obj->attack_mod);
        ::serialize(stream, &obj->supply_consumption_mod);
        ::serialize(stream, &obj->speed_mod);
    }
    static inline void deserialize(Archive& stream, UnitTrait* obj) {
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->defense_mod);
        ::deserialize(stream, &obj->attack_mod);
        ::deserialize(stream, &obj->supply_consumption_mod);
        ::deserialize(stream, &obj->speed_mod);
    }
};

template<>
class Serializer<Unit> {
public:
    static inline void serialize(Archive& stream, const Unit* obj) {
        ::serialize(stream, &obj->type);
        ::serialize(stream, &obj->size);
        ::serialize(stream, &obj->target);
        ::serialize(stream, &obj->province);
        ::serialize(stream, &obj->owner);
        ::serialize(stream, &obj->move_progress);
    }
    static inline void deserialize(Archive& stream, Unit* obj) {
        ::deserialize(stream, &obj->type);
        ::deserialize(stream, &obj->size);
        ::deserialize(stream, &obj->target);
        ::deserialize(stream, &obj->province);
        ::deserialize(stream, &obj->owner);
        ::deserialize(stream, &obj->move_progress);
    }
};

template<>
class Serializer<Pop> {
public:
    static inline void serialize(Archive& stream, const Pop* obj) {
        ::serialize(stream, &obj->size);
        ::serialize(stream, &obj->literacy);
        ::serialize(stream, &obj->militancy);
        ::serialize(stream, &obj->con);
        ::serialize(stream, &obj->budget);
        ::serialize(stream, &obj->life_needs_met);
        ::serialize(stream, &obj->everyday_needs_met);
        ::serialize(stream, &obj->luxury_needs_met);
        ::serialize(stream, &obj->type);
        ::serialize(stream, &obj->culture);
        ::serialize(stream, &obj->religion);
        ::serialize(stream, &obj->ideology_approval);
    }
    static inline void deserialize(Archive& stream, Pop* obj) {
        ::deserialize(stream, &obj->size);
        ::deserialize(stream, &obj->literacy);
        ::deserialize(stream, &obj->militancy);
        ::deserialize(stream, &obj->con);
        ::deserialize(stream, &obj->budget);
        ::deserialize(stream, &obj->life_needs_met);
        ::deserialize(stream, &obj->everyday_needs_met);
        ::deserialize(stream, &obj->luxury_needs_met);
        ::deserialize(stream, &obj->type);
        ::deserialize(stream, &obj->culture);
        ::deserialize(stream, &obj->religion);
        ::deserialize(stream, &obj->ideology_approval);
    }
};

template<>
class Serializer<Descision> {
public:
    static inline void serialize(Archive& stream, const Descision* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->do_descision_function);
        ::serialize(stream, &obj->effects);
    }
    static inline void deserialize(Archive& stream, Descision* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->do_descision_function);
        ::deserialize(stream, &obj->effects);
    }
};

template<>
class Serializer<Event> {
public:
    static inline void serialize(Archive& stream, const Event* obj) {
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->conditions_function);
        ::serialize(stream, &obj->do_event_function);
        ::serialize(stream, &obj->receivers);
        ::serialize(stream, &obj->descisions);
        ::serialize(stream, &obj->title);
        ::serialize(stream, &obj->text);
    }
    static inline void deserialize(Archive& stream, Event* obj) {
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->conditions_function);
        ::deserialize(stream, &obj->do_event_function);
        ::deserialize(stream, &obj->receivers);
        ::deserialize(stream, &obj->descisions);
        ::deserialize(stream, &obj->title);
        ::deserialize(stream, &obj->text);
    }
};

template<>
class Serializer<Tile> {
public:
    static inline void serialize(Archive& stream, const Tile* obj) {
        ::serialize(stream, &obj->province_id);
    }
    static inline void deserialize(Archive& stream, Tile* obj) {
        ::deserialize(stream, &obj->province_id);
    }
};

template<>
class Serializer<NationClientHint> {
public:
    static inline void serialize(Archive& stream, const NationClientHint* obj) {
        ::serialize(stream, &obj->color);
        ::serialize(stream, &obj->alt_name);
        ::serialize(stream, &obj->ideology);
    }
    static inline void deserialize(Archive& stream, NationClientHint* obj) {
        ::deserialize(stream, &obj->color);
        ::deserialize(stream, &obj->alt_name);
        ::deserialize(stream, &obj->ideology);
    }
};

template<>
class Serializer<Nation> {
public:
    static inline void serialize(Archive& stream, const Nation* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->relations);
        ::serialize(stream, &obj->puppet_master);
        ::serialize(stream, &obj->diplomacy_points);
        ::serialize(stream, &obj->prestige);

        ::serialize(stream, &obj->infamy);
        ::serialize(stream, &obj->military_score);
        ::serialize(stream, &obj->naval_score);
        ::serialize(stream, &obj->economy_score);

        ::serialize(stream, &obj->budget);

        ::serialize(stream, &obj->capital);
        ::serialize(stream, &obj->culture_discrim);
        ::serialize(stream, &obj->religion_discrim);
        ::serialize(stream, &obj->owned_provinces);
        ::serialize(stream, &obj->controlled_provinces);
        ::serialize(stream, &obj->current_policy);
        ::serialize(stream, &obj->diplomatic_timer);

        ::serialize(stream, &obj->inbox);
        ::serialize(stream, &obj->client_hints);
        ::serialize(stream, &obj->ideology);

        ::serialize(stream, &obj->research);
        ::serialize(stream, &obj->focus_tech);
    }
    static inline void deserialize(Archive& stream, Nation* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->relations);
        ::deserialize(stream, &obj->puppet_master);
        ::deserialize(stream, &obj->diplomacy_points);
        ::deserialize(stream, &obj->prestige);

        ::deserialize(stream, &obj->infamy);
        ::deserialize(stream, &obj->military_score);
        ::deserialize(stream, &obj->naval_score);
        ::deserialize(stream, &obj->economy_score);

        ::deserialize(stream, &obj->budget);

        ::deserialize(stream, &obj->capital);
        ::deserialize(stream, &obj->culture_discrim);
        ::deserialize(stream, &obj->religion_discrim);
        ::deserialize(stream, &obj->owned_provinces);
        ::deserialize(stream, &obj->controlled_provinces);
        ::deserialize(stream, &obj->current_policy);
        ::deserialize(stream, &obj->diplomatic_timer);

        ::deserialize(stream, &obj->inbox);
        ::deserialize(stream, &obj->client_hints);
        ::deserialize(stream, &obj->ideology);

        ::deserialize(stream, &obj->research);
        ::deserialize(stream, &obj->focus_tech);
    }
};

template<>
class Serializer<TreatyClauseType>: public SerializerMemcpy<TreatyClauseType> {};

template<>
class Serializer<enum TreatyApproval>: public SerializerMemcpy<enum TreatyApproval> {};

template<>
class Serializer<UnitType> {
public:
    static inline void serialize(Archive& stream, const UnitType* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->supply_consumption);
        ::serialize(stream, &obj->speed);
        ::serialize(stream, &obj->max_health);
        ::serialize(stream, &obj->defense);
        ::serialize(stream, &obj->attack);
        ::serialize(stream, &obj->build_time);
        ::serialize(stream, &obj->is_ground);
        ::serialize(stream, &obj->is_naval);
    }
    static inline void deserialize(Archive& stream, UnitType* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->supply_consumption);
        ::deserialize(stream, &obj->speed);
        ::deserialize(stream, &obj->max_health);
        ::deserialize(stream, &obj->defense);
        ::deserialize(stream, &obj->attack);
        ::deserialize(stream, &obj->build_time);
        ::deserialize(stream, &obj->is_ground);
        ::deserialize(stream, &obj->is_naval);
    }
};

template<>
class Serializer<Province> {
public:
    static inline void serialize(Archive& stream, const Province* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->color);
        ::serialize(stream, &obj->n_tiles);
        ::serialize(stream, &obj->max_x);
        ::serialize(stream, &obj->max_y);
        ::serialize(stream, &obj->min_x);
        ::serialize(stream, &obj->min_y);
        ::serialize(stream, &obj->supply_limit);
        ::serialize(stream, &obj->supply_rem);
        ::serialize(stream, &obj->owner);
        ::serialize(stream, &obj->nuclei);
        ::serialize(stream, &obj->neighbours);
        ::serialize(stream, &obj->products);
        ::serialize(stream, &obj->pops);
        ::serialize(stream, &obj->buildings);
        ::serialize(stream, &obj->controller);
        ::serialize(stream, &obj->terrain_type);
    }
    static inline void deserialize(Archive& stream, Province* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->color);
        ::deserialize(stream, &obj->n_tiles);
        ::deserialize(stream, &obj->max_x);
        ::deserialize(stream, &obj->max_y);
        ::deserialize(stream, &obj->min_x);
        ::deserialize(stream, &obj->min_y);
        ::deserialize(stream, &obj->supply_limit);
        ::deserialize(stream, &obj->supply_rem);
        ::deserialize(stream, &obj->owner);
        ::deserialize(stream, &obj->nuclei);
        ::deserialize(stream, &obj->neighbours);
        ::deserialize(stream, &obj->products);
        ::deserialize(stream, &obj->pops);
        ::deserialize(stream, &obj->buildings);
        ::deserialize(stream, &obj->controller);
        ::deserialize(stream, &obj->terrain_type);
    }
};

template<>
class Serializer<BuildingType> {
public:
    static inline void serialize(Archive& stream, const BuildingType* obj) {
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->is_plot_on_land);
        ::serialize(stream, &obj->is_plot_on_sea);
        ::serialize(stream, &obj->is_build_land_units);
        ::serialize(stream, &obj->is_build_naval_units);
        ::serialize(stream, &obj->defense_bonus);
        ::serialize(stream, &obj->req_goods);
    }
    static inline void deserialize(Archive& stream, BuildingType* obj) {
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->is_plot_on_land);
        ::deserialize(stream, &obj->is_plot_on_sea);
        ::deserialize(stream, &obj->is_build_land_units);
        ::deserialize(stream, &obj->is_build_naval_units);
        ::deserialize(stream, &obj->defense_bonus);
        ::deserialize(stream, &obj->req_goods);
    }
};

template<>
class Serializer<Building> {
public:
    static inline void serialize(Archive& stream, const Building* obj) {
        ::serialize(stream, &obj->working_unit_type);
        ::serialize(stream, &obj->build_time);
        ::serialize(stream, &obj->budget);
        ::serialize(stream, &obj->days_unoperational);
        ::serialize(stream, &obj->production_cost);
        ::serialize(stream, &obj->stockpile);
        ::serialize(stream, &obj->req_goods);
    }
    static inline void deserialize(Archive& stream, Building* obj) {
        ::deserialize(stream, &obj->working_unit_type);
        ::deserialize(stream, &obj->build_time);
        ::deserialize(stream, &obj->budget);
        ::deserialize(stream, &obj->days_unoperational);
        ::deserialize(stream, &obj->production_cost);
        ::deserialize(stream, &obj->stockpile);
        ::deserialize(stream, &obj->req_goods);
    }
};

template<>
class Serializer<Product> {
public:
    static inline void serialize(Archive& stream, const Product* obj) {
        ::serialize(stream, &obj->price);
        ::serialize(stream, &obj->price_vel);
        ::serialize(stream, &obj->supply);
        ::serialize(stream, &obj->demand);
    }
    static inline void deserialize(Archive& stream, Product* obj) {
        ::deserialize(stream, &obj->price);
        ::deserialize(stream, &obj->price_vel);
        ::deserialize(stream, &obj->supply);
        ::deserialize(stream, &obj->demand);
    }
};

template<>
class Serializer<Technology> {
public:
    static inline void serialize(Archive& stream, const Technology* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->description);
        ::serialize(stream, &obj->cost);
        ::serialize(stream, &obj->req_technologies);
        ::serialize(stream, &obj->modifiers);
    }
    static inline void deserialize(Archive& stream, Technology* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->description);
        ::deserialize(stream, &obj->cost);
        ::deserialize(stream, &obj->req_technologies);
        ::deserialize(stream, &obj->modifiers);
    }
};

template<>
class Serializer<Good> {
public:
    static inline void serialize(Archive& stream, const Good* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
    }
    static inline void deserialize(Archive& stream, Good* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
    }
};

template<>
class Serializer<TreatyClause::BaseClause*> {
public:
    static inline void serialize(Archive& stream, const TreatyClause::BaseClause* const* obj) {
        ::serialize(stream, &(*obj)->type);
        switch((*obj)->type) {
        case TreatyClauseType::ANEXX_PROVINCES: {
            const auto dyn_clause = static_cast<const TreatyClause::AnexxProvince*>(*obj);
            if(dyn_clause == nullptr)
                throw SerializerException("Dynamic cast failed for TreatyClause::AnexxProvince");
            ::serialize(stream, &dyn_clause->provinces);
        } break;
        case TreatyClauseType::LIBERATE_NATION: {
            const auto dyn_clause = static_cast<const TreatyClause::LiberateNation*>(*obj);
            if(dyn_clause == nullptr)
                throw SerializerException("Dynamic cast failed for TreatyClause::LiberateNation");
            ::serialize(stream, &dyn_clause->provinces);
            ::serialize(stream, &dyn_clause->liberated);
        } break;
        case TreatyClauseType::IMPOSE_POLICIES: {
            const auto dyn_clause = static_cast<const TreatyClause::ImposePolicies*>(*obj);
            if(dyn_clause == nullptr)
                throw SerializerException("Dynamic cast failed for TreatyClause::ImposePolicies");
            ::serialize(stream, &dyn_clause->imposed);
        } break;
        case TreatyClauseType::MONEY: {
            const auto dyn_clause = static_cast<const TreatyClause::WarReparations*>(*obj);
            if(dyn_clause == nullptr)
                throw SerializerException("Dynamic cast failed for TreatyClause::WarReparations");
            ::serialize(stream, &dyn_clause->amount);
        } break;
        case TreatyClauseType::HUMILIATE: {
            const auto dyn_clause = static_cast<const TreatyClause::Humiliate*>(*obj);
            if(dyn_clause == nullptr)
                throw SerializerException("Dynamic cast failed for TreatyClause::Humiliate");
            ::serialize(stream, &dyn_clause->amount);
        } break;
        default:
            break;
        }

        // This statements must not be before the switch above, since the polymorphic
        // trans-serializer is not going to handle this well (see deserializer)
        ::serialize(stream, &(*obj)->sender);
        ::serialize(stream, &(*obj)->receiver);
        ::serialize(stream, &(*obj)->days_duration);
        ::serialize(stream, &(*obj)->done);
    }
    static inline void deserialize(Archive& stream, TreatyClause::BaseClause** obj) {
        *obj = new TreatyClause::BaseClause();

        // We will also need to destroy the original pointer and allocate a new
        // one to accomodate the polymorphic stuff, if we dont destroy-then-construct
        // we will have serious SIGSEGV
        ::deserialize(stream, &(*obj)->type);
        switch((*obj)->type) {
        case TreatyClauseType::ANEXX_PROVINCES: {
            delete* obj;
            *obj = new TreatyClause::AnexxProvince();
            auto dyn_clause = (TreatyClause::AnexxProvince*)*obj;
            ::deserialize(stream, &dyn_clause->provinces);
        } break;
        case TreatyClauseType::LIBERATE_NATION: {
            delete* obj;
            *obj = new TreatyClause::LiberateNation();
            auto dyn_clause = (TreatyClause::LiberateNation*)*obj;
            ::deserialize(stream, &dyn_clause->provinces);
            ::deserialize(stream, &dyn_clause->liberated);
        } break;
        case TreatyClauseType::IMPOSE_POLICIES: {
            delete* obj;
            *obj = new TreatyClause::ImposePolicies();
            auto dyn_clause = (TreatyClause::ImposePolicies*)*obj;
            ::deserialize(stream, &dyn_clause->imposed);
        } break;
        case TreatyClauseType::MONEY: {
            delete* obj;
            *obj = new TreatyClause::WarReparations();
            auto dyn_clause = (TreatyClause::WarReparations*)*obj;
            ::deserialize(stream, &dyn_clause->amount);
        } break;
        case TreatyClauseType::HUMILIATE: {
            delete* obj;
            *obj = new TreatyClause::Humiliate();
            auto dyn_clause = (TreatyClause::Humiliate*)*obj;
            ::deserialize(stream, &dyn_clause->amount);
        } break;
        case TreatyClauseType::CEASEFIRE: {
            delete* obj;
            *obj = new TreatyClause::Ceasefire();
            auto dyn_clause = (TreatyClause::Ceasefire*)*obj;
        } break;
        case TreatyClauseType::PUPPET: {
            delete* obj;
            *obj = new TreatyClause::Puppet();
            auto dyn_clause = (TreatyClause::Puppet*)*obj;
        } break;
        default:
            break;
        }

        ::deserialize(stream, &(*obj)->sender);
        ::deserialize(stream, &(*obj)->receiver);
        ::deserialize(stream, &(*obj)->days_duration);
        ::deserialize(stream, &(*obj)->done);
    }
};

template<>
class Serializer<Treaty> {
public:
    static inline void serialize(Archive& stream, const Treaty* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->receiver);
        ::serialize(stream, &obj->sender);
        ::serialize(stream, &obj->approval_status);
        ::serialize(stream, &obj->clauses);
    }
    static inline void deserialize(Archive& stream, Treaty* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->receiver);
        ::deserialize(stream, &obj->sender);
        ::deserialize(stream, &obj->approval_status);
        ::deserialize(stream, &obj->clauses);
    }
};

template<>
class Serializer<War> {
public:
    static inline void serialize(Archive& stream, const War* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->attackers);
        ::serialize(stream, &obj->defenders);
        ::serialize(stream, &obj->wargoals);
    }
    static inline void deserialize(Archive& stream, War* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->attackers);
        ::deserialize(stream, &obj->defenders);
        ::deserialize(stream, &obj->wargoals);
    }
};

template<>
class Serializer<Ideology> {
public:
    static inline void serialize(Archive& stream, const Ideology* obj) {
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->color);
        ::serialize(stream, &obj->check_policies_fn);
    }
    static inline void deserialize(Archive& stream, Ideology* obj) {
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->color);
        ::deserialize(stream, &obj->check_policies_fn);
    }
};

template<>
class Serializer<World> {
public:
    template<typename T>
    static inline typename T::Id deserialize_and_create_list(Archive& stream, World* obj) {
        typename T::Id n_elems;
        ::deserialize(stream, &n_elems);
        for(size_t i = 0; i < n_elems; i++) {
            T* sub_obj = new T();
            obj->insert(*sub_obj);
        }
        return n_elems;
    }

    static inline void serialize(Archive& stream, const World* obj) {
        ::serialize(stream, &obj->width);
        ::serialize(stream, &obj->height);
        ::serialize(stream, &obj->sea_level);
        ::serialize(stream, &obj->time);

        //for(size_t i = 0; i < obj->width * obj->height; i++) {
        //    ::serialize(stream, &obj->tiles[i]);
        //}

        const Good::Id n_goods = obj->goods.size();
        ::serialize(stream, &n_goods);
        const UnitType::Id n_unit_types = obj->unit_types.size();
        ::serialize(stream, &n_unit_types);
        const Religion::Id n_religions = obj->religions.size();
        ::serialize(stream, &n_religions);
        const Culture::Id n_cultures = obj->cultures.size();
        ::serialize(stream, &n_cultures);
        const PopType::Id n_pop_types = obj->pop_types.size();
        ::serialize(stream, &n_pop_types);
        const Nation::Id n_nations = obj->nations.size();
        ::serialize(stream, &n_nations);
        const Province::Id n_provinces = obj->provinces.size();
        ::serialize(stream, &n_provinces);
        const Event::Id n_events = obj->events.size();
        ::serialize(stream, &n_events);
        const BuildingType::Id n_building_types = obj->building_types.size();
        ::serialize(stream, &n_building_types);
        const Treaty::Id n_treaties = obj->treaties.size();
        ::serialize(stream, &n_treaties);
        const Ideology::Id n_ideologies = obj->ideologies.size();
        ::serialize(stream, &n_ideologies);
        const Technology::Id n_technologies = obj->technologies.size();
        ::serialize(stream, &n_technologies);
        const NationModifier::Id n_nation_modifiers = obj->nation_modifiers.size();
        ::serialize(stream, &n_nation_modifiers);
        const TerrainType::Id n_terrain_type = obj->terrain_types.size();
        ::serialize(stream, &n_terrain_type);
        const War::Id n_wars = obj->wars.size();
        ::serialize(stream, &n_wars);

        print_info("(SERIALIZER) World");
        print_info("  n_goods %zu", obj->goods.size());
        print_info("  n_unit_types %zu", obj->unit_types.size());
        print_info("  n_religions %zu", obj->religions.size());
        print_info("  n_cultures %zu", obj->cultures.size());
        print_info("  n_pop_types %zu", obj->pop_types.size());
        print_info("  n_nations %zu", obj->nations.size());
        print_info("  n_provinces %zu", obj->provinces.size());
        print_info("  n_events %zu", obj->events.size());
        print_info("  n_outpost_types %zu", obj->building_types.size());
        print_info("  n_treaties %zu", obj->treaties.size());
        print_info("  n_ideologies %zu", obj->ideologies.size());
        print_info("  n_terrain_types %zu", obj->terrain_types.size());
        print_info("  n_wars %zu", obj->wars.size());

        for(auto& sub_obj : obj->goods) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->unit_types) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->religions) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->cultures) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->pop_types) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->nations) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->provinces) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->events) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->building_types) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->treaties) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->ideologies) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->technologies) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->nation_modifiers) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->terrain_types) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj : obj->wars) {
            ::serialize(stream, sub_obj);
        }
    }

    static inline void deserialize(Archive& stream, World* obj) {
        ::deserialize(stream, &obj->width);
        ::deserialize(stream, &obj->height);
        ::deserialize(stream, &obj->sea_level);
        ::deserialize(stream, &obj->time);

        //obj->tiles = new Tile[obj->width * obj->height];
        //for(size_t i = 0; i < obj->width * obj->height; i++) {
        //    ::deserialize(stream, &obj->tiles[i]);
        //}

        // In order to avoid post-deserialization relational patcher,
        // we will simply allocate everything with "empty" objects,
        // then we will fill those spots as we deserialize
        Good::Id n_goods = deserialize_and_create_list<Good>(stream, obj);
        UnitType::Id n_unit_types = deserialize_and_create_list<UnitType>(stream, obj);
        Religion::Id n_religions = deserialize_and_create_list<Religion>(stream, obj);
        Culture::Id n_cultures = deserialize_and_create_list<Culture>(stream, obj);
        PopType::Id n_pop_types = deserialize_and_create_list<PopType>(stream, obj);
        Nation::Id n_nations = deserialize_and_create_list<Nation>(stream, obj);
        Province::Id n_provinces = deserialize_and_create_list<Province>(stream, obj);
        Event::Id n_events = deserialize_and_create_list<Event>(stream, obj);
        BuildingType::Id n_building_types = deserialize_and_create_list<BuildingType>(stream, obj);
        Treaty::Id n_treaties = deserialize_and_create_list<Treaty>(stream, obj);
        Ideology::Id n_ideologies = deserialize_and_create_list<Ideology>(stream, obj);
        Technology::Id n_technologies = deserialize_and_create_list<Technology>(stream, obj);
        NationModifier::Id n_nation_modifiers = deserialize_and_create_list<NationModifier>(stream, obj);
        TerrainType::Id n_terrain_types = deserialize_and_create_list<TerrainType>(stream, obj);
        War::Id n_wars = deserialize_and_create_list<War>(stream, obj);

        print_info("(DESERIALIZER) World");
        print_info("  n_goods %zu", obj->goods.size());
        print_info("  n_unit_types %zu", obj->unit_types.size());
        print_info("  n_religions %zu", obj->religions.size());
        print_info("  n_cultures %zu", obj->cultures.size());
        print_info("  n_pop_types %zu", obj->pop_types.size());
        print_info("  n_nations %zu", obj->nations.size());
        print_info("  n_provinces %zu", obj->provinces.size());
        print_info("  n_events %zu", obj->events.size());
        print_info("  n_outpost_types %zu", obj->building_types.size());
        print_info("  n_treaties %zu", obj->treaties.size());
        print_info("  n_ideologies %zu", obj->ideologies.size());
        print_info("  n_technologies %zu", obj->technologies.size());
        print_info("  n_terrain_types %zu", obj->terrain_types.size());
        print_info("  n_wars %zu", obj->wars.size());

        for(size_t i = 0; i < n_goods; i++) {
            auto* sub_obj = obj->goods[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_unit_types; i++) {
            auto* sub_obj = obj->unit_types[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_religions; i++) {
            auto* sub_obj = obj->religions[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_cultures; i++) {
            auto* sub_obj = obj->cultures[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_pop_types; i++) {
            auto* sub_obj = obj->pop_types[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_nations; i++) {
            auto* sub_obj = obj->nations[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_provinces; i++) {
            auto* sub_obj = obj->provinces[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_events; i++) {
            auto* sub_obj = obj->events[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_building_types; i++) {
            auto* sub_obj = obj->building_types[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_treaties; i++) {
            auto* sub_obj = obj->treaties[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_ideologies; i++) {
            auto* sub_obj = obj->ideologies[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_technologies; i++) {
            auto* sub_obj = obj->technologies[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_nation_modifiers; i++) {
            auto* sub_obj = obj->nation_modifiers[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_terrain_types; i++) {
            auto* sub_obj = obj->terrain_types[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_wars; i++) {
            auto* sub_obj = obj->wars[i];
            ::deserialize(stream, sub_obj);
        }
    }
};