#ifndef COMMAND_H
#define COMMAND_H

#include "../actions.hpp"
#include "../nation.hpp"
#include "../province.hpp"
#include "../world.hpp"
#include "client_network.hpp"
#include "../diplomacy.hpp"

class Command {
   public:
    Action* action;
    virtual ~Command(){};
    // Change world state and send command to server
    virtual void run_command(World& ws, Client* client) = 0;
};

class SelectNationCommand : public Command {
   public:
    SelectNationCommand(Nation::Id nation) : id{nation} {};
    virtual ~SelectNationCommand(){};
    void run_command(World& ws, Client* client) override;

   private:
    Nation::Id id;
};

class ColonizeProvinceCommand : public Command {
   public:
    ColonizeProvinceCommand(Province::Id province) : id{province} {};
    virtual ~ColonizeProvinceCommand(){};
    void run_command(World& ws, Client* client) override;

   private:
    Province::Id id;
};

class DescisionCommand : public Command {
   public:
    DescisionCommand(const Descision& _descision, Event& _event) : descision{_descision}, event{_event} {};
    virtual ~DescisionCommand(){};
    void run_command(World& ws, Client* client) override;

   private:
    const Descision& descision;
    Event& event;
};

class TreatySendCommand : public Command {
   public:
    TreatySendCommand(Treaty* _treaty, Nation* _curr_nation) : treaty{_treaty}, curr_nation{_curr_nation} {};
    virtual ~TreatySendCommand(){};
    void run_command(World& ws, Client* client) override;

   private:
    Treaty* treaty;
    Nation* curr_nation;
};

class TreatyAcceptCommand : public Command {
   public:
    TreatyAcceptCommand(Treaty* _treaty, bool _accepts) : treaty{_treaty}, accepts{_accepts} {};
    virtual ~TreatyAcceptCommand(){};
    void run_command(World& ws, Client* client) override;

   private:
    Treaty* treaty;
    bool accepts;
};

class BuildUnitCommand : public Command {
   public:
    BuildUnitCommand(Building* _building, UnitType* _unitType) : building{_building}, unitType{_unitType} {};
    BuildUnitCommand(Building* _building, BoatType* _boatType) : building{_building}, boatType{_boatType} {};
    virtual ~BuildUnitCommand(){};
    void run_command(World& ws, Client* client) override;

   private:
    Building* building;
    UnitType* unitType = nullptr;
    BoatType* boatType = nullptr;
};

#endif