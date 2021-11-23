--------------------------------------------------------------
-- America
-- Initial nations
argentina = Nation:new{ ref_name = "argentina" }
argentina.name = _("Argentina")
argentina.adjective = "Argentine"
argentina:register()
argentina:set_ideology(republic)
argentina:add_accepted_culture(argentinian)
argentina:add_client_hint(republic, _("Argentina"), 0xace3e1)

brazil = Nation:new{ ref_name = "brazil" }
brazil.name = _("Brazil")
brazil.adjective = "Brazilian"
brazil:register()
brazil:set_ideology(republic)
brazil:add_accepted_culture(brazilian)
brazil:add_client_hint(republic, _("Brazil"), 0x60c483)

gran_colombia = Nation:new{ ref_name = "gran_colombia" }
gran_colombia.name = _("Gran colombia")
gran_colombia.adjective = "Colombian"
gran_colombia:register()
gran_colombia:set_ideology(republic)
gran_colombia:add_accepted_culture(colombian)
gran_colombia:add_accepted_culture(venezuelan)
gran_colombia:add_accepted_culture(ecuadorian)
gran_colombia:add_client_hint(republic, _("Gran colombia"), 0xe6e6b5)

peru = Nation:new{ ref_name = "peru" }
peru.name = _("Peru")
peru.adjective = "Peruvian"
peru:register()
peru:set_ideology(republic)
peru:add_accepted_culture(peruvian)
peru:add_client_hint(republic, _("Peru"), 0xe63d2e)

bolivia = Nation:new{ ref_name = "bolivia" }
bolivia.name = _("Bolivia")
bolivia.adjective = _("Bolivian")
bolivia:register()
bolivia:set_ideology(republic)
bolivia:add_accepted_culture(bolivian)
bolivia:add_client_hint(republic, _("Bolivia"), 0xded6ba)

paraguay = Nation:new{ ref_name = "paraguay" }
paraguay.name = _("Paraguay")
paraguay.adjective = _("Paraguayan")
paraguay:register()
paraguay:set_ideology(republic)
paraguay:add_accepted_culture(paraguayan)
paraguay:add_client_hint(republic, _("Paraguay"), 0x808000)

chile = Nation:new{ ref_name = "chile" }
chile.name = _("Chile")
chile.adjective = _("Chilean")
chile:register()
chile:set_ideology(republic)
chile:add_accepted_culture(chilean)
chile:add_client_hint(republic, _("Chile"), 0xad6dcf)

upca = Nation:new{ ref_name = "upca" }
upca.name = _("UPCA")
upca:register()
upca:set_ideology(republic)
upca:add_accepted_culture(mexican)
upca:add_client_hint(republic, _("UPCA"), 0x5bb2cf)

mexico = Nation:new{ ref_name = "mexico" }
mexico.name = _("Mexico")
mexico.adjective = _("Mexican")
mexico:register()
mexico:set_ideology(republic)
mexico:add_accepted_culture(mexican)
mexico:add_client_hint(republic, _("Mexico"), 0x5ac780)
mexico:add_client_hint(monarchy, _("Mexican Empire"), 0x5ac780)

united_states = Nation:new{ ref_name = "united_states" }
united_states.name = _("United states")
united_states.adjective = _("American")
united_states.noun = _("American")
united_states:register()
united_states:set_ideology(democracy)
united_states:add_accepted_culture(british)
united_states:add_accepted_culture(yankee)
united_states:add_client_hint(democracy, _("United states"), 0x6ea5cc)

haiti = Nation:new{ ref_name = "haiti" }
haiti.name = _("Haiti")
haiti:register()
haiti:set_ideology(democracy)
haiti:add_accepted_culture(haitian)
haiti:add_client_hint(democracy, _("Haiti"), 0xb147cc)

-- Releasable nations
texas = Nation:new{ ref_name = "texas" }
texas.name = _("Texas")
texas.adjective = _("Texan")
texas:register()
texas:add_accepted_culture(yankee)
texas:add_client_hint(democracy, _("Texas"), 0x8be2f7)
--------------------------------------------------------------