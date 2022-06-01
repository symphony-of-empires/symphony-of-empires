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
class Serializer<Event*>: public SerializerReference<World, Event> {};
template<>
class Serializer<Treaty*>: public SerializerReference<World, Treaty> {};
template<>
class Serializer<Unit*>: public SerializerReference<World, Unit> {};

// Local references
template<>
class Serializer<Province*>: public SerializerReferenceLocal<World, Province> {};
template<>
class Serializer<Nation*>: public SerializerReferenceLocal<World, Nation> {};
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

template<>
class Serializer<NationModifier> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, NationModifier* obj) {
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
class Serializer<NationRelation> {
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
class Serializer<Policies> {
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
class Serializer<PopType> {
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
class Serializer<TerrainType> {
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
class Serializer<Culture> {
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
class Serializer<Religion> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Religion* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->color);
    }
};

template<>
class Serializer<UnitTrait> {
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
class Serializer<Unit> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Unit* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->type);
        ::deser_dynamic<is_serialize>(ar, &obj->size);
        ::deser_dynamic<is_serialize>(ar, &obj->target_province_id);
        ::deser_dynamic<is_serialize>(ar, &obj->province_id);
        ::deser_dynamic<is_serialize>(ar, &obj->owner);
        ::deser_dynamic<is_serialize>(ar, &obj->move_progress);
        ::deser_dynamic<is_serialize>(ar, &obj->on_battle);
    }
};

template<>
class Serializer<Pop> {
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
class Serializer<Decision> {
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
class Serializer<Event> {
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
class Serializer<Tile> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Tile* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->province_id);
    }
};

template<>
class Serializer<NationClientHint> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, NationClientHint* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->color);
        ::deser_dynamic<is_serialize>(ar, &obj->alt_name);
        ::deser_dynamic<is_serialize>(ar, &obj->ideology);
    }
};

template<>
class Serializer<Nation> {
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
        ::deser_dynamic<is_serialize>(ar, &obj->capital_id);
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
class Serializer<UnitType> {
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
class Serializer<Province> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Province* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
        ::deser_dynamic<is_serialize>(ar, &obj->color);
        ::deser_dynamic<is_serialize>(ar, &obj->box_area);
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
class Serializer<BuildingType> {
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
class Serializer<Building> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Building* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->working_unit_type);
        ::deser_dynamic<is_serialize>(ar, &obj->build_time);
        ::deser_dynamic<is_serialize>(ar, &obj->budget);
        ::deser_dynamic<is_serialize>(ar, &obj->days_unoperational);
        ::deser_dynamic<is_serialize>(ar, &obj->stockpile);
        ::deser_dynamic<is_serialize>(ar, &obj->req_goods);
    }
};

template<>
class Serializer<Product> {
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
class Serializer<Technology> {
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
class Serializer<Good> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Good* obj) {
        ::deser_dynamic<is_serialize>(ar, &obj->name);
        ::deser_dynamic<is_serialize>(ar, &obj->ref_name);
    }
};

template<>
class Serializer<TreatyClause::BaseClause*> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, TreatyClause::BaseClause** obj) {
        if constexpr(is_serialize) {
            ::deser_dynamic<is_serialize>(ar, &(*obj)->type);
            switch((*obj)->type) {
            case TreatyClauseType::ANNEX_PROVINCES: {
                const auto dyn_clause = static_cast<const TreatyClause::AnnexProvince*>(*obj);
                if(dyn_clause == nullptr)
                    CXX_THROW(SerializerException, "Dynamic cast failed for TreatyClause::AnnexProvince");
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->provinces);
            } break;
            case TreatyClauseType::LIBERATE_NATION: {
                const auto dyn_clause = static_cast<const TreatyClause::LiberateNation*>(*obj);
                if(dyn_clause == nullptr)
                    CXX_THROW(SerializerException, "Dynamic cast failed for TreatyClause::LiberateNation");
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->provinces);
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->liberated);
            } break;
            case TreatyClauseType::IMPOSE_POLICIES: {
                const auto dyn_clause = static_cast<const TreatyClause::ImposePolicies*>(*obj);
                if(dyn_clause == nullptr)
                    CXX_THROW(SerializerException, "Dynamic cast failed for TreatyClause::ImposePolicies");
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->imposed);
            } break;
            case TreatyClauseType::MONEY: {
                const auto dyn_clause = static_cast<const TreatyClause::WarReparations*>(*obj);
                if(dyn_clause == nullptr)
                    CXX_THROW(SerializerException, "Dynamic cast failed for TreatyClause::WarReparations");
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->amount);
            } break;
            case TreatyClauseType::HUMILIATE: {
                const auto dyn_clause = static_cast<const TreatyClause::Humiliate*>(*obj);
                if(dyn_clause == nullptr)
                    CXX_THROW(SerializerException, "Dynamic cast failed for TreatyClause::Humiliate");
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->amount);
            } break;
            default:
                break;
            }
        } else {
            TreatyClauseType type;
            ::deser_dynamic<is_serialize>(ar, &type);
            switch(type) {
            case TreatyClauseType::ANNEX_PROVINCES: {
                *obj = new TreatyClause::AnnexProvince();
                auto dyn_clause = (TreatyClause::AnnexProvince*)*obj;
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->provinces);
            } break;
            case TreatyClauseType::LIBERATE_NATION: {
                *obj = new TreatyClause::LiberateNation();
                auto dyn_clause = (TreatyClause::LiberateNation*)*obj;
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->provinces);
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->liberated);
            } break;
            case TreatyClauseType::IMPOSE_POLICIES: {
                *obj = new TreatyClause::ImposePolicies();
                auto dyn_clause = (TreatyClause::ImposePolicies*)*obj;
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->imposed);
            } break;
            case TreatyClauseType::MONEY: {
                *obj = new TreatyClause::WarReparations();
                auto dyn_clause = (TreatyClause::WarReparations*)*obj;
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->amount);
            } break;
            case TreatyClauseType::HUMILIATE: {
                *obj = new TreatyClause::Humiliate();
                auto dyn_clause = (TreatyClause::Humiliate*)*obj;
                ::deser_dynamic<is_serialize>(ar, &dyn_clause->amount);
            } break;
            case TreatyClauseType::CEASEFIRE: {
                *obj = new TreatyClause::Ceasefire();
                //auto dyn_clause = (TreatyClause::Ceasefire*)*obj;
            } break;
            case TreatyClauseType::PUPPET: {
                *obj = new TreatyClause::Puppet();
                //auto dyn_clause = (TreatyClause::Puppet*)*obj;
            } break;
            default:
                break;
            }
        }
        ::deser_dynamic<is_serialize>(ar, &(*obj)->sender);
        ::deser_dynamic<is_serialize>(ar, &(*obj)->receiver);
        ::deser_dynamic<is_serialize>(ar, &(*obj)->days_duration);
        ::deser_dynamic<is_serialize>(ar, &(*obj)->done);
    }
};

