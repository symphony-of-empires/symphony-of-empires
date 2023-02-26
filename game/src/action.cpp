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
//      action.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/serializer.hpp"
#include "eng3d/network.hpp"

#include "action.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "world.hpp"

namespace Action {
template<ActionType type, typename Pred, typename ...Targs>
Eng3D::Networking::Packet action_handler_sr(Pred p) {
    Eng3D::Networking::Packet packet{};
    Eng3D::Deser::Archive ar{};
    Eng3D::Deser::serialize<ActionType>(ar, type);
    p(ar);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet DiploDeclareWar::form_packet(const Nation& nation) {
    return action_handler_sr<ActionType::DIPLO_DECLARE_WAR>([&nation](auto& ar) {
        Eng3D::Deser::serialize(ar, nation.get_id());
    });
}

Eng3D::Networking::Packet ProvinceUpdate::form_packet(const std::vector<Province>& list) {
    return action_handler_sr<ActionType::PROVINCE_UPDATE>([&list](auto& ar) {
        for(const auto& province : list) {
            Eng3D::Deser::serialize(ar, province.get_id()); // ProvinceRef
            Eng3D::Deser::serialize(ar, province); // ProvinceObj
        }
    });
}

Eng3D::Networking::Packet NationUpdate::form_packet(const std::vector<Nation>& list) {
    return action_handler_sr<ActionType::NATION_UPDATE>([&list](auto& ar) {
        for(const auto& nation : list) {
            Eng3D::Deser::serialize(ar, nation.get_id()); // NationRef
            Eng3D::Deser::serialize(ar, nation); // NationObj
        }
    });
}

Eng3D::Networking::Packet SelectNation::form_packet(const Nation& nation) {
    return action_handler_sr<ActionType::SELECT_NATION>([&nation](auto& ar) {
        Eng3D::Deser::serialize(ar, nation.get_id());
        Eng3D::Deser::serialize(ar, nation.ai_do_cmd_troops);
        Eng3D::Deser::serialize(ar, nation.ai_controlled);
    });
}

Eng3D::Networking::Packet BuildingStartProducingUnit::form_packet(const Province& province, const BuildingType& building_type, const Nation& nation, const UnitType& unit_type) {
    return action_handler_sr<ActionType::BUILDING_START_BUILDING_UNIT>([&](auto& ar) {
        Eng3D::Deser::serialize(ar, province.get_id());
        Eng3D::Deser::serialize(ar, building_type.get_id());
        Eng3D::Deser::serialize(ar, nation.get_id());
        Eng3D::Deser::serialize(ar, unit_type.get_id());
    });
}

Eng3D::Networking::Packet BuildingAdd::form_packet(const Province& province, const BuildingType& building_type) {
    return action_handler_sr<ActionType::BUILDING_ADD>([&province, &building_type](auto& ar) {
        Eng3D::Deser::serialize(ar, province.get_id());
        Eng3D::Deser::serialize(ar, building_type.get_id());
    });
}

Eng3D::Networking::Packet FocusTech::form_packet(const Technology& technology) {
    return action_handler_sr<ActionType::FOCUS_TECH>([&](auto& ar) {
        Eng3D::Deser::serialize(ar, technology.get_id());
    });
}

Eng3D::Networking::Packet NationTakeDecision::form_packet(const Event& event, const Decision& decision) {
    return action_handler_sr<ActionType::NATION_TAKE_DECISION>([&](auto& ar) {
        Eng3D::Deser::serialize(ar, event);
        Eng3D::Deser::serialize(ar, decision.ref_name);
    });
}

Eng3D::Networking::Packet UnitAdd::form_packet(const Unit& unit) {
    return action_handler_sr<ActionType::UNIT_ADD>([&](auto& ar) {
        Eng3D::Deser::serialize(ar, unit);
        Eng3D::Deser::serialize<ProvinceId>(ar, unit.province_id());
    });
}

Eng3D::Networking::Packet UnitUpdate::form_packet(const Eng3D::Freelist<Unit>& units) {
    return action_handler_sr<ActionType::UNIT_UPDATE>([&](auto& ar) {
        size_t size = 0;
        units.for_each([&](const auto&) {
            size++;
        });
        Eng3D::Deser::serialize<UnitId>(ar, UnitId(size));
        units.for_each([&](const auto& unit) {
            Eng3D::Deser::serialize(ar, unit);
        });
    });
}

Eng3D::Networking::Packet UnitRemove::form_packet(const Unit& unit) {
    return action_handler_sr<ActionType::UNIT_REMOVE>([&](auto& ar) {
        Eng3D::Deser::serialize<UnitId>(ar, unit.get_id());
    });
}

Eng3D::Networking::Packet UnitMove::form_packet(const Unit& unit, const Province& province) {
    return action_handler_sr<ActionType::UNIT_CHANGE_TARGET>([&](auto& ar) {
        Eng3D::Deser::serialize<UnitId>(ar, unit.get_id());
        Eng3D::Deser::serialize<ProvinceId>(ar, province.get_id());
    });
}
}  // namespace Action
