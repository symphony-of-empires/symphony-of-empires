--------------------------------------------------------------
-- America
-- Initial nations
argentina = Nation:create{ ref_name = "argentina" }
argentina.name = _("Argentina")
Nation:register(argentina)
Nation:add_accepted_culture(argentina, argentinian)
Nation:add_client_hint(argentina, communism, _("Argentina"), 0x808000)

brazil = Nation:create{ ref_name = "brazil" }
brazil.name = _("Brazil")
Nation:register(brazil)
Nation:add_accepted_culture(brazil, brazilian)
Nation:add_client_hint(brazil, communism, _("Brazil"), 0x808000)

gran_colombia = Nation:create{ ref_name = "gran_colombia" }
gran_colombia.name = _("Gran colombia")
Nation:register(gran_colombia)
Nation:add_accepted_culture(gran_colombia, colombian)
Nation:add_accepted_culture(gran_colombia, venezuelan)
Nation:add_accepted_culture(gran_colombia, ecuadorian)
Nation:add_client_hint(gran_colombia, communism, _("Gran colombia"), 0x808000)

peru = Nation:create{ ref_name = "peru" }
peru.name = _("Peru")
Nation:register(peru)
Nation:add_accepted_culture(peru, peruvian)
Nation:add_client_hint(peru, communism, _("Peru"), 0x808000)

bolivia = Nation:create{ ref_name = "bolivia" }
bolivia.name = _("Bolivia")
Nation:register(bolivia)
Nation:add_accepted_culture(bolivia, bolivian)
Nation:add_client_hint(bolivia, communism, _("Bolivia"), 0x808000)

paraguay = Nation:create{ ref_name = "paraguay" }
paraguay.name = _("Paraguay")
Nation:register(paraguay)
Nation:add_accepted_culture(paraguay, paraguayan)
Nation:add_client_hint(paraguay, communism, _("Paraguay"), 0x808000)

chile = Nation:create{ ref_name = "chile" }
chile.name = _("Chile")
Nation:register(chile)
Nation:add_accepted_culture(chile, chilean)
Nation:add_client_hint(chile, communism, _("Chile"), 0x808000)

federated_republic_central_america = Nation:create{ ref_name = "federated_republic_central_america" }
federated_republic_central_america.name = _("Federated Republic of Central America")
Nation:register(federated_republic_central_america)
Nation:add_accepted_culture(federated_republic_central_america, mexican)
Nation:add_client_hint(federated_republic_central_america, communism, _("Federated Republic of Central America"), 0x808000)

mexico = Nation:create{ ref_name = "mexico" }
mexico.name = _("Mexico")
Nation:register(mexico)
Nation:add_accepted_culture(mexico, mexican)
Nation:add_client_hint(mexico, communism, _("Mexico"), 0x808000)

united_states = Nation:create{ ref_name = "united_states" }
united_states.name = _("United states")
Nation:register(united_states)
Nation:add_accepted_culture(united_states, british)
Nation:add_accepted_culture(united_states, yankee)
Nation:add_client_hint(united_states, communism, _("United states"), 0x808000)

haiti = Nation:create{ ref_name = "haiti" }
haiti.name = _("Haiti")
Nation:register(haiti)
Nation:add_accepted_culture(haiti, haitian)
Nation:add_client_hint(haiti, communism, _("Haiti"), 0x808000)

-- Releasable nations
texas = Nation:create{ ref_name = "texas" }
texas.name = _("Texas")
Nation:register(texas)
Nation:add_accepted_culture(texas, yankee)
Nation:add_client_hint(texas, communism, _("Texas"), 0x808000)
--------------------------------------------------------------