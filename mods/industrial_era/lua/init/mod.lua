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

print("Hello!")

require('entities/events/catholic_relief_act')
require('entities/events/cholera_spread')
require('entities/events/dysentery_spread')
require('entities/events/gevhdl_001')
require('entities/events/gevhdl_002')
require('entities/events/gevhdl_003')
require('entities/events/gevhdl_004')
--require('entities/events/panama_canal')
require('entities/events/kalmar_union')
require('entities/events/minas_gerais_crash')
require('entities/events/test_event')
require('entities/events/taza_concession')
require('entities/events/unify_germany')

Nation:get("france"):make_puppet(Nation:get("piedmont_sardinia"))

Nation:get("turkey"):make_puppet(Nation:get("tunis"))
Nation:get("turkey"):make_puppet(Nation:get("algeirs"))
Nation:get("turkey"):make_puppet(Nation:get("egypt"))
Nation:get("turkey"):make_puppet(Nation:get("tripolitania"))

Nation:get("netherlands"):make_puppet(Nation:get("luxembourg"))

Nation:get("sweden"):make_puppet(Nation:get("norway"))

Nation:get("united_kingdom"):make_puppet(Nation:get("hannover"))
Nation:get("united_kingdom"):make_puppet(Nation:get("ruperts_land"))
Nation:get("united_kingdom"):make_puppet(Nation:get("lower_canada"))
Nation:get("united_kingdom"):make_puppet(Nation:get("upper_canada"))

Nation:get("russia"):make_puppet(Nation:get("finland"))
Nation:get("russia"):make_puppet(Nation:get("congress_poland"))

Nation:get("prussia"):make_puppet(Nation:get("salzburg"))
Nation:get("prussia"):make_puppet(Nation:get("thuringia"))
Nation:get("prussia"):make_puppet(Nation:get("lubeck"))
Nation:get("prussia"):make_puppet(Nation:get("waldeck"))
Nation:get("prussia"):make_puppet(Nation:get("hesse_kassel"))
Nation:get("prussia"):make_puppet(Nation:get("reuss_gera"))
Nation:get("prussia"):make_puppet(Nation:get("saxe_weimar"))
Nation:get("prussia"):make_puppet(Nation:get("saxe_gotha"))
Nation:get("prussia"):make_puppet(Nation:get("saxe_coburg"))
Nation:get("prussia"):make_puppet(Nation:get("saxe_hildburghausen"))
Nation:get("prussia"):make_puppet(Nation:get("saxe_meiningen"))
Nation:get("prussia"):make_puppet(Nation:get("lippe"))
Nation:get("prussia"):make_puppet(Nation:get("lorraine"))
Nation:get("prussia"):make_puppet(Nation:get("brandenburg"))
Nation:get("prussia"):make_puppet(Nation:get("frankfurt"))
Nation:get("prussia"):make_puppet(Nation:get("munich"))
Nation:get("prussia"):make_puppet(Nation:get("hamburg"))
Nation:get("prussia"):make_puppet(Nation:get("nassau"))
Nation:get("prussia"):make_puppet(Nation:get("baden"))
Nation:get("prussia"):make_puppet(Nation:get("brunswick"))
Nation:get("prussia"):make_puppet(Nation:get("anhalt"))
Nation:get("prussia"):make_puppet(Nation:get("oldenburg"))
Nation:get("prussia"):make_puppet(Nation:get("silesia"))
Nation:get("prussia"):make_puppet(Nation:get("danzig"))
Nation:get("prussia"):make_puppet(Nation:get("wolfenbuttel"))
Nation:get("prussia"):make_puppet(Nation:get("bremen"))
Nation:get("prussia"):make_puppet(Nation:get("schaumburg_lippe"))
Nation:get("prussia"):make_puppet(Nation:get("ritzebuttel"))
Nation:get("prussia"):make_puppet(Nation:get("liechtenstein"))
Nation:get("prussia"):make_puppet(Nation:get("mecklenburg"))
Nation:get("prussia"):make_puppet(Nation:get("grand_duchy_of_hesse_and_by_rhine"))

Nation:get("austria"):make_puppet(Nation:get("saxony"))
Nation:get("austria"):make_puppet(Nation:get("bavaria"))
Nation:get("austria"):make_puppet(Nation:get("wurttemberg"))
Nation:get("austria"):make_puppet(Nation:get("parma"))
Nation:get("austria"):make_puppet(Nation:get("modena"))
Nation:get("austria"):make_puppet(Nation:get("tuscany"))

Nation:get("denmark"):make_puppet(Nation:get("holstein"))

-- Relevant nations to choose
--Nation:get("united_kingdom"):make_relevant()
--Nation:get("france"):make_relevant()
--Nation:get("prussia"):make_relevant()
--Nation:get("russia"):make_relevant()
--Nation:get("austria"):make_relevant()
--Nation:get("united_states"):make_relevant()

--invalid_shit()
set_date(1825, 0, 0)
print("********************************************************************************")
print("Industrial era mod")
print("Date: ", get_year(), "/", get_month(), "/", get_day())
print("********************************************************************************")
