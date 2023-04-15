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
oil_refinery:add_input(Commodity:get("kpi"))
oil_refinery:add_output(Commodity:get("oil"))

lumberjack = IndustryType:new{ ref_name = "lumberjack", name = _("Lumberjack") }
lumberjack:register()
-- same logic here
lumberjack:add_input(Commodity:get("kpi"))
lumberjack:add_output(Commodity:get("timber"))

fuel_industry = IndustryType:new{ ref_name = "fuel_industry", name = _("Fuel industry") }
fuel_industry:register()
fuel_industry:add_input(Commodity:get("kpi"))
fuel_industry:add_output(Commodity:get("fuel"))

wheat_farm = IndustryType:new{ ref_name = "wheat_farm", name = _("Wheat farm") }
wheat_farm:register()
wheat_farm:add_input(Commodity:get("kpi"))
wheat_farm:add_output(Commodity:get("wheat"))

rice_farm = IndustryType:new{ ref_name = "rice_farm", name = _("Rice farm") }
rice_farm:register()
rice_farm:add_input(Commodity:get("kpi"))
rice_farm:add_output(Commodity:get("rice"))

cotton_farm = IndustryType:new{ ref_name = "cotton_farm", name = _("Cotton farm") }
cotton_farm:register()
cotton_farm:add_input(Commodity:get("kpi"))
cotton_farm:add_output(Commodity:get("cotton"))

coffee_bean_farm = IndustryType:new{ ref_name = "coffee_bean_farm", name = _("Coffee bean farm") }
coffee_bean_farm:register()
coffee_bean_farm:add_input(Commodity:get("kpi"))
coffee_bean_farm:add_output(Commodity:get("coffee"))

-- Mines
coal_mine = IndustryType:new{ ref_name = "coal_mine", name = _("Coal mine") }
coal_mine:register()
coal_mine:add_input(Commodity:get("kpi"))
coal_mine:add_output(Commodity:get("coal"))

iron_mine = IndustryType:new{ ref_name = "iron_mine", name = _("Iron mine") }
iron_mine:register()
iron_mine:add_input(Commodity:get("kpi"))
iron_mine:add_output(Commodity:get("iron"))

gold_mine = IndustryType:new{ ref_name = "gold_mine", name = _("Gold mine") }
gold_mine:register()
gold_mine:add_input(Commodity:get("kpi"))
gold_mine:add_output(Commodity:get("gold"))

sulphur_mine = IndustryType:new{ ref_name = "sulphur_mine", name = _("Sulfur mine") }
sulphur_mine:register()
sulphur_mine:add_input(Commodity:get("kpi"))
sulphur_mine:add_output(Commodity:get("sulphur"))

-- Secondary sector industries
arms_industry = IndustryType:new{ ref_name = "arms_industry", name = _("Arms industry") }
arms_industry:register()
arms_industry:add_input(Commodity:get("iron"))
arms_industry:add_input(Commodity:get("sulphur"))
arms_industry:add_output(Commodity:get("arms"))

winery = IndustryType:new{ ref_name = "winery", name = _("Winery") }
winery:register()
winery:add_input(Commodity:get("kpi"))
winery:add_output(Commodity:get("wine"))

dye_industry = IndustryType:new{ ref_name = "dye_industry", name = _("Dye industry") }
dye_industry:register()
dye_industry:add_input(Commodity:get("kpi"))
dye_industry:add_output(Commodity:get("dye"))

-- Plantations
silk_plantation = IndustryType:new{ ref_name = "silk_plantation", name = _("Silk plantation") }
silk_plantation:register()
silk_plantation:add_input(Commodity:get("kpi"))
silk_plantation:add_output(Commodity:get("silk"))

spices_plantation = IndustryType:new{ ref_name = "spices_plantation", name = _("Spices plantation") }
spices_plantation:register()
spices_plantation:add_input(Commodity:get("kpi"))
spices_plantation:add_output(Commodity:get("spices"))

opium_plantation = IndustryType:new{ ref_name = "opium_plantation", name = _("Opium plantation") }
opium_plantation:register()
opium_plantation:add_input(Commodity:get("kpi"))
opium_plantation:add_output(Commodity:get("opium"))

-- Food factories
bakery = IndustryType:new{ ref_name = "bakery", name = _("Bakery") }
bakery:register()
bakery:add_input(Commodity:get("wheat"))
bakery:add_output(Commodity:get("bread"))


