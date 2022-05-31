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

#include "eng3d/serializer.hpp"

#include "action.hpp"
#include "world.hpp"
#include "province.hpp"
#include "nation.hpp"
#include "product.hpp"
#include "good.hpp"
#include "pop.hpp"
#include "action.hpp"
#include "diplomacy.hpp"

/// @todo Endianess compatibility
template<>
class Serializer<ActionType>: public SerializerMemcpy<ActionType> {};

// Global references
template<>
class Serializer<const Province*>: public SerializerReference<World, Province> {};
template<>
class Serializer<const Nation*>: public SerializerReference<World, Nation> {};
template<>
class Serializer<const Event*>: public SerializerReference<World, Event> {};
template<>
class Serializer<const Treaty*>: public SerializerReference<World, Treaty> {};
template<>
class Serializer<const Unit*>: public SerializerReference<World, Unit> {};

// Local references
template<>
class Serializer<const Culture*>: public SerializerReferenceLocal<World, Culture> {};
template<>
class Serializer<const Good*>: public SerializerReferenceLocal<World, Good> {};
template<>
class Serializer<const UnitType*>: public SerializerReferenceLocal<World, UnitType> {};
template<>
class Serializer<const UnitTrait*>: public SerializerReferenceLocal<World, UnitTrait> {};
template<>
class Serializer<const BuildingType*>: public SerializerReferenceLocal<World, BuildingType> {};
template<>
class Serializer<const Ideology*>: public SerializerReferenceLocal<World, Ideology> {};
template<>
class Serializer<const Technology*>: public SerializerReferenceLocal<World, Technology> {};
template<>
class Serializer<const PopType*>: public SerializerReferenceLocal<World, PopType> {};
template<>
class Serializer<const Religion*>: public SerializerReferenceLocal<World, Religion> {};
template<>
class Serializer<const NationModifier*>: public SerializerReferenceLocal<World, NationModifier> {};
template<>
class Serializer<const TerrainType*>: public SerializerReferenceLocal<World, TerrainType> {};

// Global references
template<>
class Serializer<Province*>: public SerializerReference<World, Province> {};
template<>
class Serializer<Nation*>: public SerializerReference<World, Nation> {};
template<>
class Serializer<Event*>: public SerializerReference<World, Event> {};
template<>
class Serializer<Treaty*>: public SerializerReference<World, Treaty> {};
template<>
class Serializer<Unit*>: public SerializerReference<World, Unit> {};

// Local references
template<>
class Serializer<Culture*>: public SerializerReferenceLocal<World, Culture> {};
template<>
class Serializer<Good*>: public SerializerReferenceLocal<World, Good> {};
template<>
class Serializer<UnitType*>: public SerializerReferenceLocal<World, UnitType> {};
template<>
class Serializer<UnitTrait*>: public SerializerReferenceLocal<World, UnitTrait> {};
template<>
class Serializer<BuildingType*>: public SerializerReferenceLocal<World, BuildingType> {};
template<>
class Serializer<Ideology*>: public SerializerReferenceLocal<World, Ideology> {};
template<>
class Serializer<Technology*>: public SerializerReferenceLocal<World, Technology> {};
template<>
class Serializer<PopType*>: public SerializerReferenceLocal<World, PopType> {};
template<>
class Serializer<Religion*>: public SerializerReferenceLocal<World, Religion> {};
template<>
class Serializer<NationModifier*>: public SerializerReferenceLocal<World, NationModifier> {};
template<>
class Serializer<TerrainType*>: public SerializerReferenceLocal<World, TerrainType> {};

// Other objects
template<>
class Serializer<PopGroup>: public SerializerMemcpy<PopGroup> {};

template<typename T>
class SerializerEntity : public Serializer<T> {};

template<>
class SerializerEntity<NationModifier> {
public:
    template<bool is_serialize, typename T>
    static inline void deser_dynamic(Archive& ar, T* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->death_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->delivery_cost_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->everyday_needs_met_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->life_needs_met_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->literacy_learn_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->luxury_needs_met_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->militancy_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->reproduction_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->salary_paid_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->workers_needed_mod);
    }
};

