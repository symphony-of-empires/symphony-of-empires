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
--  	ideologies.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- communism = Ideology.new("communism", _("Communism"))
-- print(communism)
-- print(type(communism))
-- print(communism.ref_name)
-- for k,v in pairs(getmetatable(communism)) do
--     print(k, v)
-- end
-- communism:register()
-- communism = Ideology.get("communism")
-- print(communism)
-- print(type(communism))
-- print(communism.ref_name)

communism = Ideology:new{ ref_name = "communism", name = _("Communism"), color = 0x800e04 }
communism:register()

democracy = Ideology:new{ ref_name = "democracy", name = _("Democracy"), color = 0x4366a1 }
democracy:register()

fascism = Ideology:new{ ref_name = "fascism", name = _("Fascism"), color = 0xcc1f6d }
fascism:register()

socialism = Ideology:new{ ref_name = "socialism", name = _("Socialism"), color = 0xff1500 }
socialism:register()

republic = Ideology:new{ ref_name = "republic", name = _("Republic"), color = 0x4245f5 }
republic:register()

tribal = Ideology:new{ ref_name = "tribal", name = _("Tribal"), check_policies_fn = "?" }
tribal:register()

monarchy = Ideology:new{ ref_name = "monarchy", name = _("Monarchy"), color = 0xf2de24 }
monarchy:register()

anarchy = Ideology:new{ ref_name = "anarchy", name = _("Anarchy"), color = 0x363533 }
anarchy:register()
