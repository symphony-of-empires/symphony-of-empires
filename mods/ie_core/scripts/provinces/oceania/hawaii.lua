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
--  	provinces/oceania/hawaii.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- Continent: Oceania
-- Subcontinent: Polynesia
-- Country: Hawai'i
province = Province:new{ ref_name = "papahānaumokuākea", color = 0xcea3ff }
province.name = _("Papahānaumokuākea")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, polynesian, protestant, 10, 0.6)
province:add_pop(farmer, polynesian, protestant, 8000, 0.4)
province:add_pop(soldier, polynesian, protestant, 10, 0.6)
province:add_pop(craftsmen, polynesian, protestant, 10, 0.4)
province:add_pop(bureaucrat, polynesian, protestant, 10, 0.8)
province:add_pop(burgeoise, polynesian, protestant, 25, 0.5)
province:add_pop(clergymen, polynesian, protestant, 10, 0.6)
province:add_pop(laborer, polynesian, protestant, 1000, 0.5)
province:add_pop(burgeoise, polynesian, protestant, 5, 0.9)
province:add_nucleus(hawai_i)
province:give_to(hawai_i)
province:set_terrain(plains)

province = Province:new{ ref_name = "kaua_i", color = 0x0f6bff }
province.name = _("Kaua'i")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, polynesian, protestant, 10, 0.6)
province:add_pop(farmer, polynesian, protestant, 8000, 0.4)
province:add_pop(soldier, polynesian, protestant, 10, 0.6)
province:add_pop(craftsmen, polynesian, protestant, 10, 0.4)
province:add_pop(bureaucrat, polynesian, protestant, 10, 0.8)
province:add_pop(burgeoise, polynesian, protestant, 25, 0.5)
province:add_pop(clergymen, polynesian, protestant, 10, 0.6)
province:add_pop(laborer, polynesian, protestant, 1000, 0.5)
province:add_pop(burgeoise, polynesian, protestant, 5, 0.9)
province:add_nucleus(hawai_i)
province:give_to(hawai_i)
province:set_terrain(plains)

province = Province:new{ ref_name = "o_ahu_maui", color = 0xff7247 }
province.name = _("O'ahu-Maui")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, polynesian, protestant, 10, 0.6)
province:add_pop(farmer, polynesian, protestant, 8000, 0.4)
province:add_pop(soldier, polynesian, protestant, 10, 0.6)
province:add_pop(craftsmen, polynesian, protestant, 10, 0.4)
province:add_pop(bureaucrat, polynesian, protestant, 10, 0.8)
province:add_pop(burgeoise, polynesian, protestant, 25, 0.5)
province:add_pop(clergymen, polynesian, protestant, 10, 0.6)
province:add_pop(laborer, polynesian, protestant, 1000, 0.5)
province:add_pop(burgeoise, polynesian, protestant, 5, 0.9)
province:add_nucleus(hawai_i)
province:give_to(hawai_i)
province:set_terrain(plains)

-- Whoever did this please fix!!!!
province = Province:new{ ref_name = "hawai_i", color = 0xff54a1 }
province.name = _("Hawai'i")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, polynesian, protestant, 10, 0.6)
province:add_pop(farmer, polynesian, protestant, 8000, 0.4)
province:add_pop(soldier, polynesian, protestant, 10, 0.6)
province:add_pop(craftsmen, polynesian, protestant, 10, 0.4)
province:add_pop(bureaucrat, polynesian, protestant, 10, 0.8)
province:add_pop(burgeoise, polynesian, protestant, 25, 0.5)
province:add_pop(clergymen, polynesian, protestant, 10, 0.6)
province:add_pop(laborer, polynesian, protestant, 1000, 0.5)
province:add_pop(burgeoise, polynesian, protestant, 5, 0.9)
province:add_nucleus(hawai_i)
province:give_to(hawai_i)
province:set_terrain(plains)
hawai_i:set_capital(province)