template<>
class SerializerEntity<NationRelation> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, NationRelation* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->has_alliance);
        ::deser_dynamic<is_serialize>(ar, &obj->has_defensive_pact);
        ::deser_dynamic<is_serialize>(ar, &obj->has_embargo);
        ::deser_dynamic<is_serialize>(ar, &obj->has_market_access);
        ::deser_dynamic<is_serialize>(ar, &obj->has_military_access);
        ::deser_dynamic<is_serialize>(ar, &obj->has_war);
        ::deser_dynamic<is_serialize>(ar, &obj->interest);
        ::deser_dynamic<is_serialize>(ar, &obj->relation);
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
class SerializerEntity<Policies> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Policies* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->free_supplies);
        ::deser_dynamic<is_serialize>(ar, &obj->immigration);
        ::deser_dynamic<is_serialize>(ar, &obj->import_tax);
        ::deser_dynamic<is_serialize>(ar, &obj->industry_tax);
        ::deser_dynamic<is_serialize>(ar, &obj->legislative_parliament);
        ::deser_dynamic<is_serialize>(ar, &obj->med_flat_tax);
        ::deser_dynamic<is_serialize>(ar, &obj->migration);
        ::deser_dynamic<is_serialize>(ar, &obj->military_spending);
        ::deser_dynamic<is_serialize>(ar, &obj->poor_flat_tax);
        ::deser_dynamic<is_serialize>(ar, &obj->private_property);
        ::deser_dynamic<is_serialize>(ar, &obj->public_education);
        ::deser_dynamic<is_serialize>(ar, &obj->public_healthcare);
        ::deser_dynamic<is_serialize>(ar, &obj->rich_flat_tax);
        ::deser_dynamic<is_serialize>(ar, &obj->secular_education);
        ::deser_dynamic<is_serialize>(ar, &obj->slavery);
        ::deser_dynamic<is_serialize>(ar, &obj->social_security);
        ::deser_dynamic<is_serialize>(ar, &obj->treatment);
        ::deser_dynamic<is_serialize>(ar, &obj->min_wage);
        ::deser_dynamic<is_serialize>(ar, &obj->min_sv_for_parliament);
    }
};

template<>
class SerializerEntity<PopType> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, PopType* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->social_value);
        ::deser_dynamic<is_serialize>(ar, &obj->group);
    }
};

template<>
class SerializerEntity<TerrainType> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, TerrainType* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->color);
        ::deser_dynamic<is_serialize>(ar, &obj->movement_penalty);
        ::deser_dynamic<is_serialize>(ar, &obj->is_water_body);
    }
};

template<>
class SerializerEntity<Culture> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Culture* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->adjective);
        ::deser_dynamic<is_serialize>(ar, &obj->noun);
        ::deser_dynamic<is_serialize>(ar, &obj->combo_form);
        ::deser_dynamic<is_serialize>(ar, &obj->color);
    }
};

template<>
class SerializerEntity<Religion> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Religion* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->color);
    }
};

template<>
class SerializerEntity<UnitTrait> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, UnitTrait* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->defense_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->attack_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->supply_consumption_mod);
        ::deser_dynamic<is_serialize>(ar, &obj->speed_mod);
    }
};

template<>
class SerializerEntity<Unit> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Unit* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->type);
        ::deser_dynamic<is_serialize>(ar, &obj->size);
        ::deser_dynamic<is_serialize>(ar, &obj->target);
        ::deser_dynamic<is_serialize>(ar, &obj->province);
        ::deser_dynamic<is_serialize>(ar, &obj->owner);
        ::deser_dynamic<is_serialize>(ar, &obj->move_progress);
    }
};

template<>
class SerializerEntity<Pop> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Pop* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->size);
        ::deser_dynamic<is_serialize>(ar, &obj->literacy);
        ::deser_dynamic<is_serialize>(ar, &obj->militancy);
        ::deser_dynamic<is_serialize>(ar, &obj->budget);
        ::deser_dynamic<is_serialize>(ar, &obj->life_needs_met);
        ::deser_dynamic<is_serialize>(ar, &obj->everyday_needs_met);
        ::deser_dynamic<is_serialize>(ar, &obj->luxury_needs_met);
        ::deser_dynamic<is_serialize>(ar, &obj->type);
        ::deser_dynamic<is_serialize>(ar, &obj->culture);
        ::deser_dynamic<is_serialize>(ar, &obj->religion);
        ::deser_dynamic<is_serialize>(ar, &obj->ideology_approval);
    }
};

