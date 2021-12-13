#include "action.hpp"
#include "io_impl.hpp"
#include "unified_render/serializer.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "unified_render/network.hpp"
#include "product.hpp"
#include "building.hpp"
#include "unit.hpp"

using namespace Action;

void ProductUpdate::send(std::vector<Product*>* list) {

}

void SelectNation::send(Nation* nation) {
    Packet packet = Packet();
    Archive ar = Archive();

    ActionType action = ActionType::SELECT_NATION;
    ::serialize(ar, &action);

    ::serialize(ar, nation);

    packet.data(ar.get_buffer(), ar.size());
	
	std::scoped_lock lock(g_client->pending_packets_mutex);
    g_client->pending_packets.push_back(packet);
}

void BuildingStartProducingUnit::send(Building* building, UnitType* unit_type) {
    Packet packet = Packet();
    Archive ar = Archive();

    ActionType action = ActionType::BUILDING_START_BUILDING_UNIT;
    ::serialize(ar, &action);

    ::serialize(ar, &building);
    ::serialize(ar, &unit_type);
    
    packet.data(ar.get_buffer(), ar.size());
	
	std::scoped_lock lock(g_client->pending_packets_mutex);
    g_client->pending_packets.push_back(packet);
}

void BuildingAdd::send(Building* building) {
    Packet packet = Packet();
    Archive ar = Archive();

    ActionType action = ActionType::BUILDING_ADD;
    ::serialize(ar, &action);

    ::serialize(ar, building);
    
    packet.data(ar.get_buffer(), ar.size());
	
	std::scoped_lock lock(g_client->pending_packets_mutex);
    g_client->pending_packets.push_back(packet);
}