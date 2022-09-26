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

class Ideology : public RefnameEntity<IdeologyId> {
public:
    std::uint32_t color;
    Eng3D::StringRef name;
    Policies policies;
    // Checks that the policies of a given nation are "matching"
    // with our ideology (so we set eyecandy and stuff)
    Eng3D::StringRef check_policies_fn;
};
template<>
class Serializer<Ideology*>: public SerializerReferenceLocal<World, Ideology> {};
template<>
class Serializer<const Ideology*>: public SerializerReferenceLocal<World, const Ideology> {};
template<>
class Serializer<Ideology> {
public:
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Ideology& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.color);
        ::deser_dynamic<is_serialize>(ar, obj.check_policies_fn);
    }
};
