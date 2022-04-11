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
--  	technology.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Mechanical theory series
mechanical_theory_i = Technology:new{
    ref_name = "mechanical_theory_i",
    name = "Mechanical theory I",
    description = "Relying on natural sources such as the wind, they will be the stepping stone for future developments",
    cost = 1000.0,
    type = TECH_ECONOMIC
}
mechanical_theory_i:register()

mechanical_theory_ii = Technology:new{
    ref_name = "mechanical_theory_ii",
    name = "Mechanical theory II",
    description = "The boom in science will help more complex machinery to rise",
    cost = 180000.0,
    type = TECH_ECONOMIC
}
mechanical_theory_ii:register()
mechanical_theory_ii:requires_technology(mechanical_theory_i)

mechanical_theory_iii = Technology:new{
    ref_name = "mechanical_theory_iii",
    name = "Mechanical theory III",
    description = "With the use of power generators it's possible to automate the movement of machines",
    cost = 180000.0,
    type = TECH_ECONOMIC
}
mechanical_theory_iii:register()
mechanical_theory_iii:requires_technology(mechanical_theory_ii)

voltaic_pile = Technology:new{
    ref_name = "voltaic_pile",
    name = _("Voltaic pile"),
    description = "An early battery",
    cost = 15000.0,
    type = TECH_ECONOMIC
}
voltaic_pile:register()

combustion_engine = Technology:new{
    ref_name = "combustion_engine",
    name = "Combustion engine",
    description = "",
    cost = 50000.0,
    type = TECH_ECONOMIC
}
combustion_engine:register()

steam_locomotive = Technology:new{
    ref_name = "steam_locomotive",
    name = "Steam locomotive",
    description = "",
    cost = 100000.0,
    type = TECH_ECONOMIC
}
steam_locomotive:register()

assembly_line = Technology:new{
    ref_name = "assembly_line",
    name = "Assembly line",
    description = "",
    cost = 100000.0,
    type = TECH_ECONOMIC
}
assembly_line:register()
assembly_line:requires_technology(mechanical_theory_iii)

napoleonic_tought = Technology:new{
    ref_name = "napoleonic_tought",
    name = "Napoleonic tought",
    description = "",
    cost = 10000.0,
    type = TECH_STRATEGIC
}
napoleonic_tought:register()

trench_warfare = Technology:new{
    ref_name = "trench_warfare",
    name = "Trench warfare",
    description = "Develop efficient tactics to create defensive frontlines making offensives very costly",
    cost = 150000.0,
    type = TECH_STRATEGIC
}
trench_warfare:register()

guerrilla_warfare = Technology:new{
    ref_name = "guerrilla_warfare",
    name = "Guerrilla warfare",
    description = "Splitting the army to allow quick-attacks and small raids that weaken and distract units and slow down their movement",
    cost = 150000.0,
    type = TECH_STRATEGIC
}
guerrilla_warfare:register()

-- Social technologies
romanticism = Technology:new{
    ref_name = "romanticism",
    name = "Romanticism",
    description = "...",
    cost = 150000.0,
    type = TECH_SOCIAL
}
romanticism:register()

-- Military and naval tech
gunpowder = Technology:new{
    ref_name = "gunpowder",
    name = "Gunpowder",
    description = "...",
    cost = 150000.0,
    type = TECH_MILITARY
}
gunpowder:register()

flintlock_rifles = Technology:new{
    ref_name = "flintlock_rifles",
    name = "Flintlock Rifles",
    description = "...",
    cost = 150000.0,
    type = TECH_MILITARY
}
flintlock_rifles:register()
flintlock_rifles:requires_technology(gunpowder)

light_vessel = Technology:new{
    ref_name = "light_vessel",
    name = "Light Vessel",
    description = "...",
    cost = 150000.0,
    type = TECH_NAVAL
}
light_vessel:register()

heavy_vessel = Technology:new{
    ref_name = "heavy_vessel",
    name = "Heavy Vessel",
    description = "...",
    cost = 150000.0,
    type = TECH_NAVAL
}
heavy_vessel:register()
heavy_vessel:requires_technology(light_vessel)