template<>
class SerializerEntity<Decision> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Decision* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->do_decision_function);
        ::deser_dynamic<is_serialize>(ar, &obj->effects);
    }
};

template<>
class SerializerEntity<Event> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Event* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->conditions_function);
        ::deser_dynamic<is_serialize>(ar, &obj->do_event_function);
        ::deser_dynamic<is_serialize>(ar, &obj->receivers);
        ::deser_dynamic<is_serialize>(ar, &obj->decisions);
        ::deser_dynamic<is_serialize>(ar, &obj->title);
        ::deser_dynamic<is_serialize>(ar, &obj->text);
    }
};

template<>
class SerializerEntity<Tile> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Tile* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->province_id);
    }
};

template<>
class SerializerEntity<NationClientHint> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, NationClientHint* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->color);
        ::deser_dynamic<is_serialize>(ar, &obj->alt_name);
        ::deser_dynamic<is_serialize>(ar, &obj->ideology);
    }
};

template<>
class SerializerEntity<Nation> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Nation* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->puppet_master);
        ::deser_dynamic<is_serialize>(ar, &obj->diplomacy_points);
        ::deser_dynamic<is_serialize>(ar, &obj->prestige);
        ::deser_dynamic<is_serialize>(ar, &obj->infamy);
        ::deser_dynamic<is_serialize>(ar, &obj->military_score);
        ::deser_dynamic<is_serialize>(ar, &obj->naval_score);
        ::deser_dynamic<is_serialize>(ar, &obj->economy_score);
        ::deser_dynamic<is_serialize>(ar, &obj->budget);
        ::deser_dynamic<is_serialize>(ar, &obj->capital);
        ::deser_dynamic<is_serialize>(ar, &obj->culture_discrim);
        ::deser_dynamic<is_serialize>(ar, &obj->religion_discrim);
        ::deser_dynamic<is_serialize>(ar, &obj->owned_provinces);
        ::deser_dynamic<is_serialize>(ar, &obj->controlled_provinces);
        ::deser_dynamic<is_serialize>(ar, &obj->current_policy);
        ::deser_dynamic<is_serialize>(ar, &obj->diplomatic_timer);
        ::deser_dynamic<is_serialize>(ar, &obj->inbox);
        ::deser_dynamic<is_serialize>(ar, &obj->client_hints);
        ::deser_dynamic<is_serialize>(ar, &obj->ideology);
        ::deser_dynamic<is_serialize>(ar, &obj->research);
        ::deser_dynamic<is_serialize>(ar, &obj->focus_tech);
    }
};

template<>
class Serializer<TreatyClauseType>: public SerializerMemcpy<TreatyClauseType> {};

template<>
class Serializer<enum TreatyApproval>: public SerializerMemcpy<enum TreatyApproval> {};

template<>
class SerializerEntity<UnitType> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, UnitType* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->supply_consumption);
        ::deser_dynamic<is_serialize>(ar, &obj->speed);
        ::deser_dynamic<is_serialize>(ar, &obj->max_health);
        ::deser_dynamic<is_serialize>(ar, &obj->defense);
        ::deser_dynamic<is_serialize>(ar, &obj->attack);
        ::deser_dynamic<is_serialize>(ar, &obj->build_time);
        ::deser_dynamic<is_serialize>(ar, &obj->is_ground);
        ::deser_dynamic<is_serialize>(ar, &obj->is_naval);
    }
};

template<>
class SerializerEntity<Province> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Province* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->color);
        ::deser_dynamic<is_serialize>(ar, &obj->max_x);
        ::deser_dynamic<is_serialize>(ar, &obj->max_y);
        ::deser_dynamic<is_serialize>(ar, &obj->min_x);
        ::deser_dynamic<is_serialize>(ar, &obj->min_y);
        ::deser_dynamic<is_serialize>(ar, &obj->supply_limit);
        ::deser_dynamic<is_serialize>(ar, &obj->supply_rem);
        ::deser_dynamic<is_serialize>(ar, &obj->owner);
        ::deser_dynamic<is_serialize>(ar, &obj->nuclei);
        ::deser_dynamic<is_serialize>(ar, &obj->rgo_size);
        ::deser_dynamic<is_serialize>(ar, &obj->neighbours);
        ::deser_dynamic<is_serialize>(ar, &obj->products);
        ::deser_dynamic<is_serialize>(ar, &obj->pops);
        ::deser_dynamic<is_serialize>(ar, &obj->buildings);
        ::deser_dynamic<is_serialize>(ar, &obj->controller);
        ::deser_dynamic<is_serialize>(ar, &obj->terrain_type);
    }
};

