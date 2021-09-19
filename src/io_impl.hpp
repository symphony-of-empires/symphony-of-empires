#ifndef IO_IMPL_HPP
#define IO_IMPL_HPP

#include "world.hpp"
#include "province.hpp"
#include "nation.hpp"
#include "product.hpp"
#include "industry.hpp"
#include "good.hpp"
#include "company.hpp"
#include "pop.hpp"
#include "serializer.hpp"
#include "actions.hpp"
#include "diplomacy.hpp"
#include "print.hpp"
#include <string>

// TODO: Endianess compatibility
template<>
class Serializer<enum ActionType> : public SerializerMemcpy<enum ActionType> {};
template<>
class Serializer<enum OrderType> : public SerializerMemcpy<enum OrderType> {};

// Used as a template for serializable objects (pointers mostly) which should be
// treated as a reference instead of the object itself
template<typename W, typename T>
class SerializerReference {
public:
    static inline void serialize(Archive& stream, const T* const* obj) {
        typename T::Id id = W::get_instance().get_id(*obj);
        ::serialize(stream, &id);
    };

    static inline void deserialize(Archive& stream, T** obj) {
        typename T::Id id;
        ::deserialize(stream, &id);
        if(id >= W::get_instance().get_list(*obj).size()) {
            *obj = nullptr;
            return;
        }
        *obj = (id != (typename T::Id)-1) ? W::get_instance().get_list(*obj).at(id) : nullptr;
    };

    static inline size_t size(const T* const*) {
        return sizeof(typename T::Id);
    };
};

template<>
class Serializer<Province*> : public SerializerReference<World, Province> {};
template<>
class Serializer<Nation*> : public SerializerReference<World, Nation> {};
template<>
class Serializer<Event*> : public SerializerReference<World, Event> {};
template<>
class Serializer<Treaty*> : public SerializerReference<World, Treaty> {};
template<>
class Serializer<Product*> : public SerializerReference<World, Product> {};
template<>
class Serializer<Culture*> : public SerializerReference<World, Culture> {};
template<>
class Serializer<Good*> : public SerializerReference<World, Good> {};
template<>
class Serializer<Company*> : public SerializerReference<World, Company> {};
template<>
class Serializer<IndustryType*> : public SerializerReference<World, IndustryType> {};
template<>
class Serializer<UnitType*> : public SerializerReference<World, UnitType> {};
template<>
class Serializer<BoatType*> : public SerializerReference<World, BoatType> {};
template<>
class Serializer<UnitTrait*> : public SerializerReference<World, UnitTrait> {};
template<>
class Serializer<Unit*> : public SerializerReference<World, Unit> {};
template<>
class Serializer<Boat*> : public SerializerReference<World, Boat> {};
template<>
class Serializer<OutpostType*> : public SerializerReference<World, OutpostType> {};
template<>
class Serializer<Outpost*> : public SerializerReference<World, Outpost> {};
template<>
class Serializer<Ideology*> : public SerializerReference<World, Ideology> {};

// TODO: Properly do this ffs - this is dangereous and **will** create bugs!
template<>
class Serializer<Industry *> : public SerializerMemcpy<Industry *> {};

template<>
class Serializer<NationRelation> : public SerializerMemcpy<NationRelation> {
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
    static inline size_t size(const NationRelation* obj) {
        return serialized_size(&obj->free_supplies)
            + serialized_size(&obj->has_alliance)
            + serialized_size(&obj->has_defensive_pact)
            + serialized_size(&obj->has_embargo)
            + serialized_size(&obj->has_embassy)
            + serialized_size(&obj->has_market_access)
            + serialized_size(&obj->has_military_access)
            + serialized_size(&obj->has_truce)
            + serialized_size(&obj->has_war)
            + serialized_size(&obj->interest)
            + serialized_size(&obj->relation)
        ;
    }
};

template<>
class Serializer<enum AllowancePolicy> : public SerializerMemcpy<enum AllowancePolicy> {};
template<>
class Serializer<enum CensorshipPolicy> : public SerializerMemcpy<enum CensorshipPolicy> {};
template<>
class Serializer<enum AutoBuildPolicy> : public SerializerMemcpy<enum AutoBuildPolicy> {};
template<>
class Serializer<enum TreatmentPolicy> : public SerializerMemcpy<enum TreatmentPolicy> {};

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
        ::serialize(stream, &obj->minimum_wage);
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
        ::deserialize(stream, &obj->minimum_wage);
    }
    static inline size_t size(const Policies* obj) {
        return serialized_size(&obj->free_supplies)
            + serialized_size(&obj->immigration)
            + serialized_size(&obj->import_tax)
            + serialized_size(&obj->industry_tax)
            + serialized_size(&obj->legislative_parliament)
            + serialized_size(&obj->med_flat_tax)
            + serialized_size(&obj->men_labour)
            + serialized_size(&obj->men_suffrage)
            + serialized_size(&obj->migration)
            + serialized_size(&obj->military_spending)
            + serialized_size(&obj->national_id)
            + serialized_size(&obj->poor_flat_tax)
            + serialized_size(&obj->private_property)
            + serialized_size(&obj->public_education)
            + serialized_size(&obj->public_healthcare)
            + serialized_size(&obj->rich_flat_tax)
            + serialized_size(&obj->secular_education)
            + serialized_size(&obj->slavery)
            + serialized_size(&obj->social_security)
            + serialized_size(&obj->treatment)
            + serialized_size(&obj->women_labour)
            + serialized_size(&obj->women_suffrage)
            + serialized_size(&obj->minimum_wage);
        ;
    }
};

template<>
class Serializer<PopType> {
public:
    static inline void serialize(Archive& stream, const PopType* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->average_budget);
    }
    static inline void deserialize(Archive& stream, PopType* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->average_budget);
    }
    static inline size_t size(const PopType* obj) {
        return serialized_size(&obj->name)
            + serialized_size(&obj->ref_name)
            + serialized_size(&obj->average_budget)
        ;
    }
};

template<>
class Serializer<Culture> {
public:
    static inline void serialize(Archive& stream, const Culture* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
    }
    static inline void deserialize(Archive& stream, Culture* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
    }
    static inline size_t size(const Culture* obj) {
        return serialized_size(&obj->name)
            + serialized_size(&obj->ref_name)
        ;
    }
};

