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
--  	industry_types.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Industry types

-- Primary sector industries (the most essential)
oil_refinery = IndustryType:new{ ref_name = "oil_refinery", name = _("Oil refinery") }
oil_refinery:register()
-- thin air oil, i think the air needs some democracy!
oil_refinery:add_output(Good:get("oil"))

lumberjack = IndustryType:new{ ref_name = "lumberjack", name = _("Lumberjack") }
lumberjack:register()
-- same logic here
lumberjack:add_output(Good:get("timber"))

vineyard = IndustryType:new{ ref_name = "vineyard", name = _("Vineyard") }
vineyard:register()
vineyard:add_output(Good:get("grapes"))

wheat_farm = IndustryType:new{ ref_name = "wheat_farm", name = _("Wheat farm") }
wheat_farm:register()
wheat_farm:add_output(Good:get("wheat"))

rice_farm = IndustryType:new{ ref_name = "rice_farm", name = _("Rice farm") }
rice_farm:register()
rice_farm:add_output(Good:get("rice"))

cotton_farm = IndustryType:new{ ref_name = "cotton_farm", name = _("Cotton farm") }
cotton_farm:register()
cotton_farm:add_output(Good:get("cotton"))

-- Mines
coal_mine = IndustryType:new{ ref_name = "coal_mine", name = _("Coal mine") }
coal_mine:register()
coal_mine:add_output(Good:get("coal"))

iron_mine = IndustryType:new{ ref_name = "iron_mine", name = _("Iron mine") }
iron_mine:register()
iron_mine:add_output(Good:get("iron"))

gold_mine = IndustryType:new{ ref_name = "gold_mine", name = _("Gold mine") }
gold_mine:register()
gold_mine:add_output(Good:get("gold"))

sulphur_mine = IndustryType:new{ ref_name = "sulphur_mine", name = _("Sulfur mine") }
sulphur_mine:register()
sulphur_mine:add_output(Good:get("sulphur"))

-- Secondary sector industries
arms_factory = IndustryType:new{ ref_name = "arms_factory", name = _("Arms factory") }
arms_factory:register()
arms_factory:add_input(Good:get("iron"))
arms_factory:add_input(Good:get("sulphur"))
arms_factory:add_output(Good:get("arms"))

winery = IndustryType:new{ ref_name = "winery", name = _("Winery") }
winery:register()
winery:add_input(Good:get("grapes"))
winery:add_output(Good:get("wine"))

-- Food factories
bakery = IndustryType:new{ ref_name = "bakery", name = _("Bakery") }
bakery:register()
bakery:add_input(Good:get("wheat"))
bakery:add_output(Good:get("bread"))