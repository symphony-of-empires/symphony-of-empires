#include <cstdio>
#include <mutex>
#include <string>

#include "world.hpp"
#include "economy.hpp"
#include "path.hpp"
#include "network.hpp"
#include "serializer.hpp"
#include "io_impl.hpp"

extern World* g_world;
Nation* selected_nation;

void ai_fix_taxes(void) {
    std::lock_guard<std::recursive_mutex> lock(g_world->provinces_mutex);

    double living_std = 0.f;
    for(const auto& province: selected_nation->owned_provinces) {
        for(const auto& pop: province->pops) {
            living_std += pop.life_needs_met;
        }
    }

    // We did it, we fixed the world economy
    Policies draft_policy = selected_nation->current_policy;
    draft_policy.poor_flat_tax = 0.f;
    draft_policy.med_flat_tax = 0.f;
    draft_policy.rich_flat_tax = 0.f;

    g_client->packet_mutex.lock();
    Packet packet = Packet();
    Archive ar = Archive();
    enum ActionType action = ACTION_NATION_ENACT_POLICY;
    ::serialize(ar, &action);
    ::serialize(ar, &draft_policy); // PoliciesObj
    packet.data(ar.get_buffer(), ar.size());
    g_client->packet_queue.push_back(packet);
    g_client->packet_mutex.unlock();

    print_info("[AI] Enacting tax policy for le %s", selected_nation->name.c_str());
}

void ai_main(void) {
    print_info("Hello!, I'm SimpleAI");

    // Tells the server about our chosen nation
    selected_nation = g_world->nations[0];
    g_client->packet_mutex.lock();
    Packet packet = Packet();
    Archive ar = Archive();
    enum ActionType action = ACTION_SELECT_NATION;
    ::serialize(ar, &action);
    ::serialize(ar, &selected_nation);
    packet.data(ar.get_buffer(), ar.size());
    g_client->packet_queue.push_back(packet);
    g_client->packet_mutex.unlock();
    print_info("[AI] Selecting %s", selected_nation->name.c_str());

    while(1) {
        ai_fix_taxes();
    }
}