template<>
class Serializer<Religion> {
public:
    static inline void serialize(Archive& stream, const Religion* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
    }
    static inline void deserialize(Archive& stream, Religion* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
    }
    static inline size_t size(const Religion* obj) {
        return serialized_size(&obj->name)
            + serialized_size(&obj->ref_name)
        ;
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
    static inline size_t size(const UnitTrait* obj) {
        return serialized_size(&obj->ref_name)
            + serialized_size(&obj->defense_mod)
            + serialized_size(&obj->attack_mod)
            + serialized_size(&obj->supply_consumption_mod)
            + serialized_size(&obj->speed_mod)
        ;
    }
};

template<>
class Serializer<Unit> {
public:
    static inline void serialize(Archive& stream, const Unit* obj) {
        ::serialize(stream, &obj->type);
        ::serialize(stream, &obj->size);
        ::serialize(stream, &obj->tx);
        ::serialize(stream, &obj->ty);
        ::serialize(stream, &obj->x);
        ::serialize(stream, &obj->y);
        ::serialize(stream, &obj->owner);
        ::serialize(stream, &obj->traits);
    }
    static inline void deserialize(Archive& stream, Unit* obj) {
        ::deserialize(stream, &obj->type);
        ::deserialize(stream, &obj->size);
        ::deserialize(stream, &obj->tx);
        ::deserialize(stream, &obj->ty);
        ::deserialize(stream, &obj->x);
        ::deserialize(stream, &obj->y);
        ::deserialize(stream, &obj->owner);
        ::deserialize(stream, &obj->traits);
    }
    static inline size_t size(const Unit* obj) {
        return serialized_size(&obj->type)
            + serialized_size(&obj->size)
            + serialized_size(&obj->tx)
            + serialized_size(&obj->ty)
            + serialized_size(&obj->x)
            + serialized_size(&obj->y)
            + serialized_size(&obj->owner)
            + serialized_size(&obj->traits)
        ;
    }
};

template<>
class Serializer<Boat> {
public:
    static inline void serialize(Archive& stream, const Boat* obj) {
        ::serialize(stream, &obj->type);
        ::serialize(stream, &obj->size);
        ::serialize(stream, &obj->tx);
        ::serialize(stream, &obj->ty);
        ::serialize(stream, &obj->x);
        ::serialize(stream, &obj->y);
        ::serialize(stream, &obj->owner);
        ::serialize(stream, &obj->traits);
    }
    static inline void deserialize(Archive& stream, Boat* obj) {
        ::deserialize(stream, &obj->type);
        ::deserialize(stream, &obj->size);
        ::deserialize(stream, &obj->tx);
        ::deserialize(stream, &obj->ty);
        ::deserialize(stream, &obj->x);
        ::deserialize(stream, &obj->y);
        ::deserialize(stream, &obj->owner);
        ::deserialize(stream, &obj->traits);
    }
    static inline size_t size(const Boat* obj) {
        return serialized_size(&obj->type)
            + serialized_size(&obj->size)
            + serialized_size(&obj->tx)
            + serialized_size(&obj->ty)
            + serialized_size(&obj->x)
            + serialized_size(&obj->y)
            + serialized_size(&obj->owner)
            + serialized_size(&obj->traits)
        ;
    }
};

template<>
class Serializer<Pop> {
public:
    static inline void serialize(Archive& stream, const Pop* obj) {
        ::serialize(stream, &obj->size);
        ::serialize(stream, &obj->unemployed);

        ::serialize(stream, &obj->literacy);
        ::serialize(stream, &obj->militancy);
        ::serialize(stream, &obj->consciousness);
        ::serialize(stream, &obj->budget);

        ::serialize(stream, &obj->life_needs_met);
        ::serialize(stream, &obj->everyday_needs_met);
        ::serialize(stream, &obj->luxury_needs_met);

        ::serialize(stream, &obj->type_id);
        ::serialize(stream, &obj->culture_id);
        ::serialize(stream, &obj->religion_id);
    }
    static inline void deserialize(Archive& stream, Pop* obj) {
        ::deserialize(stream, &obj->size);
        ::deserialize(stream, &obj->unemployed);
        
        ::deserialize(stream, &obj->literacy);
        ::deserialize(stream, &obj->militancy);
        ::deserialize(stream, &obj->consciousness);
        ::deserialize(stream, &obj->budget);

        ::deserialize(stream, &obj->life_needs_met);
        ::deserialize(stream, &obj->everyday_needs_met);
        ::deserialize(stream, &obj->luxury_needs_met);

        ::deserialize(stream, &obj->type_id);
        ::deserialize(stream, &obj->culture_id);
        ::deserialize(stream, &obj->religion_id);
    }
    static inline size_t size(const Pop* obj) {
        return serialized_size(&obj->size)
            + serialized_size(&obj->unemployed)
            + serialized_size(&obj->literacy)
            + serialized_size(&obj->militancy)
            + serialized_size(&obj->consciousness)
            + serialized_size(&obj->budget)
            + serialized_size(&obj->life_needs_met)
            + serialized_size(&obj->everyday_needs_met)
            + serialized_size(&obj->luxury_needs_met)
            + serialized_size(&obj->type_id)
            + serialized_size(&obj->culture_id)
            + serialized_size(&obj->religion_id)
        ;
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
    static inline size_t size(const Descision* obj) {
        return
            serialized_size(&obj->name)
            + serialized_size(&obj->ref_name)
            + serialized_size(&obj->do_descision_function)
            + serialized_size(&obj->effects)
        ;
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
    static inline size_t size(const Event* obj) {
        return
            serialized_size(&obj->ref_name)
            + serialized_size(&obj->conditions_function)
            + serialized_size(&obj->do_event_function)
            + serialized_size(&obj->receivers)
            + serialized_size(&obj->descisions)
            + serialized_size(&obj->title)
            + serialized_size(&obj->text)
        ;
    }
};

template<>
class Serializer<Tile> {
public:
    static inline void serialize(Archive& stream, const Tile* obj) {
        ::serialize(stream, &obj->elevation);
        ::serialize(stream, &obj->infra_level);
        ::serialize(stream, &obj->owner_id);
        ::serialize(stream, &obj->province_id);
    }
    static inline void deserialize(Archive& stream, Tile* obj) {
        ::deserialize(stream, &obj->elevation);
        ::deserialize(stream, &obj->infra_level);
        ::deserialize(stream, &obj->owner_id);
        ::deserialize(stream, &obj->province_id);
    }
    static inline size_t size(const Tile* obj) {
        return
            serialized_size(&obj->elevation)
            + serialized_size(&obj->infra_level)
            + serialized_size(&obj->owner_id)
            + serialized_size(&obj->province_id)
        ;
    }
};
template<>
class Serializer<OrderGoods> {
public:
    static inline void serialize(Archive& stream, const OrderGoods* obj) {
        ::serialize(stream, &obj->good);
        ::serialize(stream, &obj->industry);
        ::serialize(stream, &obj->outpost);
        ::serialize(stream, &obj->payment);
        ::serialize(stream, &obj->province);
        ::serialize(stream, &obj->quantity);
        ::serialize(stream, &obj->type);
    }
    static inline void deserialize(Archive& stream, OrderGoods* obj) {
        ::deserialize(stream, &obj->good);
        ::deserialize(stream, &obj->industry);
        ::deserialize(stream, &obj->outpost);
        ::deserialize(stream, &obj->payment);
        ::deserialize(stream, &obj->province);
        ::deserialize(stream, &obj->quantity);
        ::deserialize(stream, &obj->type);
    }
    static inline size_t size(const OrderGoods* obj) {
        return
            serialized_size(&obj->good)
            + serialized_size(&obj->industry)
            + serialized_size(&obj->outpost)
            + serialized_size(&obj->payment)
            + serialized_size(&obj->province)
            + serialized_size(&obj->quantity)
            + serialized_size(&obj->type)
        ;
    }
};

template<>
class Serializer<DeliverGoods> {
public:
    static inline void serialize(Archive& stream, const DeliverGoods* obj) {
        ::serialize(stream, &obj->good);
        ::serialize(stream, &obj->industry);
        ::serialize(stream, &obj->payment);
        ::serialize(stream, &obj->product);
        ::serialize(stream, &obj->province);
        ::serialize(stream, &obj->quantity);
    }
    static inline void deserialize(Archive& stream, DeliverGoods* obj) {
        ::deserialize(stream, &obj->good);
        ::deserialize(stream, &obj->industry);
        ::deserialize(stream, &obj->payment);
        ::deserialize(stream, &obj->product);
        ::deserialize(stream, &obj->province);
        ::deserialize(stream, &obj->quantity);
    }
    static inline size_t size(const DeliverGoods* obj) {
        return
            serialized_size(&obj->good)
            + serialized_size(&obj->industry)
            + serialized_size(&obj->payment)
            + serialized_size(&obj->product)
            + serialized_size(&obj->province)
            + serialized_size(&obj->quantity)
        ;
    }
};

template<>
class Serializer<NationClientHint> {
public:
    static inline void serialize(Archive& stream, const NationClientHint* obj) {
        ::serialize(stream, &obj->colour);
        ::serialize(stream, &obj->alt_name);
        ::serialize(stream, &obj->ideology);
    }
    static inline void deserialize(Archive& stream, NationClientHint* obj) {
        ::deserialize(stream, &obj->colour);
        ::deserialize(stream, &obj->alt_name);
        ::deserialize(stream, &obj->ideology);
    }
    static inline size_t size(const NationClientHint* obj) {
        return
            serialized_size(&obj->colour)
            + serialized_size(&obj->alt_name)
            + serialized_size(&obj->ideology)
        ;
    }
};

template<>
class Serializer<Nation> {
public:
    static inline void serialize(Archive& stream, const Nation* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->controlled_by_ai);
        ::serialize(stream, &obj->relations);
        ::serialize(stream, &obj->spherer_id);
        ::serialize(stream, &obj->diplomacy_points);
        ::serialize(stream, &obj->prestige);

        ::serialize(stream, &obj->base_literacy);
        ::serialize(stream, &obj->is_civilized);

        ::serialize(stream, &obj->infamy);
        ::serialize(stream, &obj->military_score);
        ::serialize(stream, &obj->naval_score);
        ::serialize(stream, &obj->economy_score);

        ::serialize(stream, &obj->budget);

        ::serialize(stream, &obj->capital);
        ::serialize(stream, &obj->accepted_cultures);
        ::serialize(stream, &obj->owned_provinces);
        ::serialize(stream, &obj->current_policy);
        ::serialize(stream, &obj->diplomatic_timer);

        ::serialize(stream, &obj->inbox);
        ::serialize(stream, &obj->client_hints);
        ::serialize(stream, &obj->ideology);
    }
    static inline void deserialize(Archive& stream, Nation* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->controlled_by_ai);
        ::deserialize(stream, &obj->relations);
        ::deserialize(stream, &obj->spherer_id);
        ::deserialize(stream, &obj->diplomacy_points);
        ::deserialize(stream, &obj->prestige);

        ::deserialize(stream, &obj->base_literacy);
        ::deserialize(stream, &obj->is_civilized);

        ::deserialize(stream, &obj->infamy);
        ::deserialize(stream, &obj->military_score);
        ::deserialize(stream, &obj->naval_score);
        ::deserialize(stream, &obj->economy_score);

        ::deserialize(stream, &obj->budget);

        ::deserialize(stream, &obj->capital);
        ::deserialize(stream, &obj->accepted_cultures);
        ::deserialize(stream, &obj->owned_provinces);
        ::deserialize(stream, &obj->current_policy);
        ::deserialize(stream, &obj->diplomatic_timer);

        ::deserialize(stream, &obj->inbox);
        ::deserialize(stream, &obj->client_hints);
        ::deserialize(stream, &obj->ideology);
    }
    static inline size_t size(const Nation* obj) {
        return
            serialized_size(&obj->name)
            + serialized_size(&obj->ref_name)
            + serialized_size(&obj->controlled_by_ai)
            + serialized_size(&obj->relations)
            + serialized_size(&obj->spherer_id)
            + serialized_size(&obj->diplomacy_points)
            + serialized_size(&obj->prestige)
            + serialized_size(&obj->base_literacy)
            + serialized_size(&obj->is_civilized)
            + serialized_size(&obj->infamy)
            + serialized_size(&obj->military_score)
            + serialized_size(&obj->naval_score)
            + serialized_size(&obj->economy_score)
            + serialized_size(&obj->budget)
            + serialized_size(&obj->capital)
            + serialized_size(&obj->accepted_cultures)
            + serialized_size(&obj->owned_provinces)
            + serialized_size(&obj->current_policy)
            + serialized_size(&obj->diplomatic_timer)
            + serialized_size(&obj->inbox)
            + serialized_size(&obj->client_hints)
            + serialized_size(&obj->ideology)
        ;
    }
};

template<>
class Serializer<TreatyClauseType> : public SerializerMemcpy<TreatyClauseType> {};

template<>
class Serializer<enum TreatyApproval> : public SerializerMemcpy<enum TreatyApproval> {};

template<>
class Serializer<BoatType> {
public:
    static inline void serialize(Archive& stream, const BoatType* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->speed);
        ::serialize(stream, &obj->max_health);
        ::serialize(stream, &obj->defense);
        ::serialize(stream, &obj->attack);
        ::serialize(stream, &obj->capacity);
        ::serialize(stream, &obj->build_time);
    }
    static inline void deserialize(Archive& stream, BoatType* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->speed);
        ::deserialize(stream, &obj->max_health);
        ::deserialize(stream, &obj->defense);
        ::deserialize(stream, &obj->attack);
        ::deserialize(stream, &obj->capacity);
        ::deserialize(stream, &obj->build_time);
    }
    static inline size_t size(const BoatType* obj) {
        return
            serialized_size(&obj->name)
            + serialized_size(&obj->ref_name)
            + serialized_size(&obj->speed)
            + serialized_size(&obj->max_health)
            + serialized_size(&obj->defense)
            + serialized_size(&obj->attack)
            + serialized_size(&obj->capacity)
            + serialized_size(&obj->build_time)
        ;
    }
};

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
    }
    static inline size_t size(const UnitType* obj) {
        return
            serialized_size(&obj->name)
            + serialized_size(&obj->ref_name)
            + serialized_size(&obj->supply_consumption)
            + serialized_size(&obj->speed)
            + serialized_size(&obj->max_health)
            + serialized_size(&obj->defense)
            + serialized_size(&obj->attack)
            + serialized_size(&obj->req_goods)
            + serialized_size(&obj->build_time)
        ;
    }
};

