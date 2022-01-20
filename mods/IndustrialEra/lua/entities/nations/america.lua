-- Industrial_Era_mod - Base game files for Symphony of Empires
-- Copyright (C) 2021, Symphony of Empires contributors
-- 
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License along
-- with this program; if not, write to the Free Software Foundation, Inc.,
-- 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
--
-- ----------------------------------------------------------------------------
--	Name:
--  	lua/nations/america.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- America
-- Initial nations
argentina = Nation:new{ ref_name = "argentina" }
argentina.name = _("Argentina")
argentina.adjective = "Argentine"
argentina:register()
argentina:set_ideology(republic)
argentina:add_accepted_culture(argetine)
argentina:add_client_hint(monarchy, _("Argentina"), 0xace3e1)
argentina:add_client_hint(republic, _("Argentina"), 0xace3e1)

brazil = Nation:new{ ref_name = "brazil" }
brazil.name = _("Brazil")
brazil.adjective = "Brazilian"
brazil:register()
brazil:set_ideology(monarchy)
brazil:add_accepted_culture(brazilian)
brazil:add_client_hint(monarchy, _("Brazil"), 0x60c483)

gran_colombia = Nation:new{ ref_name = "gran_colombia" }
gran_colombia.name = _("Gran colombia")
gran_colombia.adjective = "Colombian"
gran_colombia:register()
gran_colombia:set_ideology(republic)
gran_colombia:add_accepted_culture(colombian)
gran_colombia:add_accepted_culture(venezuelan)
gran_colombia:add_accepted_culture(ecuadorian)
gran_colombia:add_client_hint(monarchy, _("Gran colombia"), 0xe6e6b5)
gran_colombia:add_client_hint(republic, _("Gran colombia"), 0xe6e6b5)

peru = Nation:new{ ref_name = "peru" }
peru.name = _("Peru")
peru.adjective = "Peruvian"
peru:register()
peru:set_ideology(republic)
peru:add_accepted_culture(peruvian)
peru:add_client_hint(monarchy, _("Peru"), 0xe63d2e)
peru:add_client_hint(republic, _("Peru"), 0xe63d2e)

bolivia = Nation:new{ ref_name = "bolivia" }
bolivia.name = _("Bolivia")
bolivia.adjective = _("Bolivian")
bolivia:register()
bolivia:set_ideology(republic)
bolivia:add_accepted_culture(bolivian)
bolivia:add_client_hint(republic, _("Bolivia"), 0xded6ba)
bolivia:add_client_hint(monarchy, _("Bolivia"), 0xded6ba)

paraguay = Nation:new{ ref_name = "paraguay" }
paraguay.name = _("Paraguay")
paraguay.adjective = _("Paraguayan")
paraguay:register()
paraguay:set_ideology(republic)
paraguay:add_accepted_culture(paraguayan)
paraguay:add_client_hint(monarchy, _("Paraguay"), 0x808000)
paraguay:add_client_hint(republic, _("Paraguay"), 0x808000)

chile = Nation:new{ ref_name = "chile" }
chile.name = _("Chile")
chile.adjective = _("Chilean")
chile:register()
chile:set_ideology(republic)
chile:add_accepted_culture(Culture:get("chilean"))
chile:add_client_hint(monarchy, _("Chile"), 0xad6dcf)
chile:add_client_hint(republic, _("Chile"), 0xad6dcf)

upca = Nation:new{ ref_name = "upca" }
upca.name = _("United Provinces of Central America")
upca:register()
upca:set_ideology(republic)
upca:add_accepted_culture(mexican)
upca:add_client_hint(republic, _("United Provinces of Central America"), 0x5bb2cf)
upca:add_client_hint(communism, _("United Provinces of Central America"), 0x5bb2cf)
upca:add_client_hint(socialism, _("United Provinces of Central America"), 0x5bb2cf)

mexico = Nation:new{ ref_name = "mexico" }
mexico.name = _("Mexico")
mexico.adjective = _("Mexican")
mexico:register()
mexico:set_ideology(republic)
mexico:add_accepted_culture(mexican)
mexico:add_client_hint(republic, _("Mexico"), 0x5ac780)
mexico:add_client_hint(monarchy, _("Mexican Empire"), 0x5ac780)

usa = Nation:new{ ref_name = "usa" }
usa.name = _("United states")
usa.adjective = _("American")
usa.noun = _("American")
usa:register()
usa:set_ideology(democracy)
usa:add_accepted_culture(british)
usa:add_accepted_culture(Culture:get("american"))
usa:add_client_hint(democracy, _("United States of America"), 0x6ea5cc)
usa:add_client_hint(fascism, _("United States of America"), 0x6ea5cc)
usa:add_client_hint(monarchy, _("United States of America"), 0x6ea5cc)
usa:add_client_hint(socialism, _("United States of America"), 0x6ea5cc)
usa:add_client_hint(communism, _("United States of America"), 0x6ea5cc)

haiti = Nation:new{ ref_name = "haiti" }
haiti.name = _("Haiti")
haiti:register()
haiti:set_ideology(republic)
haiti:add_accepted_culture(haitian)
haiti:add_client_hint(republic, _("Haiti"), 0xb147cc)

mapuche = Nation:new{ ref_name = "mapuche" }
mapuche.name = _("Mapuche")
mapuche:register()
mapuche:set_ideology(tribal)
mapuche:add_accepted_culture(mapuche)
mapuche:add_client_hint(tribal, _("Mapuche"), 0x8c9ebd)

-- Releasable nations
texas = Nation:new{ ref_name = "texas" }
texas.name = _("Texas")
texas.adjective = _("Texan")
texas:register()
texas:add_accepted_culture(Culture:get("american"))
texas:add_client_hint(republic, _("Texas"), 0x8be2f7)
texas:add_client_hint(fascism, _("Texas"), 0x8be2f7)
texas:add_client_hint(socialism, _("Texas"), 0x8be2f7)
texas:add_client_hint(communism, _("Texas"), 0x8be2f7)
texas:add_client_hint(monarchy, _("Texas"), 0x8be2f7)

csa = Nation:new{ ref_name = "csa" }
csa.name = _("Confederate States of America")
csa.adjective = _("Confederate")
csa:register()
csa:add_accepted_culture(Culture:get("american"))
csa:add_client_hint(republic, _("Confederate States of America"), 0x8be2f7)
csa:add_client_hint(monarchy, _("Confederate States of America"), 0x8be2f7)

uruguay = Nation:new{ ref_name = "uruguay" }
uruguay.name = _("Uruguay")
uruguay.adjective = _("Uruguayan")
uruguay:register()
uruguay:add_accepted_culture(paraguayan)
uruguay:add_client_hint(monarchy, _("Uruguay"), 0x808000)
uruguay:add_client_hint(republic, _("Uruguay"), 0x808000)

panama = Nation:new{ ref_name = "panama" }
panama.name = _("Panama")
panama.adjective = _("Panaman")
panama:register()
panama:add_accepted_culture(paraguayan)
panama:add_client_hint(monarchy, _("Panama"), 0x808000)
panama:add_client_hint(republic, _("Panama"), 0x808000)
--------------------------------------------------------------