#pragma once

#include "client/ui/group.hpp"

class GameState;
namespace Interface {
    class TopWindow : public UI::Group {
        GameState& gs;
    public:
        TopWindow(GameState& gs);

    };
    class TimeControlView : public UI::Group {
        GameState& gs;
    public:
        TimeControlView(GameState& gs);

    };
};