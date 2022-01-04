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
--  	provinces/america/guianas.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: British Empire

province = Province:new{ ref_name = "essequibo", color = 0x99FFC8 }
province.name = _("Essequibo")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, british, protestant, 5000, 0.7)
province:add_pop(farmer, british, protestant, 5000, 0.1)
province:add_pop(soldier, british, protestant, 5000, 0.2)
province:add_pop(craftsmen, british, protestant, 5000, 0.2)
province:add_pop(bureaucrat, british, protestant, 5000, 0.7)
province:add_pop(burgeoise, british, protestant, 5000, 0.2)
province:add_pop(clergymen, british, protestant, 5000, 0.3)
province:add_pop(laborer, british, protestant, 5000, 0.2)
province:add_pop(burgeoise, british, protestant, 500, 0.9)
province:add_nucleus(united_kingdom)
province:give_to(united_kingdom)

province = Province:new{ ref_name = "demerara", color = 0xcc0a37 }
province.name = _("Demerara")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, british, protestant, 5000, 0.7)
province:add_pop(farmer, british, protestant, 5000, 0.1)
province:add_pop(soldier, british, protestant, 5000, 0.2)
province:add_pop(craftsmen, british, protestant, 5000, 0.2)
province:add_pop(bureaucrat, british, protestant, 5000, 0.7)
province:add_pop(burgeoise, british, protestant, 5000, 0.2)
province:add_pop(clergymen, british, protestant, 5000, 0.3)
province:add_pop(laborer, british, protestant, 5000, 0.2)
province:add_pop(burgeoise, british, protestant, 500, 0.9)
province:add_nucleus(united_kingdom)
province:give_to(united_kingdom)

--if taken by suriname rename it to corantijn
province = Province:new{ ref_name = "corentyne", color = 0x56ffa5 }
province.name = _("Corentyne")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, british, protestant, 5000, 0.7)
province:add_pop(farmer, british, protestant, 5000, 0.1)
province:add_pop(soldier, british, protestant, 5000, 0.2)
province:add_pop(craftsmen, british, protestant, 5000, 0.2)
province:add_pop(bureaucrat, british, protestant, 5000, 0.7)
province:add_pop(burgeoise, british, protestant, 5000, 0.2)
province:add_pop(clergymen, british, protestant, 5000, 0.3)
province:add_pop(laborer, british, protestant, 5000, 0.2)
province:add_pop(burgeoise, british, protestant, 500, 0.9)
province:add_nucleus(united_kingdom)
province:give_to(united_kingdom)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Kingdom of the Netherlands

province = Province:new{ ref_name = "suriname", color = 0xFF42AD }
province.name = _("Suriname")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(netherlands)
province:give_to(netherlands)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: France

province = Province:new{ ref_name = "cayenne", color = 0x552804 }
province.name = _("Cayenne")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(france)
province:give_to(france)

-- if taken by suriname, rename it to Marowijne
province = Province:new{ ref_name = "maroni", color = 0xe7ffa5 }
province.name = _("Maroni")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, german, protestant, 5000, 0.7)
province:add_pop(farmer, german, protestant, 5000, 0.1)
province:add_pop(soldier, german, protestant, 5000, 0.2)
province:add_pop(craftsmen, german, protestant, 5000, 0.2)
province:add_pop(bureaucrat, german, protestant, 5000, 0.7)
province:add_pop(burgeoise, german, protestant, 5000, 0.2)
province:add_pop(clergymen, german, protestant, 5000, 0.3)
province:add_pop(laborer, german, protestant, 5000, 0.2)
province:add_pop(burgeoise, german, protestant, 500, 0.9)
province:add_pop(slave, german, protestant, 500, 0.5)
province:add_nucleus(france)
province:add_nucleus(netherlands)
province:give_to(france)