template<>
class Serializer<Province> {
public:
    static inline void serialize(Archive& stream, const Province* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        
        ::serialize(stream, &obj->color);
        ::serialize(stream, &obj->budget);
        ::serialize(stream, &obj->n_tiles);
        
        ::serialize(stream, &obj->max_x);
        ::serialize(stream, &obj->max_y);
        ::serialize(stream, &obj->min_x);
        ::serialize(stream, &obj->min_y);
        
        ::serialize(stream, &obj->supply_limit);
        ::serialize(stream, &obj->supply_rem);
        ::serialize(stream, &obj->worker_pool);
        
        ::serialize(stream, &obj->owner);
        
        ::serialize(stream, &obj->nucleuses);
        ::serialize(stream, &obj->neighbours);
        ::serialize(stream, &obj->stockpile);
        ::serialize(stream, &obj->industries);
        ::serialize(stream, &obj->products);
        ::serialize(stream, &obj->pops);
    }
    static inline void deserialize(Archive& stream, Province* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        
        ::deserialize(stream, &obj->color);
        ::deserialize(stream, &obj->budget);
        ::deserialize(stream, &obj->n_tiles);
        
        ::deserialize(stream, &obj->max_x);
        ::deserialize(stream, &obj->max_y);
        ::deserialize(stream, &obj->min_x);
        ::deserialize(stream, &obj->min_y);
        
        ::deserialize(stream, &obj->supply_limit);
        ::deserialize(stream, &obj->supply_rem);
        ::deserialize(stream, &obj->worker_pool);
        
        ::deserialize(stream, &obj->owner);
        
        ::deserialize(stream, &obj->nucleuses);
        ::deserialize(stream, &obj->neighbours);
        ::deserialize(stream, &obj->stockpile);
        ::deserialize(stream, &obj->industries);
        ::deserialize(stream, &obj->products);
        ::deserialize(stream, &obj->pops);
    }
    static inline size_t size(const Province* obj) {
        return
            serialized_size(&obj->name)
            + serialized_size(&obj->ref_name)
            + serialized_size(&obj->color)
            + serialized_size(&obj->budget)
            + serialized_size(&obj->n_tiles)
            + serialized_size(&obj->max_x)
            + serialized_size(&obj->max_y)
            + serialized_size(&obj->min_x)
            + serialized_size(&obj->min_y)
            + serialized_size(&obj->supply_limit)
            + serialized_size(&obj->supply_rem)
            + serialized_size(&obj->worker_pool)
            + serialized_size(&obj->owner)
            + serialized_size(&obj->nucleuses)
            + serialized_size(&obj->neighbours)
            + serialized_size(&obj->stockpile)
            + serialized_size(&obj->industries)
            + serialized_size(&obj->products)
            + serialized_size(&obj->pops)
        ;
        // TODO: Rest of fields
    }
};

