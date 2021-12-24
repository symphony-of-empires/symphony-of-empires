#pragma once

#include "client/ui.hpp"
#include "client/interface/common.hpp"

class World;
class Nation;
class GameState;
class Technology;

namespace Interface {
    class TechTreeStrategicTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeStrategicTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreeMilitaryTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeMilitaryTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreeNavyTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeNavyTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreeSocialTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeSocialTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreeEconomicTab : public UI::Group {
        GameState& gs;
    public:
        TechTreeEconomicTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreePoliticsTab : public UI::Group {
        GameState& gs;
    public:
        TechTreePoliticsTab(GameState& gs, int x, int y, UI::Widget* parent);
    };

    class TechTreeView : public UI::Window {
        GameState& gs;
    public:
        TechTreeView(GameState& gs);

        TechTreeStrategicTab* strategic_tab;
        TechTreeMilitaryTab* military_tab;
        TechTreeNavyTab* navy_tab;
        TechTreeSocialTab* social_tab;
        TechTreeEconomicTab* economic_tab;
        TechTreePoliticsTab* politics_tab;
    };
};
