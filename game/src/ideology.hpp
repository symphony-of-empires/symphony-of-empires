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
//      ideology.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <cstdint>
#include "objects.hpp"
#include "policy.hpp"

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
struct Serializer<Ideology::Subideology> {
    template<bool is_const>
    using type = CondConstType<is_const, Ideology::Subideology>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.economic);
        ::deser_dynamic<is_serialize>(ar, obj.political);
    }
};

template<>
struct Serializer<Ideology> {
    template<bool is_const>
    using type = CondConstType<is_const, Ideology>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.color);
        ::deser_dynamic<is_serialize>(ar, obj.subideologies);
    }
};
