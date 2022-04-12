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
--  	good_types.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Good types
-- Farmable stuff
wheat = Good:new{ ref_name = "wheat" }
wheat.name = _("Wheat")
wheat:register()
grapes = Good:new{ ref_name = "grapes" }
grapes.name = _("Grapes")
grapes:register()
rice = Good:new{ ref_name = "rice" }
rice.name = _("Rice")
rice:register()
cotton = Good:new{ ref_name = "cotton" }
cotton.name = _("Cotton")
cotton:register()
coffee = Good:new{ ref_name = "coffee" }
coffee.name = _("Coffe bean")
coffee:register()

-- Industrial materials
fuel = Good:new{ ref_name = "fuel" }
fuel.name = _("Fuel")
fuel:register()
silk = Good:new{ ref_name = "silk" }
silk.name = _("Silk")
silk:register()
glass = Good:new{ ref_name = "glass" }
glass.name = _("Glass")
glass:register()
fabric = Good:new{ ref_name = "fabric" }
fabric.name = _("Fabric")
fabric:register()
dye = Good:new{ ref_name = "dye" }
dye.name = _("Dye")
dye:register()

-- Military stuff
arms = Good:new{ ref_name = "arms" }
arms.name = _("Arms")
arms:register()
ammunition = Good:new{ ref_name = "ammunition" }
ammunition.name = _("Ammunition")
ammunition:register()
sail = Good:new{ ref_name = "sail" }
sail.name = _("Sail")
sail:register()

-- Minerals
copper = Good:new{ ref_name = "copper" }
copper.name = _("Copper")
copper:register()
gold = Good:new{ ref_name = "gold" }
gold.name = _("Gold")
gold:register()
iron = Good:new{ ref_name = "iron" }
iron.name = _("Iron")
iron:register()
oil = Good:new{ ref_name = "oil" }
oil.name = _("Oil")
oil:register()
timber = Good:new{ ref_name = "timber" }
timber.name = _("Timber")
timber:register()
coal = Good:new{ ref_name = "coal" }
coal.name = _("Coal")
coal:register()
sulphur = Good:new{ ref_name = "sulphur" }
sulphur.name = _("Sulfur")
sulphur:register()
steel = Good:new{ ref_name = "steel" }
steel.name = _("Steel bean")
steel:register()

-- Food
bread = Good:new{ ref_name = "bread" }
bread.name = _("Bread")
bread:register()
salt = Good:new{ ref_name = "salt" }
salt.name = _("Salt")
salt:register()

-- Commodities
wine = Good:new{ ref_name = "wine" }
wine.name = _("Wine")
wine:register()
alcohol = Good:new{ ref_name = "alcohol" }
alcohol.name = _("Alcohol")
alcohol:register()
opium = Good:new{ ref_name = "opium" }
opium.name = _("Opium")
opium:register()
spices = Good:new{ ref_name = "spices" }
spices.name = _("Spices")
spices:register()
