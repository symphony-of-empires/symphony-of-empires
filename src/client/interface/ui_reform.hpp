#ifndef UI_REFORM_H
#define UI_REFORM_H

#include "../render/texture.hpp"
#include "../../policy.hpp"
#include "../game_state.hpp"

class UIReform {
   public:
    UIReform(GameState& gs);
	GameState& gs;
    void show();

    const UnifiedRender::Texture *checkbox_ppv_on, *checkbox_ppv_off;
    Policies draft_policy;
};

#endif