template<>
class SerializerEntity<BuildingType> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, BuildingType* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->flags);
        ::deser_dynamic<is_serialize>(ar, &obj->inputs);
        ::deser_dynamic<is_serialize>(ar, &obj->output);
        ::deser_dynamic<is_serialize>(ar, &obj->req_goods);
        ::deser_dynamic<is_serialize>(ar, &obj->req_technologies);
        ::deser_dynamic<is_serialize>(ar, &obj->num_req_workers);
    }
};

template<>
class SerializerEntity<Building> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Building* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->working_unit_type);
        ::deser_dynamic<is_serialize>(ar, &obj->build_time);
        ::deser_dynamic<is_serialize>(ar, &obj->budget);
        ::deser_dynamic<is_serialize>(ar, &obj->days_unoperational);
        ::deser_dynamic<is_serialize>(ar, &obj->production_cost);
        ::deser_dynamic<is_serialize>(ar, &obj->stockpile);
        ::deser_dynamic<is_serialize>(ar, &obj->req_goods);
    }
};

template<>
class SerializerEntity<Product> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Product* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->price);
        ::deser_dynamic<is_serialize>(ar, &obj->price_vel);
        ::deser_dynamic<is_serialize>(ar, &obj->supply);
        ::deser_dynamic<is_serialize>(ar, &obj->demand);
    }
};

template<>
class SerializerEntity<Technology> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Technology* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->description);
        ::deser_dynamic<is_serialize>(ar, &obj->cost);
        ::deser_dynamic<is_serialize>(ar, &obj->req_technologies);
        ::deser_dynamic<is_serialize>(ar, &obj->modifiers);
    }
};

template<>
class SerializerEntity<Good> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Good* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
    }
};

