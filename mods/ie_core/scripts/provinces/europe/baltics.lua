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
--  	provinces/europe/baltics.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Baltics
-- Country: Estonia
province = Province:new{ ref_name = "saare", color = 0xfff06d }
province.name = _("Saare")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, estonian, orthodox, 5000, 0.9)
province:add_pop(farmer, estonian, orthodox, 40000, 0.8)
province:add_pop(soldier, estonian, orthodox, 8000, 0.8)
province:add_pop(craftsmen, estonian, orthodox, 5000, 0.9)
province:add_pop(bureaucrat, estonian, orthodox, 5000, 0.9)
province:add_pop(burgeoise, estonian, orthodox, 5000, 0.9)
province:add_pop(clergymen, estonian, orthodox, 5000, 0.9)
province:add_pop(laborer, estonian, orthodox, 50000, 0.8)
province:add_pop(burgeoise, estonian, orthodox, 800, 0.9)
province:add_nucleus(estonia)
province:add_nucleus(russia)
province:give_to(russia)

province = Province:new{ ref_name = "hiiu", color = 0xff4fe4 }
province.name = _("Hiiu")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, estonian, orthodox, 5000, 0.9)
province:add_pop(farmer, estonian, orthodox, 40000, 0.8)
province:add_pop(soldier, estonian, orthodox, 8000, 0.8)
province:add_pop(craftsmen, estonian, orthodox, 5000, 0.9)
province:add_pop(bureaucrat, estonian, orthodox, 5000, 0.9)
province:add_pop(burgeoise, estonian, orthodox, 5000, 0.9)
province:add_pop(clergymen, estonian, orthodox, 5000, 0.9)
province:add_pop(laborer, estonian, orthodox, 50000, 0.8)
province:add_pop(burgeoise, estonian, orthodox, 800, 0.9)
province:add_nucleus(estonia)
province:add_nucleus(russia)
province:give_to(russia)

province = Province:new{ ref_name = "harju", color = 0x70ffe0 }
province.name = _("Harju")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, estonian, orthodox, 5000, 0.9)
province:add_pop(farmer, estonian, orthodox, 40000, 0.8)
province:add_pop(soldier, estonian, orthodox, 8000, 0.8)
province:add_pop(craftsmen, estonian, orthodox, 5000, 0.9)
province:add_pop(bureaucrat, estonian, orthodox, 5000, 0.9)
province:add_pop(burgeoise, estonian, orthodox, 5000, 0.9)
province:add_pop(clergymen, estonian, orthodox, 5000, 0.9)
province:add_pop(laborer, estonian, orthodox, 50000, 0.8)
province:add_pop(burgeoise, estonian, orthodox, 800, 0.9)
province:add_nucleus(estonia)
province:add_nucleus(russia)
province:give_to(russia)

province = Province:new{ ref_name = "laane", color = 0x3a7cff }
province.name = _("Lääne")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, estonian, orthodox, 5000, 0.9)
province:add_pop(farmer, estonian, orthodox, 40000, 0.8)
province:add_pop(soldier, estonian, orthodox, 8000, 0.8)
province:add_pop(craftsmen, estonian, orthodox, 5000, 0.9)
province:add_pop(bureaucrat, estonian, orthodox, 5000, 0.9)
province:add_pop(burgeoise, estonian, orthodox, 5000, 0.9)
province:add_pop(clergymen, estonian, orthodox, 5000, 0.9)
province:add_pop(laborer, estonian, orthodox, 50000, 0.8)
province:add_pop(burgeoise, estonian, orthodox, 800, 0.9)
province:add_nucleus(estonia)
province:add_nucleus(russia)
province:give_to(russia)

province = Province:new{ ref_name = "parnu", color = 0x5effae }
province.name = _("Pärnu")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, estonian, orthodox, 5000, 0.9)
province:add_pop(farmer, estonian, orthodox, 40000, 0.8)
province:add_pop(soldier, estonian, orthodox, 8000, 0.8)
province:add_pop(craftsmen, estonian, orthodox, 5000, 0.9)
province:add_pop(bureaucrat, estonian, orthodox, 5000, 0.9)
province:add_pop(burgeoise, estonian, orthodox, 5000, 0.9)
province:add_pop(clergymen, estonian, orthodox, 5000, 0.9)
province:add_pop(laborer, estonian, orthodox, 50000, 0.8)
province:add_pop(burgeoise, estonian, orthodox, 800, 0.9)
province:add_nucleus(estonia)
province:add_nucleus(russia)
province:give_to(russia)

province = Province:new{ ref_name = "viljandi", color = 0x8132ff }
province.name = _("Viljandi")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, estonian, orthodox, 5000, 0.9)
province:add_pop(farmer, estonian, orthodox, 40000, 0.8)
province:add_pop(soldier, estonian, orthodox, 8000, 0.8)
province:add_pop(craftsmen, estonian, orthodox, 5000, 0.9)
province:add_pop(bureaucrat, estonian, orthodox, 5000, 0.9)
province:add_pop(burgeoise, estonian, orthodox, 5000, 0.9)
province:add_pop(clergymen, estonian, orthodox, 5000, 0.9)
province:add_pop(laborer, estonian, orthodox, 50000, 0.8)
province:add_pop(burgeoise, estonian, orthodox, 800, 0.9)
province:add_nucleus(estonia)
province:add_nucleus(russia)
province:give_to(russia)

province = Province:new{ ref_name = "jarva", color = 0xff6b86 }
province.name = _("Järva")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_pop(artisan, estonian, orthodox, 5000, 0.9)
province:add_pop(farmer, estonian, orthodox, 40000, 0.8)
province:add_pop(soldier, estonian, orthodox, 8000, 0.8)
province:add_pop(craftsmen, estonian, orthodox, 5000, 0.9)
province:add_pop(bureaucrat, estonian, orthodox, 5000, 0.9)
province:add_pop(burgeoise, estonian, orthodox, 5000, 0.9)
province:add_pop(clergymen, estonian, orthodox, 5000, 0.9)
province:add_pop(laborer, estonian, orthodox, 50000, 0.8)
province:add_pop(burgeoise, estonian, orthodox, 800, 0.9)
province:add_nucleus(estonia)
province:add_nucleus(russia)
province:give_to(russia)
