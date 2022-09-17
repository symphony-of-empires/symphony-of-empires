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
//      action.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/serializer.hpp"
#include "eng3d/network.hpp"

#include "action.hpp"
#include "io_impl.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "product.hpp"
#include "building.hpp"
#include "unit.hpp"
#include "nation.hpp"
#include "province.hpp"

using namespace Action;

template<ActionType type, typename Pred, typename ...Targs>
Eng3D::Networking::Packet ActionHandlerSR(Pred p) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, type);
    p(ar);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet AiControl::form_packet(const Nation& nation) {
    return ActionHandlerSR<ActionType::AI_CONTROL>([&nation](auto& ar) {
        ::serialize(ar, nation.ai_controlled);
        ::serialize(ar, nation.ai_do_cmd_troops);
        ::serialize(ar, nation.ai_controlled);
        ::serialize(ar, nation.ai_controlled);
        ::serialize(ar, nation.ai_controlled);
        ::serialize(ar, nation.ai_controlled);
        ::serialize(ar, nation.ai_controlled);
        ::serialize(ar, nation.ai_controlled);
    });
}

Eng3D::Networking::Packet DiploDeclareWar::form_packet(const Nation& nation) {
    return ActionHandlerSR<ActionType::DIPLO_DECLARE_WAR>([&nation](auto& ar) {
        ::serialize(ar, &nation);
    });
}

Eng3D::Networking::Packet ProvinceUpdate::form_packet(const std::vector<Province>& list) {
    return ActionHandlerSR<ActionType::PROVINCE_UPDATE>([&list](auto& ar) {
        for(const auto& province : list) {
            auto& ref = province;
            ::serialize(ar, &ref); // ProvinceRef
            ::serialize(ar, ref); // ProvinceObj
        }
    });
}

Eng3D::Networking::Packet NationUpdate::form_packet(const std::vector<Nation>& list) {
    return ActionHandlerSR<ActionType::NATION_UPDATE>([&list](auto& ar) {
        for(const auto& nation : list) {
            auto& ref = nation;
            ::serialize(ar, &ref); // NationRef
            ::serialize(ar, ref); // NationObj
        }
    });
}

Eng3D::Networking::Packet SelectNation::form_packet(const Nation& nation) {
    return ActionHandlerSR<ActionType::SELECT_NATION>([&nation](auto& ar) {
        ::serialize(ar, &nation);
    });
}

Eng3D::Networking::Packet BuildingStartProducingUnit::form_packet(const Province& province, const BuildingType& building_type, const Nation& nation, const UnitType& unit_type) {
    return ActionHandlerSR<ActionType::SELECT_NATION>([&](auto& ar) {
        ::serialize(ar, &province);
        ::serialize(ar, &building_type);
        ::serialize(ar, &nation);
        ::serialize(ar, &unit_type);
    });
}

Eng3D::Networking::Packet BuildingAdd::form_packet(const Province& province, const BuildingType& building_type) {
    return ActionHandlerSR<ActionType::BUILDING_ADD>([&province, &building_type](auto& ar) {
        ::serialize(ar, &province);
        ::serialize(ar, &building_type);
    });
}

Eng3D::Networking::Packet FocusTech::form_packet(const Technology& technology) {
    return ActionHandlerSR<ActionType::FOCUS_TECH>([&](auto& ar) {
        ::serialize(ar, &technology);
    });
}

Eng3D::Networking::Packet NationAdd::form_packet(const Nation& nation) {
    return ActionHandlerSR<ActionType::NATION_ADD>([&](auto& ar) {
        ::serialize(ar, nation);
    });
}

Eng3D::Networking::Packet NationTakeDecision::form_packet(const Event& event, const Decision& decision) {
    return ActionHandlerSR<ActionType::NATION_TAKE_DECISION>([&](auto& ar) {
        ::serialize(ar, event);
        ::serialize(ar, decision.ref_name);
    });
}

Eng3D::Networking::Packet UnitAdd::form_packet(const Unit& unit) {
    return ActionHandlerSR<ActionType::UNIT_ADD>([&](auto& ar) {
        ::serialize(ar, unit);
        ::serialize<Province::Id>(ar, unit.province_id());
    });
}

Eng3D::Networking::Packet UnitUpdate::form_packet(const std::vector<Unit>& units) {
    return ActionHandlerSR<ActionType::UNIT_ADD>([&](auto& ar) {
        ::serialize<Unit::Id>(ar, units.size());
        for(const auto& unit : units)
            ::serialize(ar, unit);
    });
}

Eng3D::Networking::Packet UnitRemove::form_packet(const Unit& unit) {
    return ActionHandlerSR<ActionType::UNIT_ADD>([&](auto& ar) {
        ::serialize<Unit::Id>(ar, unit.get_id());
    });
}

Eng3D::Networking::Packet UnitMove::form_packet(const Unit& unit, const Province& province) {
    return ActionHandlerSR<ActionType::UNIT_ADD>([&](auto& ar) {
        ::serialize<Unit::Id>(ar, unit.get_id());
        ::serialize<Province::Id>(ar, province.get_id());
    });
}
