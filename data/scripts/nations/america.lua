-- Initial nations
argentina = Nation:create{ ref_name = "argentina", color = 0x00a9ff, default_flag = "flags/argentina_republic.png" }
argentina.name = _("Argentina")
Nation:register(argentina)
Nation:add_accepted_culture(argentina, argentinian)
brazil = Nation:create{ ref_name = "brazil", color = 0x4CFF00, default_flag = "flags/brazil_monarchy.png" }
brazil.name = _("Brazil")
Nation:register(brazil)
Nation:add_accepted_culture(brazil, brazilian)
gran_colombia = Nation:create{ ref_name = "gran_colombia", color = 0xE1FF91, default_flag = "flags/gran_colombia_republic.png" }
gran_colombia.name = _("Gran colombia")
Nation:register(gran_colombia)
Nation:add_accepted_culture(gran_colombia, colombian)
Nation:add_accepted_culture(gran_colombia, venezuelan)
Nation:add_accepted_culture(gran_colombia, ecuadorian)
peru = Nation:create{ ref_name = "peru", color = 0xFF2158, default_flag = "flags/peru_republic.png" }
peru.name = _("Peru")
Nation:register(peru)
Nation:add_accepted_culture(peru, peruvian)
bolivia = Nation:create{ ref_name = "bolivia", color = 0xffc4ee, default_flag = "flags/bolivia_republic.png" }
bolivia.name = _("Bolivia")
Nation:register(bolivia)
Nation:add_accepted_culture(bolivia, bolivian)
paraguay = Nation:create{ ref_name = "paraguay", color = 0xb71cff, default_flag = "flags/paraguay_republic.png" }
paraguay.name = _("Paraguay")
Nation:register(paraguay)
Nation:add_accepted_culture(paraguay, paraguayan)
chile = Nation:create{ ref_name = "chile", color = 0x241f3d, default_flag = "flags/chile_republic.png" }
chile.name = _("Chile")
Nation:register(chile)
Nation:add_accepted_culture(chile, chilean)
federated_republic_central_america = Nation:create{ ref_name = "federated_republic_central_america", color = 0x5199ff, default_flag = "flags/federated_republic_central_america_republic.png" }
federated_republic_central_america.name = _("Federated Republic of Central America")
Nation:register(federated_republic_central_america)
Nation:add_accepted_culture(federated_republic_central_america, mexican)
mexico = Nation:create{ ref_name = "mexico", color = 0x92FF60, default_flag = "flags/mexico_republic.png" }
mexico.name = _("Mexico")
Nation:register(mexico)
Nation:add_accepted_culture(mexico, mexican)
united_states = Nation:create{ ref_name = "united_states", color = 0x306bd1, default_flag = "flags/united_states_democracy.png" }
united_states.name = _("United states")
Nation:register(united_states)
Nation:add_accepted_culture(united_states, british)
Nation:add_accepted_culture(united_states, yankee)
haiti = Nation:create{ ref_name = "haiti", color = 0x4c2a9c, default_flag = "flags/unknown.png" }
haiti.name = _("Haiti")
Nation:register(haiti)
Nation:add_accepted_culture(haiti, haitian)