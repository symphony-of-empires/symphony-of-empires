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
#include "policy.hpp"

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
    using type = Eng3D::Deser::CondConstType<is_const, Language>::type;
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
    using type = Eng3D::Deser::CondConstType<is_const, Religion>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.color);
    }
};

enum class PopGroup : uint8_t {
    OTHER,
    BURGEOISE,
    SLAVE,
    LABORER,
    SOLDIER,
    ARTISAN,
};
template<>
struct Eng3D::Deser::Serializer<PopGroup> : Eng3D::Deser::SerializerMemcpy<PopGroup> {};

struct PopType : RefnameEntity<PopTypeId> {
    Eng3D::StringRef name;
    float social_value = 1.f;
    enum PopGroup group = PopGroup::OTHER;
    std::vector<float> basic_needs_amount; // Amount of goods needed to satisfy basic needs
    std::vector<float> luxury_needs_satisfaction; // Amount of satisfaction each luxury good gives
    std::vector<float> luxury_needs_deminishing_factor; // Deminishing returns factor of the luxury good satisfaction
};
template<>
struct Eng3D::Deser::Serializer<PopType> {
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, PopType>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.social_value);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.group);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.basic_needs_amount);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.luxury_needs_satisfaction);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.luxury_needs_deminishing_factor);
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

    PopTypeId type_id;
    float size = 1.f;
    float life_needs_met = 0.f;
    float literacy = 0.f;
    float militancy = 0.f;
    float budget = 0.f;
    Policies wanted_policies;
};
template<>
struct Eng3D::Deser::Serializer<Pop> {
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, Pop>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.type_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.size);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.literacy);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.militancy);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.budget);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.life_needs_met);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.wanted_policies);
    }
};