template<>
class Serializer<OutpostType> {
public:
    static inline void serialize(Archive& stream, const OutpostType* obj) {
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->is_naval);
        ::serialize(stream, &obj->is_build_land_units);
        ::serialize(stream, &obj->is_build_naval_units);
        ::serialize(stream, &obj->defense_bonus);
        ::serialize(stream, &obj->req_goods);
    }
    static inline void deserialize(Archive& stream, OutpostType* obj) {
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->is_naval);
        ::deserialize(stream, &obj->is_build_land_units);
        ::deserialize(stream, &obj->is_build_naval_units);
        ::deserialize(stream, &obj->defense_bonus);
        ::deserialize(stream, &obj->req_goods);
    }
    static inline size_t size(const OutpostType* obj) {
        return
            serialized_size(&obj->ref_name)
            + serialized_size(&obj->is_naval)
            + serialized_size(&obj->is_build_land_units)
            + serialized_size(&obj->is_build_naval_units)
            + serialized_size(&obj->defense_bonus)
            + serialized_size(&obj->req_goods)
        ;
    }
};

template<>
class Serializer<Outpost> {
public:
    static inline void serialize(Archive& stream, const Outpost* obj) {
        ::serialize(stream, &obj->x);
        ::serialize(stream, &obj->y);
        ::serialize(stream, &obj->type);
        ::serialize(stream, &obj->owner);
        ::serialize(stream, &obj->working_unit_type);
        ::serialize(stream, &obj->working_boat_type);
        ::serialize(stream, &obj->build_time);
        ::serialize(stream, &obj->req_goods);
    }
    static inline void deserialize(Archive& stream, Outpost* obj) {
        ::deserialize(stream, &obj->x);
        ::deserialize(stream, &obj->y);
        ::deserialize(stream, &obj->type);
        ::deserialize(stream, &obj->owner);
        ::deserialize(stream, &obj->working_unit_type);
        ::deserialize(stream, &obj->working_boat_type);
        ::deserialize(stream, &obj->build_time);
        ::deserialize(stream, &obj->req_goods);
    }
    static inline size_t size(const Outpost* obj) {
        return
            serialized_size(&obj->x)
            + serialized_size(&obj->y)
            + serialized_size(&obj->type)
            + serialized_size(&obj->owner)
            + serialized_size(&obj->working_unit_type)
            + serialized_size(&obj->working_boat_type)
            + serialized_size(&obj->build_time)
            + serialized_size(&obj->req_goods)
        ;
    }
};

template<>
class Serializer<Company> {
public:
    static inline void serialize(Archive& stream, const Company* obj) {
        ::serialize(stream, &obj->name);
        
        ::serialize(stream, &obj->money);
        ::serialize(stream, &obj->is_transport);
        ::serialize(stream, &obj->is_retailer);
        ::serialize(stream, &obj->is_industry);
        
        ::serialize(stream, &obj->operating_provinces);
    }
    static inline void deserialize(Archive& stream, Company* obj) {
        ::deserialize(stream, &obj->name);
        
        ::deserialize(stream, &obj->money);
        ::deserialize(stream, &obj->is_transport);
        ::deserialize(stream, &obj->is_retailer);
        ::deserialize(stream, &obj->is_industry);
        
        ::deserialize(stream, &obj->operating_provinces);
    }
    static inline size_t size(const Company* obj) {
        return
            serialized_size(&obj->name)
            + serialized_size(&obj->money)
            + serialized_size(&obj->is_transport)
            + serialized_size(&obj->is_retailer)
            + serialized_size(&obj->is_industry)
            + serialized_size(&obj->operating_provinces)
        ;
    }
};

