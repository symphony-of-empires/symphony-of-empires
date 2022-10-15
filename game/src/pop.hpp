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
//      pop.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include "objects.hpp"

struct Language : RefnameEntity<LanguageId> {
    std::uint32_t color;
    Eng3D::StringRef name;
    Eng3D::StringRef adjective;
    Eng3D::StringRef noun;
    Eng3D::StringRef combo_form;
};
template<>
struct Serializer<Language*> : SerializerReference<World, Language> {};
template<>
struct Serializer<const Language*> : SerializerReference<World, const Language> {};
template<>
struct Serializer<Language> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Language& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.adjective);
        ::deser_dynamic<is_serialize>(ar, obj.noun);
        ::deser_dynamic<is_serialize>(ar, obj.combo_form);
        ::deser_dynamic<is_serialize>(ar, obj.color);
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
struct Serializer<Religion*> : SerializerReference<World, Religion> {};
template<>
struct Serializer<const Religion*> : SerializerReference<World, const Religion> {};
template<>
struct Serializer<Religion> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Religion& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.color);
    }
};

enum class PopGroup : uint8_t {
    OTHER,
    BURGEOISE,
    SLAVE,
    FARMER,
    LABORER,
    SOLDIER,
    ARTISAN,
};
template<>
struct Serializer<PopGroup> : SerializerMemcpy<PopGroup> {};

struct PopType : RefnameEntity<PopTypeId> {
    Eng3D::StringRef name;
    float social_value = 1.f;
    enum PopGroup group = PopGroup::OTHER;
    std::vector<float> basic_needs_amount; // Amount of goods needed to satisfy basic needs
    std::vector<float> luxury_needs_satisfaction; // Amount of satisfaction each luxury good gives
    std::vector<float> luxury_needs_deminishing_factor; // Deminishing returns factor of the luxury good satisfaction
};
template<>
struct Serializer<PopType*> : SerializerReference<World, PopType> {};
template<>
struct Serializer<const PopType*> : SerializerReference<World, const PopType> {};
template<>
struct Serializer<PopType> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, PopType& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.social_value);
        ::deser_dynamic<is_serialize>(ar, obj.group);
        ::deser_dynamic<is_serialize>(ar, obj.basic_needs_amount);
        ::deser_dynamic<is_serialize>(ar, obj.luxury_needs_satisfaction);
        ::deser_dynamic<is_serialize>(ar, obj.luxury_needs_deminishing_factor);
    }
};

class Province;
class Ideology;
class Pop : public Entity<PopId> {
    Pop& operator=(const Pop&) = default;
    friend class Province;
public:
    constexpr bool operator==(const Pop& rhs) const {
        return this->type_id == rhs.type_id;
    }

    Ideology& get_ideology() const;

    PopTypeId type_id;
    float size = 0.f;
    float life_needs_met = 0.f;
    float literacy = 0.f;
    float militancy = 0.f;
    float budget = 0.f;
    std::vector<float> ideology_approval; // Approval % of all the ideologies (1:1)
};
template<>
struct Serializer<Pop> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Pop& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.size);
        ::deser_dynamic<is_serialize>(ar, obj.literacy);
        ::deser_dynamic<is_serialize>(ar, obj.militancy);
        ::deser_dynamic<is_serialize>(ar, obj.budget);
        ::deser_dynamic<is_serialize>(ar, obj.life_needs_met);
        ::deser_dynamic<is_serialize>(ar, obj.type_id);
        ::deser_dynamic<is_serialize>(ar, obj.ideology_approval);
    }
};
