#include "action.hpp"
#include "io_impl.hpp"
#include "serializer.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "network.hpp"
#include "product.hpp"
#include "building.hpp"
#include "unit.hpp"

using namespace Action;

void ProductUpdate::send(std::vector<Product*>* list) {

}

void SelectNation::send(Nation* nation) {
    std::scoped_lock lock(g_client->packet_mutex);

    Packet packet = Packet();
    Archive ar = Archive();

    ActionType action = ActionType::SELECT_NATION;
    ::serialize(ar, &action);

    ::serialize(ar, nation);

    packet.data(ar.get_buffer(), ar.size());
    g_client->packet_queue.push_back(packet);
}

void BuildingStartProducingUnit::send(Building* building, UnitType* unit_type) {
    std::scoped_lock lock(g_client->packet_mutex);

    Packet packet = Packet();
    Archive ar = Archive();

    ActionType action = ActionType::BUILDING_START_BUILDING_UNIT;
    ::serialize(ar, &action);

    ::serialize(ar, &building);
    ::serialize(ar, unit_type);
    
    packet.data(ar.get_buffer(), ar.size());
    g_client->packet_queue.push_back(packet);
}