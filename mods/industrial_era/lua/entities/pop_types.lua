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
    { "rice", 0.5 },
    { "coffee", 0.1 },
    { "bread", 0.5, 0.8 }
}

standard_luxury_needs = {
    { "arms", 0.9, 0.4 },
    { "wine", 0.5, 0.8 },
    { "spices", 0.3, 0.8 }
}

burgeoise = PopType:new{
    name = _("Burgeoise"),
    ref_name = "burgeoise",
    social_value = 1.0,
    is_burgeoise = true,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
burgeoise:register()

artisan = PopType:new{
    name = _("Artisan"),
    ref_name = "artisan",
    social_value = 0.5,
    is_artisan = true,
    basic_needs = {
        { "timber", 1.0 },
        { "silk", 1.0 },
        { "wheat", 1.0 },
        { "rice", 0.8 },
        { "coffee", 0.1 },
        { "bread", 0.5, 0.8 },
    },
    luxury_needs = standard_luxury_needs
}
artisan:register()

bureaucrat = PopType:new{
    name = _("Bureaucrat"),
    ref_name = "bureaucrat",
    social_value = 1.0,
    is_bureaucrat = true,
    basic_needs = standard_basic_needs,
    luxury_needs = standard_luxury_needs
}
bureaucrat:register()

soldier = PopType:new{
    name = _("Soldier"),
    ref_name = "soldier",
    social_value = 0.3,
    is_soldier = true,
    basic_needs = {
        { "arms", 1.0 },
        { "wheat", 1.0 },
        { "rice", 0.8 },
        { "coffee", 0.1 },
        { "bread", 0.5, 0.8 }
    },
    luxury_needs = standard_luxury_needs
}
soldier:register()

laborer = PopType:new{
    name = _("Laborer"),
    ref_name = "laborer",
    social_value = 0.3,
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