template<>
class SerializerEntity<TreatyClause::BaseClause*> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, TreatyClause::BaseClause** obj) {
        if constexpr(is_serialize) {
            ::serialize(ar, &(*obj)->type);
            switch((*obj)->type) {
            case TreatyClauseType::ANNEX_PROVINCES: {
                const auto dyn_clause = static_cast<const TreatyClause::AnnexProvince*>(*obj);
                if(dyn_clause == nullptr)
                    throw SerializerException("Dynamic cast failed for TreatyClause::AnnexProvince");
                ::serialize(ar, &dyn_clause->provinces);
            } break;
            case TreatyClauseType::LIBERATE_NATION: {
                const auto dyn_clause = static_cast<const TreatyClause::LiberateNation*>(*obj);
                if(dyn_clause == nullptr)
                    throw SerializerException("Dynamic cast failed for TreatyClause::LiberateNation");
                ::serialize(ar, &dyn_clause->provinces);
                ::serialize(ar, &dyn_clause->liberated);
            } break;
            case TreatyClauseType::IMPOSE_POLICIES: {
                const auto dyn_clause = static_cast<const TreatyClause::ImposePolicies*>(*obj);
                if(dyn_clause == nullptr)
                    throw SerializerException("Dynamic cast failed for TreatyClause::ImposePolicies");
                ::serialize(ar, &dyn_clause->imposed);
            } break;
            case TreatyClauseType::MONEY: {
                const auto dyn_clause = static_cast<const TreatyClause::WarReparations*>(*obj);
                if(dyn_clause == nullptr)
                    throw SerializerException("Dynamic cast failed for TreatyClause::WarReparations");
                ::serialize(ar, &dyn_clause->amount);
            } break;
            case TreatyClauseType::HUMILIATE: {
                const auto dyn_clause = static_cast<const TreatyClause::Humiliate*>(*obj);
                if(dyn_clause == nullptr)
                    throw SerializerException("Dynamic cast failed for TreatyClause::Humiliate");
                ::serialize(ar, &dyn_clause->amount);
            } break;
            default:
                break;
            }

            // This statements must not be before the switch above, since the polymorphic
            // trans-serializer is not going to handle this well (see deserializer)
            ::serialize(ar, &(*obj)->sender);
            ::serialize(ar, &(*obj)->receiver);
            ::serialize(ar, &(*obj)->days_duration);
            ::serialize(ar, &(*obj)->done);
        } else {
            *obj = new TreatyClause::BaseClause();

            // We will also need to destroy the original pointer and allocate a new
            // one to accomodate the polymorphic stuff, if we dont destroy-then-construct
            // we will have serious SIGSEGV
            ::deserialize(ar, &(*obj)->type);
            switch((*obj)->type) {
            case TreatyClauseType::ANNEX_PROVINCES: {
                delete* obj;
                *obj = new TreatyClause::AnnexProvince();
                auto dyn_clause = (TreatyClause::AnnexProvince*)*obj;
                ::deserialize(ar, &dyn_clause->provinces);
            } break;
            case TreatyClauseType::LIBERATE_NATION: {
                delete* obj;
                *obj = new TreatyClause::LiberateNation();
                auto dyn_clause = (TreatyClause::LiberateNation*)*obj;
                ::deserialize(ar, &dyn_clause->provinces);
                ::deserialize(ar, &dyn_clause->liberated);
            } break;
            case TreatyClauseType::IMPOSE_POLICIES: {
                delete* obj;
                *obj = new TreatyClause::ImposePolicies();
                auto dyn_clause = (TreatyClause::ImposePolicies*)*obj;
                ::deserialize(ar, &dyn_clause->imposed);
            } break;
            case TreatyClauseType::MONEY: {
                delete* obj;
                *obj = new TreatyClause::WarReparations();
                auto dyn_clause = (TreatyClause::WarReparations*)*obj;
                ::deserialize(ar, &dyn_clause->amount);
            } break;
            case TreatyClauseType::HUMILIATE: {
                delete* obj;
                *obj = new TreatyClause::Humiliate();
                auto dyn_clause = (TreatyClause::Humiliate*)*obj;
                ::deserialize(ar, &dyn_clause->amount);
            } break;
            case TreatyClauseType::CEASEFIRE: {
                delete* obj;
                *obj = new TreatyClause::Ceasefire();
                //auto dyn_clause = (TreatyClause::Ceasefire*)*obj;
            } break;
            case TreatyClauseType::PUPPET: {
                delete* obj;
                *obj = new TreatyClause::Puppet();
                //auto dyn_clause = (TreatyClause::Puppet*)*obj;
            } break;
            default:
                break;
            }

            ::deserialize(ar, &(*obj)->sender);
            ::deserialize(ar, &(*obj)->receiver);
            ::deserialize(ar, &(*obj)->days_duration);
            ::deserialize(ar, &(*obj)->done);
        }
    }
};

template<>
class SerializerEntity<Treaty> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Treaty* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->receiver);
        ::deser_dynamic<is_serialize>(ar, &obj->sender);
        ::deser_dynamic<is_serialize>(ar, &obj->approval_status);
        ::deser_dynamic<is_serialize>(ar, &obj->clauses);
    }
};

template<>
class SerializerEntity<War> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, War* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->attackers);
        ::deser_dynamic<is_serialize>(ar, &obj->defenders);
        ::deser_dynamic<is_serialize>(ar, &obj->wargoals);
    }
};

template<>
class SerializerEntity<Ideology> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Ideology* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->color);
        ::deser_dynamic<is_serialize>(ar, &obj->check_policies_fn);
    }
};

