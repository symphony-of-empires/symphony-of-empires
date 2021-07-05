-- Initial nations
argentina = Nation:create{ ref_name = "argentina" }
argentina.name = _("Argentina")
Nation:register(argentina)
Nation:add_accepted_culture(argentina, argentinian)
brazil = Nation:create{ ref_name = "brazil" }
brazil.name = _("Brazil")
Nation:register(brazil)
Nation:add_accepted_culture(brazil, brazilian)
gran_colombia = Nation:create{ ref_name = "gran_colombia" }
gran_colombia.name = _("Gran colombia")
Nation:register(gran_colombia)
Nation:add_accepted_culture(gran_colombia, colombian)
Nation:add_accepted_culture(gran_colombia, venezuelan)
Nation:add_accepted_culture(gran_colombia, ecuadorian)
peru = Nation:create{ ref_name = "peru" }
peru.name = _("Peru")
Nation:register(peru)
Nation:add_accepted_culture(peru, peruvian)
bolivia = Nation:create{ ref_name = "bolivia" }
bolivia.name = _("Bolivia")
Nation:register(bolivia)
Nation:add_accepted_culture(bolivia, bolivian)
paraguay = Nation:create{ ref_name = "paraguay" }
paraguay.name = _("Paraguay")
Nation:register(paraguay)
Nation:add_accepted_culture(paraguay, paraguayan)
chile = Nation:create{ ref_name = "chile" }
chile.name = _("Chile")
Nation:register(chile)
Nation:add_accepted_culture(chile, chilean)
federated_republic_central_america = Nation:create{ ref_name = "federated_republic_central_america" }
federated_republic_central_america.name = _("Federated Republic of Central America")
Nation:register(federated_republic_central_america)
Nation:add_accepted_culture(federated_republic_central_america, mexican)
mexico = Nation:create{ ref_name = "mexico" }
mexico.name = _("Mexico")
Nation:register(mexico)
Nation:add_accepted_culture(mexico, mexican)
united_states = Nation:create{ ref_name = "united_states" }
united_states.name = _("United states")
Nation:register(united_states)
Nation:add_accepted_culture(united_states, british)
Nation:add_accepted_culture(united_states, yankee)
haiti = Nation:create{ ref_name = "haiti" }
haiti.name = _("Haiti")
Nation:register(haiti)
Nation:add_accepted_culture(haiti, haitian)