template<>
class Serializer<Industry> {
public:
    static inline void serialize(Archive& stream, const Industry* obj) {
        ::serialize(stream, &obj->owner);
        ::serialize(stream, &obj->type);
        ::serialize(stream, &obj->budget);
        ::serialize(stream, &obj->days_unoperational);
        ::serialize(stream, &obj->production_cost);
        ::serialize(stream, &obj->stockpile);
        ::serialize(stream, &obj->output_products);
        ::serialize(stream, &obj->min_quality);
        ::serialize(stream, &obj->willing_payment);
        ::serialize(stream, &obj->workers);
        ::serialize(stream, &obj->req_goods);
    }
    static inline void deserialize(Archive& stream, Industry* obj) {
        ::deserialize(stream, &obj->owner);
        ::deserialize(stream, &obj->type);
        ::deserialize(stream, &obj->budget);
        ::deserialize(stream, &obj->days_unoperational);
        ::deserialize(stream, &obj->production_cost);
        ::deserialize(stream, &obj->stockpile);
        ::deserialize(stream, &obj->output_products);
        ::deserialize(stream, &obj->min_quality);
        ::deserialize(stream, &obj->willing_payment);
        ::deserialize(stream, &obj->workers);
        ::deserialize(stream, &obj->req_goods);
    }
    static inline size_t size(const Industry* obj) {
        return
            serialized_size(&obj->owner)
            + serialized_size(&obj->type)
            + serialized_size(&obj->budget)
            + serialized_size(&obj->days_unoperational)
            + serialized_size(&obj->production_cost)
            + serialized_size(&obj->stockpile)
            + serialized_size(&obj->output_products)
            + serialized_size(&obj->min_quality)
            + serialized_size(&obj->willing_payment)
            + serialized_size(&obj->workers)
            + serialized_size(&obj->req_goods)
        ;
    }
};

template<>
class Serializer<Product> {
public:
    static inline void serialize(Archive& stream, const Product* obj) {
        ::serialize(stream, &obj->owner);
        ::serialize(stream, &obj->origin);
        ::serialize(stream, &obj->industry);
        ::serialize(stream, &obj->good);
        ::serialize(stream, &obj->price);
        ::serialize(stream, &obj->price_vel);
        ::serialize(stream, &obj->quality);
        ::serialize(stream, &obj->supply);
        ::serialize(stream, &obj->demand);
    }
    static inline void deserialize(Archive& stream, Product* obj) {
        ::deserialize(stream, &obj->owner);
        ::deserialize(stream, &obj->origin);
        ::deserialize(stream, &obj->industry);
        ::deserialize(stream, &obj->good);
        ::deserialize(stream, &obj->price);
        ::deserialize(stream, &obj->price_vel);
        ::deserialize(stream, &obj->quality);
        ::deserialize(stream, &obj->supply);
        ::deserialize(stream, &obj->demand);
    }
    static inline size_t size(const Product* obj) {
        return
            serialized_size(&obj->owner)
            + serialized_size(&obj->origin)
            + serialized_size(&obj->industry)
            + serialized_size(&obj->good)
            + serialized_size(&obj->price)
            + serialized_size(&obj->price_vel)
            + serialized_size(&obj->quality)
            + serialized_size(&obj->supply)
            + serialized_size(&obj->demand)
        ;
    }
};

template<>
class Serializer<IndustryType> {
public:
    static inline void serialize(Archive& stream, const IndustryType* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->inputs);
        ::serialize(stream, &obj->outputs);
        ::serialize(stream, &obj->req_goods);
    }
    static inline void deserialize(Archive& stream, IndustryType* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->inputs);
        ::deserialize(stream, &obj->outputs);
        ::deserialize(stream, &obj->req_goods);
    }
    static inline size_t size(const IndustryType* obj) {
        return
            serialized_size(&obj->name)
            + serialized_size(&obj->ref_name)
            + serialized_size(&obj->inputs)
            + serialized_size(&obj->outputs)
            + serialized_size(&obj->req_goods)
        ;
    }
};

template<>
class Serializer<Good> {
public:
    static inline void serialize(Archive& stream, const Good* obj) {
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->is_edible);
    }
    static inline void deserialize(Archive& stream, Good* obj) {
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->is_edible);
    }
    static inline size_t size(const Good* obj) {
        return
            serialized_size(&obj->name)
            + serialized_size(&obj->ref_name)
            + serialized_size(&obj->is_edible)
        ;
    }
};

