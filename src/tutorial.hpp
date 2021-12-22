#pragma once

struct TutorialState {
    // Tutorial will only be fired at the first opening
    int fire_at_start = 1;

    int ctl_intro = 1;
    int ctl_economy = 1;
    int ctl_diplomacy = 1;
    int ctl_policies = 1;
    int ctl_parliament = 1;
};
