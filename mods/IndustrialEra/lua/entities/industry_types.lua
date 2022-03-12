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
glass_factory = IndustryType:new{ ref_name = "glass_factory", name = _("Glass factory") }
glass_factory:register()
-- glass factories can new glass from thin air
glass_factory:add_output(glass)

oil_refinery = IndustryType:new{ ref_name = "oil_refinery", name = _("Oil refinery") }
oil_refinery:register()
-- thin air oil, i think the air needs some democracy!
oil_refinery:add_output(oil)

lumberjack = IndustryType:new{ ref_name = "lumberjack", name = _("Lumberjack") }
lumberjack:register()
-- same logic here
lumberjack:add_output(timber)

vineyard = IndustryType:new{ ref_name = "vineyard", name = _("Vineyard") }
vineyard:register()
vineyard:add_output(grapes)

wheat_farm = IndustryType:new{ ref_name = "wheat_farm", name = _("Wheat farm") }
wheat_farm:register()
wheat_farm:add_output(wheat)

rice_farm = IndustryType:new{ ref_name = "rice_farm", name = _("Rice farm") }
rice_farm:register()
rice_farm:add_output(rice)

cotton_farm = IndustryType:new{ ref_name = "cotton_farm", name = _("Cotton farm") }
cotton_farm:register()
cotton_farm:add_output(cotton)

-- Mines can new stuff from thin air, but don't worry because this is
-- not abuse-ble
coal_mine = IndustryType:new{ ref_name = "coal_mine", name = _("Coal mine") }
coal_mine:register()
coal_mine:add_output(coal)

copper_mine = IndustryType:new{ ref_name = "copper_mine", name = _("Copper mine") }
copper_mine:register()
copper_mine:add_output(copper)

iron_mine = IndustryType:new{ ref_name = "iron_mine", name = _("Iron mine") }
iron_mine:register()
iron_mine:add_output(iron)

gold_mine = IndustryType:new{ ref_name = "gold_mine", name = _("Gold mine") }
gold_mine:register()
gold_mine:add_output(gold)

sulfur_mine = IndustryType:new{ ref_name = "sulfur_mine", name = _("Sulfur mine") }
sulfur_mine:register()
sulfur_mine:add_output(sulfur)

-- Secondary sector industries (now the fun begins)
iron_smelter = IndustryType:new{ ref_name = "iron_smelter", name = _("Iron smelter") }
iron_smelter:register()
iron_smelter:add_input(iron)
iron_smelter:add_input(coal)
iron_smelter:add_output(steel)

ammunition_factory = IndustryType:new{ ref_name = "ammunition_factory", name = _("Ammunition factory") }
ammunition_factory:register()
ammunition_factory:add_input(sulfur)
ammunition_factory:add_input(coal)
ammunition_factory:add_input(steel)
ammunition_factory:add_output(ammunition)

arms_factory = IndustryType:new{ ref_name = "arms_factory", name = _("Arms factory") }
arms_factory:register()
arms_factory:add_input(timber)
arms_factory:add_input(ammunition)
arms_factory:add_output(arms)

sails_factory = IndustryType:new{ ref_name = "sails_factory", name = _("Sails factory") }
sails_factory:register()
sails_factory:add_input(dye)
sails_factory:add_input(timber)
sails_factory:add_input(fabric)
sails_factory:add_output(sail)

-- Food factories
bakery = IndustryType:new{ ref_name = "bakery", name = _("Bakery") }
bakery:register()
bakery:add_input(wheat)
bakery:add_output(bread)