template<>
class Serializer<TreatyClause::BaseClause*> {
public:
    static inline void serialize(Archive& stream, const TreatyClause::BaseClause* const* obj) {
        ::serialize(stream, &(*obj)->type);
        switch((*obj)->type) {
        case TREATY_CLAUSE_ANEXX_PROVINCES:
            {
                const auto dyn_clause = dynamic_cast<const TreatyClause::AnexxProvince*>(*obj);
                if(dyn_clause == nullptr)
                    throw SerializerException("Dynamic cast failed for TreatyClause::AnexxProvince");
                ::serialize(stream, &dyn_clause->provinces);
            }
            break;
        case TREATY_CLAUSE_LIBERATE_NATION:
            {
                const auto dyn_clause = dynamic_cast<const TreatyClause::LiberateNation*>(*obj);
                if(dyn_clause == nullptr)
                    throw SerializerException("Dynamic cast failed for TreatyClause::LiberateNation");
                ::serialize(stream, &dyn_clause->provinces);
                ::serialize(stream, &dyn_clause->liberated);
            }
            break;
        case TREATY_CLAUSE_IMPOSE_POLICIES:
            {
                const auto dyn_clause = dynamic_cast<const TreatyClause::ImposePolicies*>(*obj);
                if(dyn_clause == nullptr)
                    throw SerializerException("Dynamic cast failed for TreatyClause::ImposePolicies");
                ::serialize(stream, &dyn_clause->imposed);
            }
            break;
        case TREATY_CLAUSE_WAR_REPARATIONS:
            {
                const auto dyn_clause = dynamic_cast<const TreatyClause::WarReparations*>(*obj);
                if(dyn_clause == nullptr)
                    throw SerializerException("Dynamic cast failed for TreatyClause::WarReparations");
                ::serialize(stream, &dyn_clause->amount);
            }
            break;
        case TREATY_CLAUSE_HUMILIATE:
            {
                const auto dyn_clause = dynamic_cast<const TreatyClause::Humiliate*>(*obj);
                if(dyn_clause == nullptr)
                    throw SerializerException("Dynamic cast failed for TreatyClause::Humiliate");
                ::serialize(stream, &dyn_clause->amount);
            }
            break;
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
        case TREATY_CLAUSE_ANEXX_PROVINCES:
            {
                delete *obj;
                *obj = new TreatyClause::AnexxProvince();
                auto dyn_clause = (TreatyClause::AnexxProvince*)*obj;
                ::deserialize(stream, &dyn_clause->provinces);
            }
            break;
        case TREATY_CLAUSE_LIBERATE_NATION:
            {
                delete *obj;
                *obj = new TreatyClause::LiberateNation();
                auto dyn_clause = (TreatyClause::LiberateNation*)*obj;
                ::deserialize(stream, &dyn_clause->provinces);
                ::deserialize(stream, &dyn_clause->liberated);
            }
            break;
        case TREATY_CLAUSE_IMPOSE_POLICIES:
            {
                delete *obj;
                *obj = new TreatyClause::ImposePolicies();
                auto dyn_clause = (TreatyClause::ImposePolicies*)*obj;
                ::deserialize(stream, &dyn_clause->imposed);
            }
            break;
        case TREATY_CLAUSE_WAR_REPARATIONS:
            {
                delete *obj;
                *obj = new TreatyClause::WarReparations();
                auto dyn_clause = (TreatyClause::WarReparations*)*obj;
                ::deserialize(stream, &dyn_clause->amount);
            }
            break;
        case TREATY_CLAUSE_HUMILIATE:
            {
                delete *obj;
                *obj = new TreatyClause::Humiliate();
                auto dyn_clause = (TreatyClause::Humiliate*)*obj;
                ::deserialize(stream, &dyn_clause->amount);
            }
            break;
        case TREATY_CLAUSE_CEASEFIRE:
            {
                delete *obj;
                *obj = new TreatyClause::Ceasefire();
                auto dyn_clause = (TreatyClause::Ceasefire*)*obj;
            }
            break;
        default:
            break;
        }

        ::deserialize(stream, &(*obj)->sender);
        ::deserialize(stream, &(*obj)->receiver);
        ::deserialize(stream, &(*obj)->days_duration);
        ::deserialize(stream, &(*obj)->done);
    }
    static inline size_t size(const TreatyClause::BaseClause* const* obj) {
        size_t total = 0;
        total += ::serialized_size(&(*obj)->type);
        switch((*obj)->type) {
        case TREATY_CLAUSE_ANEXX_PROVINCES:
            {
                const auto dyn_clause = dynamic_cast<const TreatyClause::AnexxProvince*>(*obj);
                total += ::serialized_size(&dyn_clause->provinces);
            }
            break;
        case TREATY_CLAUSE_LIBERATE_NATION:
            {
                const auto dyn_clause = dynamic_cast<const TreatyClause::LiberateNation*>(*obj);
                total += ::serialized_size(&dyn_clause->provinces);
                total += ::serialized_size(&dyn_clause->liberated);
            }
            break;
        case TREATY_CLAUSE_IMPOSE_POLICIES:
            {
                const auto dyn_clause = dynamic_cast<const TreatyClause::ImposePolicies*>(*obj);
                total += ::serialized_size(&dyn_clause->imposed);
            }
            break;
        case TREATY_CLAUSE_WAR_REPARATIONS:
            {
                const auto dyn_clause = dynamic_cast<const TreatyClause::WarReparations*>(*obj);
                total += ::serialized_size(&dyn_clause->amount);
            }
            break;
        case TREATY_CLAUSE_HUMILIATE:
            {
                const auto dyn_clause = dynamic_cast<const TreatyClause::Humiliate*>(*obj);
                total += ::serialized_size(&dyn_clause->amount);
            }
            break;
        default:
            break;
        }

        total += ::serialized_size(&(*obj)->receiver);
        total += ::serialized_size(&(*obj)->sender);
        total += ::serialized_size(&(*obj)->days_duration);
        total += ::serialized_size(&(*obj)->done);
        return total;
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
    static inline size_t size(const Treaty* obj) {
        return
            serialized_size(&obj->name)
            + serialized_size(&obj->receiver)
            + serialized_size(&obj->sender)
            + serialized_size(&obj->approval_status)
            + serialized_size(&obj->clauses)
        ;
    }
};

template<>
class Serializer<Ideology> {
public:
    static inline void serialize(Archive& stream, const Ideology* obj) {
        ::serialize(stream, &obj->ref_name);
        ::serialize(stream, &obj->name);
        ::serialize(stream, &obj->check_policies_fn);
    }
    static inline void deserialize(Archive& stream, Ideology* obj) {
        ::deserialize(stream, &obj->ref_name);
        ::deserialize(stream, &obj->name);
        ::deserialize(stream, &obj->check_policies_fn);
    }
    static inline size_t size(const Ideology* obj) {
        return
            serialized_size(&obj->ref_name)
            + serialized_size(&obj->name)
            + serialized_size(&obj->check_policies_fn)
        ;
    }
};

template<>
class Serializer<World> {
public:
    static inline void serialize(Archive& stream, const World* obj) {
        ::serialize(stream, &obj->width);
        ::serialize(stream, &obj->height);
        ::serialize(stream, &obj->sea_level);
        ::serialize(stream, &obj->time);
        
        for(size_t i = 0; i < obj->width * obj->height; i++) {
            ::serialize(stream, &obj->tiles[i]);
        }
        
        const uint32_t n_goods = obj->goods.size();
        ::serialize(stream, &n_goods);
        const uint32_t n_industry_types = obj->industry_types.size();
        ::serialize(stream, &n_industry_types);
        const uint32_t n_unit_types = obj->unit_types.size();
        ::serialize(stream, &n_unit_types);
        const uint32_t n_boat_types = obj->boat_types.size();
        ::serialize(stream, &n_boat_types);
        const uint32_t n_religions = obj->religions.size();
        ::serialize(stream, &n_religions);
        const uint32_t n_cultures = obj->cultures.size();
        ::serialize(stream, &n_cultures);
        const uint32_t n_pop_types = obj->pop_types.size();
        ::serialize(stream, &n_pop_types);
        const uint32_t n_nations = obj->nations.size();
        ::serialize(stream, &n_nations);
        const uint32_t n_provinces = obj->provinces.size();
        ::serialize(stream, &n_provinces);
        const uint32_t n_companies = obj->companies.size();
        ::serialize(stream, &n_companies);
        const uint32_t n_products = obj->products.size();
        ::serialize(stream, &n_products);
        const uint32_t n_events = obj->events.size();
        ::serialize(stream, &n_events);
        const uint32_t n_unit_traits = obj->unit_traits.size();
        ::serialize(stream, &n_unit_traits);
        const uint32_t n_outpost_types = obj->outpost_types.size();
        ::serialize(stream, &n_outpost_types);
        const uint32_t n_outposts = obj->outposts.size();
        ::serialize(stream, &n_outposts);
        const uint32_t n_treaties = obj->treaties.size();
        ::serialize(stream, &n_treaties);
        const uint32_t n_boats = obj->boats.size();
        ::serialize(stream, &n_boats);
        const uint32_t n_ideologies = obj->ideologies.size();
        ::serialize(stream, &n_ideologies);
        
        print_info("(SERIALIZER) WORLD INFORMATION\n");
        print_info("  n_goods %zu\n", (size_t)n_goods);
        print_info("  n_industry_types %zu\n", (size_t)n_industry_types);
        print_info("  n_unit_types %zu\n", (size_t)n_unit_types);
        print_info("  n_religions %zu\n", (size_t)n_religions);
        print_info("  n_cultures %zu\n", (size_t)n_cultures);
        print_info("  n_pop_types %zu\n", (size_t)n_pop_types);
        print_info("  n_nations %zu\n", (size_t)n_nations);
        print_info("  n_provinces %zu\n", (size_t)n_provinces);
        print_info("  n_companies %zu\n", (size_t)n_companies);
        print_info("  n_products %zu\n", (size_t)n_products);
        print_info("  n_events %zu\n", (size_t)n_events);
        print_info("  n_unit_traits %zu\n", (size_t)n_unit_traits);
        print_info("  n_outpost_types %zu\n", (size_t)n_outpost_types);
        print_info("  n_outposts %zu\n", (size_t)n_outposts);
        print_info("  n_treaties %zu\n", (size_t)n_treaties);
        print_info("  n_boats %zu\n", (size_t)n_boats);
        print_info("  n_ideologies %zu\n", (size_t)n_ideologies);
        
        for(auto& sub_obj: obj->goods) {
            ::serialize(stream, sub_obj);
        }
        
        for(auto& sub_obj: obj->industry_types) {
            ::serialize(stream, sub_obj);
        }
        
        for(auto& sub_obj: obj->unit_types) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj: obj->boat_types) {
            ::serialize(stream, sub_obj);
        }
        
