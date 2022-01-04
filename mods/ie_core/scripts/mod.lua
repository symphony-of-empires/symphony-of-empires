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
--  	mod.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

math.randomseed(os.time())

require('events/catholic_relief_act')
require('events/cholera_spread')
require('events/dysentery_spread')
require('events/gevhdl_001')
require('events/gevhdl_002')
require('events/gevhdl_003')
require('events/gevhdl_004')
--require('events/panama_canal')
require('events/kalmar_union')
require('events/minas_gerais_crash')
require('events/test_event')
require('events/taza_concession')
require('events/unify_germany')

--invalid_shit()

set_date(1825, 0, 0)
print("********************************************************************************")
print("Industrial era mod")
print("Date: ", get_year(), "/", get_month(), "/", get_day())
print("********************************************************************************")
