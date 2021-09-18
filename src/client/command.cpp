#include "command.hpp"

#include "../actions.hpp"
#include "../diplomacy.hpp"
#include "../nation.hpp"
#include "../network.hpp"
#include "../province.hpp"
#include "../serializer.hpp"
#include "client_network.hpp"
#include "io_impl.hpp"

void SelectNationCommand::run_command(World& ws, Client* client){

};

void ColonizeProvinceCommand::run_command(World& ws, Client* client){

};

void DescisionCommand::run_command(World& ws, Client* client) {
    client->packet_mutex.lock();
    Packet packet = Packet(g_client->get_fd());
    Archive ar = Archive();
    enum ActionType action = ACTION_NATION_TAKE_DESCISION;
    ::serialize(ar, &action);
    ::serialize(ar, &event.ref_name);
    ::serialize(ar, &descision.ref_name);
    packet.data(ar.get_buffer(), ar.size());
    g_client->packet_queue.push_back(packet);
    g_client->packet_mutex.unlock();
};

void TreatyAcceptCommand::run_command(World& ws, Client* client) {
    client->packet_mutex.lock();
    Packet packet = Packet(g_client->get_fd());
    Archive ar = Archive();
    enum ActionType action = ACTION_CHANGE_TREATY_APPROVAL;
    // ::serialize(ar, &action);
    // ::serialize(ar, treaty);
    // enum TreatyApproval approval = accepts ? TREATY_APPROVAL_ACCEPTED : TREATY_APPROVAL_DENIED;
    // ::serialize(ar, &approval);
    packet.data(ar.get_buffer(), ar.size());
    client->packet_queue.push_back(packet);
    client->packet_mutex.unlock();
};

void TreatySendCommand::run_command(World& ws, Client* client) {
    client->packet_mutex.lock();
    Packet packet = Packet(g_client->get_fd());
    Archive ar = Archive();
    enum ActionType action = ACTION_DRAFT_TREATY;
    ::serialize(ar, &action);
    ::serialize(ar, &treaty->clauses);  // ClausesRefList
    ::serialize(ar, &treaty->name);     // StringObj
    ::serialize(ar, &curr_nation);      // Sender - NationRef
    packet.data(ar.get_buffer(), ar.size());
    client->packet_queue.push_back(packet);
    client->packet_mutex.unlock();
};

void BuildUnitCommand::run_command(World& ws, Client* client) {
    client->packet_mutex.lock();
    Packet packet = Packet(g_client->get_fd());
    Archive ar = Archive();
    if (unitType != nullptr) {
        enum ActionType action = ACTION_OUTPOST_START_BUILDING_UNIT;
        ::serialize(ar, &action);

        ::serialize(ar, &outpost);  // OutpostRef
        ::serialize(ar, unitType);  // UnitTypeRef
    } else {
        enum ActionType action = ACTION_OUTPOST_START_BUILDING_BOAT;
        ::serialize(ar, &action);

        ::serialize(ar, &outpost);  // OutpostRef
        ::serialize(ar, boatType);  // BoatTypeRef
    }
    packet.data(ar.get_buffer(), ar.size());
    client->packet_queue.push_back(packet);
    client->packet_mutex.unlock();
};