        for(auto& sub_obj: obj->religions) {
            ::serialize(stream, sub_obj);
        }
        
        for(auto& sub_obj: obj->cultures) {
            ::serialize(stream, sub_obj);
        }
        
        for(auto& sub_obj: obj->pop_types) {
            ::serialize(stream, sub_obj);
        }
        
        for(auto& sub_obj: obj->nations) {
            ::serialize(stream, sub_obj);
        }
        
        for(auto& sub_obj: obj->provinces) {
            ::serialize(stream, sub_obj);
        }
        
        for(auto& sub_obj: obj->companies) {
            ::serialize(stream, sub_obj);
        }
        
        for(auto& sub_obj: obj->products) {
            ::serialize(stream, sub_obj);
        }
        
        for(auto& sub_obj: obj->events) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj: obj->unit_traits) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj: obj->outpost_types) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj: obj->outposts) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj: obj->treaties) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj: obj->boats) {
            ::serialize(stream, sub_obj);
        }

        for(auto& sub_obj: obj->ideologies) {
            ::serialize(stream, sub_obj);
        }
        
        ::serialize(stream, &obj->delivers);
        ::serialize(stream, &obj->orders);
    }

    static inline void deserialize(Archive& stream, World* obj) {
        ::deserialize(stream, &obj->width);
        ::deserialize(stream, &obj->height);
        ::deserialize(stream, &obj->sea_level);
        ::deserialize(stream, &obj->time);
        
        obj->tiles = new Tile[obj->width * obj->height];
        for(size_t i = 0; i < obj->width * obj->height; i++) {
            ::deserialize(stream, &obj->tiles[i]);
        }
        
        /* In order to avoid post-deserialization relational patcher,
        * we will simply allocate everything with "empty" objects,
        * then we will fill those spots as we deserialize
         */
        uint32_t n_goods;
        ::deserialize(stream, &n_goods);
        for(size_t i = 0; i < n_goods; i++) {
            Good* sub_obj = new Good();
            obj->goods.push_back(sub_obj);
        }
        
        uint32_t n_industry_types;
        ::deserialize(stream, &n_industry_types);
        for(size_t i = 0; i < n_industry_types; i++) {
            IndustryType* sub_obj = new IndustryType();
            obj->industry_types.push_back(sub_obj);
        }
        
        uint32_t n_unit_types;
        ::deserialize(stream, &n_unit_types);
        for(size_t i = 0; i < n_unit_types; i++) {
            UnitType* sub_obj = new UnitType();
            obj->unit_types.push_back(sub_obj);
        }

        uint32_t n_boat_types;
        ::deserialize(stream, &n_boat_types);
        for(size_t i = 0; i < n_boat_types; i++) {
            BoatType* sub_obj = new BoatType();
            obj->boat_types.push_back(sub_obj);
        }
        
        uint32_t n_religions;
        ::deserialize(stream, &n_religions);
        for(size_t i = 0; i < n_religions; i++) {
            Religion* sub_obj = new Religion();
            obj->religions.push_back(sub_obj);
        }
        
        uint32_t n_cultures;
        ::deserialize(stream, &n_cultures);
        for(size_t i = 0; i < n_cultures; i++) {
            Culture* sub_obj = new Culture();
            obj->cultures.push_back(sub_obj);
        }
        
        uint32_t n_pop_types;
        ::deserialize(stream, &n_pop_types);
        for(size_t i = 0; i < n_pop_types; i++) {
            PopType* sub_obj = new PopType();
            obj->pop_types.push_back(sub_obj);
        }
        
        uint32_t n_nations;
        ::deserialize(stream, &n_nations);
        for(size_t i = 0; i < n_nations; i++) {
            Nation* sub_obj = new Nation();
            obj->nations.push_back(sub_obj);
        }
        
        uint32_t n_provinces;
        ::deserialize(stream, &n_provinces);
        for(size_t i = 0; i < n_provinces; i++) {
            Province* sub_obj = new Province();
            obj->provinces.push_back(sub_obj);
        }
        
        uint32_t n_companies;
        ::deserialize(stream, &n_companies);
        for(size_t i = 0; i < n_companies; i++) {
            Company* sub_obj = new Company();
            obj->companies.push_back(sub_obj);
        }
        
        uint32_t n_products;
        ::deserialize(stream, &n_products);
        for(size_t i = 0; i < n_products; i++) {
            Product* sub_obj = new Product();
            obj->products.push_back(sub_obj);
        }
        
        uint32_t n_events;
        ::deserialize(stream, &n_events);
        for(size_t i = 0; i < n_events; i++) {
            Event* sub_obj = new Event();
            obj->events.push_back(sub_obj);
        }

        uint32_t n_unit_traits;
        ::deserialize(stream, &n_unit_traits);
        for(size_t i = 0; i < n_unit_traits; i++) {
            UnitTrait* sub_obj = new UnitTrait();
            obj->unit_traits.push_back(sub_obj);
        }

        uint32_t n_outpost_types;
        ::deserialize(stream, &n_outpost_types);
        for(size_t i = 0; i < n_outpost_types; i++) {
            OutpostType* sub_obj = new OutpostType();
            obj->outpost_types.push_back(sub_obj);
        }

        uint32_t n_outposts;
        ::deserialize(stream, &n_outposts);
        for(size_t i = 0; i < n_outposts; i++) {
            Outpost* sub_obj = new Outpost();
            obj->outposts.push_back(sub_obj);
        }

        uint32_t n_treaties;
        ::deserialize(stream, &n_treaties);
        for(size_t i = 0; i < n_treaties; i++) {
            Treaty* sub_obj = new Treaty();
            obj->treaties.push_back(sub_obj);
        }

        uint32_t n_boats;
        ::deserialize(stream, &n_boats);
        for(size_t i = 0; i < n_boats; i++) {
            Boat* sub_obj = new Boat();
            obj->boats.push_back(sub_obj);
        }

        uint32_t n_ideologies;
        ::deserialize(stream, &n_ideologies);
        for(size_t i = 0; i < n_ideologies; i++) {
            Ideology* sub_obj = new Ideology();
            obj->ideologies.push_back(sub_obj);
        }
        
        print_info("(DESERIALIZER) WORLD INFORMATION\n");
        print_info("  n_goods %zu\n", (size_t)n_goods);
        print_info("  n_industry_types %zu\n", (size_t)n_industry_types);
        print_info("  n_unit_types %zu\n", (size_t)n_unit_types);
        print_info("  n_religions %zu\n", (size_t)n_religions);
        print_info("  n_cultures %zu\n", (size_t)n_cultures);
        print_info("  n_pop_types %zu\n", (size_t)n_pop_types);
        print_info("  n_nations %zu\n", (size_t)n_nations);
        print_info("  n_provinces %zu\n", (size_t)n_provinces);
        print_info("  n_companies %zu\n", (size_t)n_companies);
        print_info("  n_products %zu\n", (size_t)n_products);
        print_info("  n_events %zu\n", (size_t)n_events);
        print_info("  n_unit_traits %zu\n", (size_t)n_unit_traits);
        print_info("  n_outpost_types %zu\n", (size_t)n_outpost_types);
        print_info("  n_outposts %zu\n", (size_t)n_outposts);
        print_info("  n_treaties %zu\n", (size_t)n_treaties);
        print_info("  n_boats %zu\n", (size_t)n_boats);
        print_info("  n_ideologies %zu\n", (size_t)n_ideologies);
        
        for(size_t i = 0; i < n_goods; i++) {
            Good* sub_obj = obj->goods[i];
            ::deserialize(stream, sub_obj);
        }
        
        for(size_t i = 0; i < n_industry_types; i++) {
            IndustryType* sub_obj = obj->industry_types[i];
            ::deserialize(stream, sub_obj);
        }
        
        for(size_t i = 0; i < n_unit_types; i++) {
            UnitType* sub_obj = obj->unit_types[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_boat_types; i++) {
            BoatType* sub_obj = obj->boat_types[i];
            ::deserialize(stream, sub_obj);
        }
        
        for(size_t i = 0; i < n_religions; i++) {
            Religion* sub_obj = obj->religions[i];
            ::deserialize(stream, sub_obj);
        }
        
        for(size_t i = 0; i < n_cultures; i++) {
            Culture* sub_obj = obj->cultures[i];
            ::deserialize(stream, sub_obj);
        }
        
        for(size_t i = 0; i < n_pop_types; i++) {
            PopType* sub_obj = obj->pop_types[i];
            ::deserialize(stream, sub_obj);
        }
        
        for(size_t i = 0; i < n_nations; i++) {
            Nation* sub_obj = obj->nations[i];
            ::deserialize(stream, sub_obj);
        }
        
        for(size_t i = 0; i < n_provinces; i++) {
            Province* sub_obj = obj->provinces[i];
            ::deserialize(stream, sub_obj);
        }
        
        for(size_t i = 0; i < n_companies; i++) {
            Company* sub_obj = obj->companies[i];
            ::deserialize(stream, sub_obj);
        }
        
        for(size_t i = 0; i < n_products; i++) {
            Product* sub_obj = obj->products[i];
            ::deserialize(stream, sub_obj);
        }
        
        for(size_t i = 0; i < n_events; i++) {
            Event* sub_obj = obj->events[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_unit_traits; i++) {
            UnitTrait* sub_obj = obj->unit_traits[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_outpost_types; i++) {
            OutpostType* sub_obj = obj->outpost_types[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_outposts; i++) {
            Outpost* sub_obj = obj->outposts[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_treaties; i++) {
            Treaty* sub_obj = obj->treaties[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_boats; i++) {
            Boat* sub_obj = obj->boats[i];
            ::deserialize(stream, sub_obj);
        }

        for(size_t i = 0; i < n_ideologies; i++) {
            Ideology* sub_obj = obj->ideologies.at(i);
            ::deserialize(stream, sub_obj);
        }
        
        ::deserialize(stream, &obj->delivers);
        ::deserialize(stream, &obj->orders);
    }

    static inline size_t size(const World* obj) {
        return
            serialized_size(&obj->width)
            + serialized_size(&obj->height)
            + serialized_size(&obj->sea_level)
            + serialized_size(&obj->time)
            + serialized_size(&obj->delivers)
            + serialized_size(&obj->orders)
            + (sizeof(Tile) * (obj->width* obj->height))
            + (obj->goods.size() * sizeof(Good))
            + (obj->industry_types.size() * sizeof(IndustryType))
            + (obj->unit_types.size() * sizeof(UnitType))
            + (obj->boat_types.size() * sizeof(BoatType))
            + (obj->religions.size() * sizeof(Religion))
            + (obj->cultures.size() * sizeof(Culture))
            + (obj->pop_types.size() * sizeof(PopType))
            + (obj->nations.size() * sizeof(Nation))
            + (obj->provinces.size() * sizeof(Province))
            + (obj->companies.size() * sizeof(Company))
            + (obj->products.size() * sizeof(Product))
            + (obj->events.size() * sizeof(Event))
            + (obj->unit_traits.size() * sizeof(UnitTrait))
            + (obj->outpost_types.size() * sizeof(OutpostType))
            + (obj->outposts.size() * sizeof(Outpost))
            + (obj->treaties.size() * sizeof(Treaty))
            + (obj->boats.size() * sizeof(Boat))
            + (obj->ideologies.size() * sizeof(Ideology))
        ;
    }
};

#endif