template<>
class SerializerEntity<World> {
public:
    template<typename T>
    static inline typename T::Id deserialize_and_create_list(Archive& ar, World* obj) {
        typename T::Id n_elems;
        ::deserialize(ar, &n_elems);
        for(size_t i = 0; i < n_elems; i++) {
            T* sub_obj = new T();
            obj->insert(*sub_obj);
        }
        return n_elems;
    }

    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, World* obj) {
        if constexpr(is_serialize) {
            ::serialize(ar, &obj->width);
            ::serialize(ar, &obj->height);
            ::deserialize(ar, &obj->time);

            //for(size_t i = 0; i < obj->width * obj->height; i++) {
            //    ::deserialize(ar, &obj->tiles[i]);
            //}

            const Nation::Id n_nations = obj->nations.size();
            ::serialize(ar, &n_nations);
            const Province::Id n_provinces = obj->provinces.size();
            ::serialize(ar, &n_provinces);
            const Event::Id n_events = obj->events.size();
            ::serialize(ar, &n_events);
            const Treaty::Id n_treaties = obj->treaties.size();
            ::serialize(ar, &n_treaties);
            const War::Id n_wars = obj->wars.size();
            ::serialize(ar, &n_wars);

            ::serialize(ar, &obj->goods);
            ::serialize(ar, &obj->unit_types);
            ::serialize(ar, &obj->religions);
            ::serialize(ar, &obj->cultures);
            ::serialize(ar, &obj->pop_types);
            ::serialize(ar, &obj->terrain_types);
            ::serialize(ar, &obj->building_types);
            ::serialize(ar, &obj->ideologies);
            ::serialize(ar, &obj->technologies);
            ::serialize(ar, &obj->nation_modifiers);

            for(size_t i = 0; i < obj->nations.size() * 2; i++) {
                ::serialize(ar, &obj->relations);
            }
            for(auto& sub_obj : obj->nations) {
                ::serialize(ar, sub_obj);
            }
            for(auto& sub_obj : obj->provinces) {
                ::serialize(ar, sub_obj);
            }
            for(auto& sub_obj : obj->events) {
                ::serialize(ar, sub_obj);
            }
            for(auto& sub_obj : obj->treaties) {
                ::serialize(ar, sub_obj);
            }
            for(auto& sub_obj : obj->wars) {
                ::serialize(ar, sub_obj);
            }
        } else {
            ::deserialize(ar, &obj->width);
            ::deserialize(ar, &obj->height);
            ::deserialize(ar, &obj->time);

            //obj->tiles = new Tile[obj->width * obj->height];
            //for(size_t i = 0; i < obj->width * obj->height; i++) {
            //    ::deserialize(ar, &obj->tiles[i]);
            //}

            // In order to avoid post-deserialization relational patcher, we will simply allocate everything with "empty" objects,
            // then we will fill those spots as we deserialize
            Nation::Id n_nations = deserialize_and_create_list<Nation>(ar, obj);
            Province::Id n_provinces = deserialize_and_create_list<Province>(ar, obj);
            Event::Id n_events = deserialize_and_create_list<Event>(ar, obj);
            Treaty::Id n_treaties = deserialize_and_create_list<Treaty>(ar, obj);
            War::Id n_wars = deserialize_and_create_list<War>(ar, obj);
            ::deserialize(ar, &obj->goods);
            ::deserialize(ar, &obj->unit_types);
            ::deserialize(ar, &obj->religions);
            ::deserialize(ar, &obj->cultures);
            ::deserialize(ar, &obj->pop_types);
            ::deserialize(ar, &obj->terrain_types);
            ::deserialize(ar, &obj->building_types);
            ::deserialize(ar, &obj->ideologies);
            ::deserialize(ar, &obj->technologies);
            ::deserialize(ar, &obj->nation_modifiers);
            for(size_t i = 0; i < n_nations; i++) {
                ::deserialize(ar, &obj->relations[i]);
            }
            for(size_t i = 0; i < n_nations; i++) {
                auto* sub_obj = obj->nations[i];
                ::deserialize(ar, sub_obj);
            }
            for(size_t i = 0; i < n_provinces; i++) {
                auto* sub_obj = obj->provinces[i];
                ::deserialize(ar, sub_obj);
            }
            for(size_t i = 0; i < n_events; i++) {
                auto* sub_obj = obj->events[i];
                ::deserialize(ar, sub_obj);
            }
            for(size_t i = 0; i < n_treaties; i++) {
                auto* sub_obj = obj->treaties[i];
                ::deserialize(ar, sub_obj);
            }
            for(size_t i = 0; i < n_wars; i++) {
                auto* sub_obj = obj->wars[i];
                ::deserialize(ar, sub_obj);
            }
        }
    }
};