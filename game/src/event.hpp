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
// along with this program.  If not, see <https://www.gnu.org/licenses/>.-----------------
// Name:
//      event.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

#include "eng3d/string.hpp"
#include "objects.hpp"

struct Decision : RefnameEntity<DecisionId> {
    Eng3D::StringRef name;
    Eng3D::StringRef effects;

    /// @todo Reload these when serializing
    int do_decision_function = 0;
};
template<>
struct Serializer<Decision> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Decision& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.do_decision_function);
        ::deser_dynamic<is_serialize>(ar, obj.effects);
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
    bool checked = false;

    /// @todo Reload these when serializing
    int conditions_function = 0;
    int do_event_function = 0;
};
template<>
struct Serializer<Event*> : SerializerReference<World, Event> {};
template<>
struct Serializer<const Event*> : SerializerReference<World, const Event> {};
template<>
struct Serializer<Event> {
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, Event& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.receiver_ids);
        ::deser_dynamic<is_serialize>(ar, obj.decisions);
        ::deser_dynamic<is_serialize>(ar, obj.title);
        ::deser_dynamic<is_serialize>(ar, obj.text);
        ::deser_dynamic<is_serialize>(ar, obj.conditions_function);
        ::deser_dynamic<is_serialize>(ar, obj.do_event_function);
    }
};
