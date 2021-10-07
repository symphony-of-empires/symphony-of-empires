#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include "good.hpp"

/**
* Defines a type of unit, it can be a tank, garrison, infantry, etc
* this is moddable via a lua script and new unit types can be added
 */
class UnitType {
public:
    using Id = uint16_t;
    std::string name;
    std::string ref_name;
    
    float supply_consumption;
    float speed;
    float max_health;
    float defense;
    float attack;

    // Max ticks allowed for defensive bonus - this basically prevents an unit from staying 200
    // years on the same spot and be fucking indestructible
    uint64_t max_defensive_ticks;

    // The defense provided by the unit each tick for staying on the same position
    // aka. digging trenches
    float position_defense;

    // Capacity of units that can be carried (transport units)
    size_t capacity;

    // Time needed to build
    size_t build_time;

    // Can go on ground?
    bool is_ground;
    
    // Can go on water?
    bool is_naval;

    // Required goods, first describes the id of the good and the second describes how many
    std::vector<std::pair<Good *, size_t>> req_goods;
};

/** A trait for an unit; given randomly per each recruited unit
 */
class UnitTrait {
public:
    using Id = uint16_t;
    std::string ref_name;
    
    float supply_consumption_mod;
    float speed_mod;
    float max_health_mod;
    float defense_mod;
    float attack_mod;
    float morale_mod;
};

#include "nation.hpp"
/**
* Roughly a batallion, consisting of approximately 500 soldiers each
 */
class Unit {
public:
    using Id = uint32_t;

    void attack(Unit& enemy) {
        // Calculate the attack of our unit
        float attack_mod = 0.f;
        for(const auto& trait: this->traits) {
            attack_mod *= trait->attack_mod;
        }
        const float attack = this->type->attack * attack_mod;

        // Calculate the defense of the enemy
        float defense_mod = 0.f;
        for(const auto& trait: this->traits) {
            defense_mod *= trait->defense_mod;
        }
        const float enemy_defense = std::max(0.1f, enemy.type->defense * defense_mod);

        // Calculate the total damage dealt by our unit to the enemy
        const float damage_dealt = this->size * std::min(10.f, std::max(.05f, this->experience))
            * (attack / std::pow(std::min(0.f, enemy_defense), 2))
            * std::max(0.1f, this->morale) * this->supply
        ;
        
        // Deal with the morale loss of the enemy
        float enemy_fanaticism = 0.f;
        for(const auto& trait: enemy.traits) {
            enemy_fanaticism *= trait->morale_mod;
        }
        enemy.morale -= 10.f * enemy_fanaticism * damage_dealt / enemy.size;

        // Our unit receives half of the morale
        this->morale += 5.f * enemy_fanaticism * damage_dealt / enemy.size;

        // Deal the damage
        enemy.size -= damage_dealt;
    };
    
    // Type of unit
    UnitType* type;

    // Size of the unit (soldiers in unit)
    size_t size;

    // Base size of the unit (max size due to anti-attrition)
    size_t base;

    // Target coordinates of this unit
    size_t tx, ty;

    // Current position
    float x, y;

    // Who owns this unit
    Nation* owner;

    float morale;

    // For perspective, 0.5 is the normal unit (i.e a soldier POP)
    float experience;

    // Used to "ignore" an unit when doing any check, this allows other units to
    // attack this unit
    bool ignore_tag;

    // The ticks the unit has not been moved
    uint64_t defensive_ticks;

    // Available supplies, 1.0 is all supplies fullfilled, lower than that and the unit starts shrinking
    float supply;

    // Money that the unit has
    float budget;
    
    std::vector<UnitTrait*> traits;
};

#endif
