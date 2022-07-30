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
--  	lua/nations/africa.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-----------------------------------------------------------------------
-- North Africa
-- Initial nations
tunis = Nation:new{ ref_name = "tunis" }
tunis.name = _("Tunis")
tunis.adjective = _("Tunisian")
tunis:register()
tunis:set_ideology(monarchy)
tunis:add_accepted_culture(arabic)
tunis:add_client_hint(monarchy, _("Tunis"), 0xAC433A)

algiers = Nation:new{ ref_name = "algiers" }
algiers.name = _("Algiers")
algiers.adjective = _("Algerian")
algiers:register()
algiers:set_ideology(monarchy)
algiers:add_accepted_culture(arabic)
algiers:add_client_hint(monarchy, _("Algiers"), 0x509095)

abbyssina = Nation:new{ ref_name = "abbyssina" }
abbyssina.name = _("Abbyssina")
abbyssina:register()
abbyssina:set_ideology(monarchy)
abbyssina:add_client_hint(monarchy, _("Abbyssina"), 0x4761C2)

egypt = Nation:new{ ref_name = "egypt" }
egypt.name = _("Egypt")
egypt.adjective = _("Egyptians")
egypt:register()
egypt:set_ideology(monarchy)
egypt:add_client_hint(monarchy, _("Egypt"), 0xC1BC73)

morrocco = Nation:new{ ref_name = "morrocco" }
morrocco.name = _("Morrocco")
morrocco:register()
morrocco:set_ideology(monarchy)
morrocco:add_client_hint(monarchy, _("Morrocco"), 0x802249)

tripolitania = Nation:new{ ref_name = "tripolitania" }
tripolitania.name = _("Tripolitania")
tripolitania.adjective = _("Tripolitanians")
tripolitania:register()
tripolitania:set_ideology(monarchy)
tripolitania:add_client_hint(monarchy, _("Tripolitania"), 0x91D38A)

ashantee = Nation:new{ ref_name = "ashantee" }
ashantee.name = _("Ashantee")
ashantee:register()
ashantee:add_client_hint(monarchy, _("Ashantee"), 0x9ba324)
-- Formable nations
arabia = Nation:new{ ref_name = "arabia" }
arabia.name = _("Arabia")
arabia.adjective = "Arabic"
arabia.noun = "Arab"
arabia:register()
arabia:add_accepted_culture(arabic)
arabia:add_client_hint(monarchy, _("Arabia"), 0x388713)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Western Africa
-- Initial nations
sokoto = Nation:new{ ref_name = "sokoto" }
sokoto.name = _("Sokoto")
sokoto:register()
sokoto:add_accepted_culture(arabic)
sokoto:add_client_hint(monarchy, _("Sokoto"), 0x135910)
sokoto:add_client_hint(fascism, _("Sokoto"), 0x135910)
sokoto:add_client_hint(communism, _("Sokoto"), 0x135910)
sokoto:add_client_hint(socialism, _("Sokoto"), 0x135910)

wadai_empire = Nation:new{ ref_name = "wadai_empire" }
wadai_empire.name = _("Wadai Empire")
wadai_empire:register()
wadai_empire:add_client_hint(monarchy, _("Wadai Empire"), 0x808000)

bornu = Nation:new{ ref_name = "bornu" }
bornu.name = _("Bornu")
bornu:register()
bornu:add_client_hint(monarchy, _("Bornu"), 0x58d69b)

jolof = Nation:new{ ref_name = "jolof" }
jolof.name = _("Jolof")
jolof:register()
jolof:add_client_hint(monarchy, _("Jolof"), 0xdce30e)

dahomey = Nation:new{ ref_name = "dahomey" }
dahomey.name = _("Dahomey")
dahomey:register()
dahomey:add_client_hint(monarchy, _("Dahomey"), 0x3f9bd1)
-----------------------------------------------------------------------