template<>
class Serializer<Treaty> {
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
class Serializer<War> {
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
class Serializer<Ideology> {
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
class Serializer<World> {
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
        ::deser_dynamic<is_serialize>(ar, &obj->width);
        ::deser_dynamic<is_serialize>(ar, &obj->height);
        ::deser_dynamic<is_serialize>(ar, &obj->time);
        ::deser_dynamic<is_serialize>(ar, &obj->goods);
        ::deser_dynamic<is_serialize>(ar, &obj->unit_types);
        ::deser_dynamic<is_serialize>(ar, &obj->religions);
        ::deser_dynamic<is_serialize>(ar, &obj->cultures);
        ::deser_dynamic<is_serialize>(ar, &obj->pop_types);
        ::deser_dynamic<is_serialize>(ar, &obj->terrain_types);
        ::deser_dynamic<is_serialize>(ar, &obj->building_types);
        ::deser_dynamic<is_serialize>(ar, &obj->ideologies);
        ::deser_dynamic<is_serialize>(ar, &obj->technologies);
        ::deser_dynamic<is_serialize>(ar, &obj->nation_modifiers);
        ::deser_dynamic<is_serialize>(ar, &obj->nations);
        ::deser_dynamic<is_serialize>(ar, &obj->provinces);
        if constexpr(is_serialize) {
            const Event::Id n_events = obj->events.size();
            ::deser_dynamic<is_serialize>(ar, &n_events);
            const Treaty::Id n_treaties = obj->treaties.size();
            ::deser_dynamic<is_serialize>(ar, &n_treaties);
            const War::Id n_wars = obj->wars.size();
            ::deser_dynamic<is_serialize>(ar, &n_wars);
            for(size_t i = 0; i < obj->nations.size() * 2; i++)
                ::deser_dynamic<is_serialize>(ar, &obj->relations);
            for(auto& sub_obj : obj->events)
                ::deser_dynamic<is_serialize>(ar, sub_obj);
            for(auto& sub_obj : obj->treaties)
                ::deser_dynamic<is_serialize>(ar, sub_obj);
            for(auto& sub_obj : obj->wars)
                ::deser_dynamic<is_serialize>(ar, sub_obj);
        } else {
            // In order to avoid post-deserialization relational patcher, we will simply allocate everything with "empty" objects,
            // then we will fill those spots as we deserialize
            Event::Id n_events = deserialize_and_create_list<Event>(ar, obj);
            Treaty::Id n_treaties = deserialize_and_create_list<Treaty>(ar, obj);
            War::Id n_wars = deserialize_and_create_list<War>(ar, obj);
            for(size_t i = 0; i < obj->nations.size() * obj->nations.size(); i++)
                ::deser_dynamic<is_serialize>(ar, &obj->relations[i]);
            for(size_t i = 0; i < n_events; i++) {
                auto* sub_obj = obj->events[i];
                ::deser_dynamic<is_serialize>(ar, sub_obj);
            }
            for(size_t i = 0; i < n_treaties; i++) {
                auto* sub_obj = obj->treaties[i];
                ::deser_dynamic<is_serialize>(ar, sub_obj);
            }
            for(size_t i = 0; i < n_wars; i++) {
                auto* sub_obj = obj->wars[i];
                ::deser_dynamic<is_serialize>(ar, sub_obj);
            }
            obj->tiles.reset(new Tile[obj->width * obj->height]);
        }

        // (De-)serialize all the tiles, for the deserialization path, see above
        for(size_t i = 0; i < obj->width * obj->height; i++)
            ::deser_dynamic<is_serialize>(ar, &obj->tiles[i]);
    }
};