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
--  	pop_types.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Pop types

standard_basic_needs = {
    { "wheat", 1.0 },
    { "salt", 0.1 }
}

standard_luxury_needs = {
    { "bread", 0.5, 0.8 }
}

burgeoise = PopType:new{
    name = _("Burgeoise"),
    ref_name = "burgeoise",
    social_value = 2.0,
    is_burgeoise = true,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
burgeoise:register()

artisan = PopType:new{
    name = _("Artisan"),
    ref_name = "artisan",
    social_value = 1.0,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
artisan:register()

bureaucrat = PopType:new{
    name = _("Bureaucrat"),
    ref_name = "bureaucrat",
    social_value = 2.5,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
bureaucrat:register()

intellectual = PopType:new{
    name = _("Clergymen"),
    ref_name = "intellectual",
    social_value = 2.5,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
intellectual:register()

soldier = PopType:new{
    name = _("Soldier"),
    ref_name = "soldier",
    social_value = 1.5,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
soldier:register()

farmer = PopType:new{
    name = _("Farmer"),
    ref_name = "farmer",
    social_value = 1.0,
    is_farmer = true,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
farmer:register()

laborer = PopType:new{
    name = _("Laborer"),
    ref_name = "laborer",
    social_value = 1.0,
    is_laborer = true,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
laborer:register()

slave = PopType:new{
    name = _("Slave"),
    ref_name = "slave",
    social_value = 0.1,
    is_slave = true,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
slave:register()

-- Depricated ??
officer = PopType:new{
    name = _("Officer"),
    ref_name = "officer",
    social_value = 1.5,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
officer:register()

craftsmen = PopType:new{
    name = _("Craftsmen"),
    ref_name = "craftsmen",
    social_value = 1.5,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
craftsmen:register()