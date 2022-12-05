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
    Eng3D::StringRef extra_data; // Extra data for the decision
    /// @todo Reload these when serializing
    int do_decision_function = 0;
};
template<>
struct Eng3D::Deser::Serializer<Decision> {
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, Decision>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.effects);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.extra_data);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.do_decision_function);
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
    Eng3D::StringRef extra_data; // Extra data for the event
    bool checked = false;
    /// @todo Reload these when serializing
    int conditions_function = 0;
    int do_event_function = 0;
};
template<>
struct Eng3D::Deser::Serializer<::Event> {
    template<bool is_const>
    using type = Eng3D::Deser::CondConstType<is_const, ::Event>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Eng3D::Deser::Archive& ar, type<is_serialize>& obj) {
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.cached_id);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.ref_name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.name);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.receiver_ids);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.decisions);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.text);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.title);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.extra_data);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.checked);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.conditions_function);
        Eng3D::Deser::deser_dynamic<is_serialize>(ar, obj.do_event_function);
    }
};
