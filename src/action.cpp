#include "action.hpp"
#include "io_impl.hpp"
#include "unified_render/serializer.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "unified_render/network.hpp"
#include "product.hpp"
#include "building.hpp"
#include "unit.hpp"
#include "nation.hpp"
#include "province.hpp"

using namespace Action;

Packet DiploIncRelations::form_packet(Nation* target) {
    Packet packet = Packet();
    Archive ar = Archive();
    ActionType action = ActionType::DIPLO_INC_RELATIONS;
    ::serialize(ar, &action);
    ::serialize(ar, &target);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Packet DiploDecRelations::form_packet(Nation* target) {
    Packet packet = Packet();
    Archive ar = Archive();
    ActionType action = ActionType::DIPLO_DEC_RELATIONS;
    ::serialize(ar, &action);
    ::serialize(ar, &target);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Packet DiploDeclareWar::form_packet(Nation* target) {
    Packet packet = Packet();
    Archive ar = Archive();
    ActionType action = ActionType::DIPLO_DECLARE_WAR;
    ::serialize(ar, &action);
    ::serialize(ar, &target);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Packet ProductUpdate::form_packet(const std::vector<Product*>& list) {
    Packet packet = Packet();
    Archive ar = Archive();
    ActionType action = ActionType::PRODUCT_UPDATE;
    ::serialize(ar, &action);
    for(const auto& product : list) {
        ::serialize(ar, &product); // ProductRef
        ::serialize(ar, product); // ProductObj
    }
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Packet ProvinceUpdate::form_packet(const std::vector<Province*>& list) {
    Packet packet = Packet();
    Archive ar = Archive();
    ActionType action = ActionType::PROVINCE_UPDATE;
    ::serialize(ar, &action);
    for(const auto& province : list) {
        ::serialize(ar, &province); // ProvinceRef
        ::serialize(ar, province); // ProvinceObj
    }
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Packet NationUpdate::form_packet(const std::vector<Nation*>& list) {
    Packet packet = Packet();
    Archive ar = Archive();
    ActionType action = ActionType::NATION_UPDATE;
    ::serialize(ar, &action);
    for(const auto& nation : list) {
        ::serialize(ar, &nation); // NationRef
        ::serialize(ar, nation); // NationObj
    }
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Packet SelectNation::form_packet(Nation* nation) {
    Packet packet = Packet();
    Archive ar = Archive();
    ActionType action = ActionType::SELECT_NATION;
    ::serialize(ar, &action);
    ::serialize(ar, &nation);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Packet BuildingStartProducingUnit::form_packet(Building* building, UnitType* unit_type) {
    Packet packet = Packet();
    Archive ar = Archive();
    ActionType action = ActionType::BUILDING_START_BUILDING_UNIT;
    ::serialize(ar, &action);
    ::serialize(ar, &building);
    ::serialize(ar, &unit_type);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}

Packet BuildingAdd::form_packet(Building* building) {
    Packet packet = Packet();
    Archive ar = Archive();
    ActionType action = ActionType::BUILDING_ADD;
    ::serialize(ar, &action);
    ::serialize(ar, building);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}