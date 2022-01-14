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
--  	nations.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Nations
rebel = Nation:new{ ref_name = "rebel" }
rebel.name = _("Rebel")
rebel:register()
rebel:add_client_hint(monarchy, _("Monarchist Rebellion"), 0x808000)
rebel:add_client_hint(communism, _("Communist Rebellion"), 0x808000)
rebel:add_client_hint(socialism, _("Socialist Rebellion"), 0x808000)
rebel:add_client_hint(fascism, _("Fascist Rebellion"), 0x808000)
rebel:add_client_hint(republic, _("Democratic Rebellion"), 0x808000)
rebel:add_client_hint(anarchy, _("Anarchist Rebellion"), 0x808000)
rebel:add_client_hint(tribal, _("Primitivism Rebellion"), 0x808000)

hawai_i = Nation:new{ ref_name = "hawai_i" }
hawai_i.name = _("Hawai'i")
hawai_i:register()
hawai_i:add_accepted_culture(polynesian)
hawai_i:add_client_hint(monarchy, _("Hawai'i"), 0x808000)
hawai_i:add_client_hint(communism, _("Hawai'i"), 0x808000)
hawai_i:add_client_hint(socialism, _("Hawai'i"), 0x808000)
hawai_i:add_client_hint(fascism, _("Hawai'i"), 0x808000)
hawai_i:add_client_hint(republic, _("Hawai'i"), 0x808000)

require('nations/africa')
require('nations/america')
require('nations/asia')
require('nations/europe')