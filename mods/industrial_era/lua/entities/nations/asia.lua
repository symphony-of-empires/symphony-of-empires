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
--  	lua/nations/asia.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

nation = Nation:new{ ref_name = "korea", name = _("Korea") }
nation.adjective = _("Korean")
nation:register()
nation:add_accepted_language(Language:get("korean"))
nation:add_client_hint(monarchy, _("Korea"), 0x808080)

nation = Nation:new{ ref_name = "taiwan", name = _("Taiwan") }
nation.adjective = _("Taiwanesse")
nation:register()
nation:add_accepted_language(Language:get("chinesse"))
nation:add_client_hint(socialism, _("Taiwan's People's Republic"), 0x808080)
nation:add_client_hint(communism, _("Taiwan"), 0x808080)

nation = Nation:new{ ref_name = "qing", name = _("Qing") }
nation.adjective = _("Chinesse")
nation:register()
nation:add_accepted_language(Language:get("chinesse"))
nation:add_client_hint(monarchy, _("Qing Dynasty"), 0xF8F180)
nation:add_client_hint(communism, _("Qing's People's Republic"), 0xF8F180)

nation = Nation:new{ ref_name = "beyiang", name = _("Beyiang") }
nation.adjective = _("Chinesse")
nation:register()
nation:add_accepted_language(Language:get("chinesse"))
nation:add_client_hint(monarchy, _("China"), 0x808080)
nation:add_client_hint(communism, _("China"), 0x808080)
nation:add_client_hint(fascism, _("China"), 0x808080)
nation:add_client_hint(socialism, _("China"), 0x808080)
nation:add_client_hint(republic, _("China"), 0x808080)

nation = Nation:new{ ref_name = "china", name = _("China") }
nation.adjective = _("Chinesse")
nation:register()
nation:add_accepted_language(Language:get("chinesse"))
nation:add_client_hint(republic, _("China"), 0x808080)
nation:add_client_hint(communism, _("China"), 0x808080)

nation = Nation:new{ ref_name = "yunnan", name = _("Yunnan") }
nation.adjective = _("Yunnan")
nation:register()
nation:add_accepted_language(Language:get("chinesse"))
nation:add_client_hint(monarchy, _("Yunnan Dynasty"), 0x808080)

nation = Nation:new{ ref_name = "guanxi", name = _("Guanxi") }
nation.adjective = _("Guanxi")
nation:register()
nation:add_accepted_language(Language:get("chinesse"))
nation:add_client_hint(monarchy, _("Guanxi Clique"), 0x808080)

nation = Nation:new{ ref_name = "shanxi", name = _("Shanxi") }
nation.adjective = _("Shanxi")
nation:register()
nation:add_accepted_language(Language:get("chinesse"))
nation:add_client_hint(monarchy, _("Shanxi Clique"), 0x808080)

nation = Nation:new{ ref_name = "tokugawa", name = _("Tokugawa") }
nation.adjective = _("Japanesse")
nation:register()
nation:add_accepted_language(Language:get("japanesse"))
nation:add_client_hint(monarchy, _("Edo Shogunate"), 0xf0b4d4)

nation = Nation:new{ ref_name = "johor", name = _("Johor") }
nation.adjective = _("Johor")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("chinesse"))
nation:add_client_hint(monarchy, _("Johor"), 0x896241)
nation:add_client_hint(fascism, _("Johor"), 0x896241)
nation:add_client_hint(republic, _("Johor"), 0x896241)
nation:add_client_hint(socialism, _("Johor"), 0x896241)

nation = Nation:new{ ref_name = "siam", name = _("Siam") }
nation.adjective = _("Siamesse")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("thai"))
nation:add_client_hint(monarchy, _("Siam"), 0x2F54BD)

nation = Nation:new{ ref_name = "burma", name = _("Burma") }
nation.adjective = _("Burmesse")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("burmesse"))
nation:add_client_hint(monarchy, _("Burma"), 0x2FFFBD)

nation = Nation:new{ ref_name = "vietnam", name = _("Vietnam") }
nation.adjective = _("Vietnamesse")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Vietnam"), 0x857732)
nation:add_client_hint(socialism, _("Vietnam"), 0xF57732)
nation:add_client_hint(communism, _("Vietnam"), 0xF57732)

nation = Nation:new{ ref_name = "cambodia", name = _("Cambodia") }
nation.adjective = _("Cambodian")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Cambodia"), 0x512f91)

nation = Nation:new{ ref_name = "east_india_company", name = _("East India Company") }
nation.adjective = _("British Indian")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("East India Company"), 0xdb5656)

nation = Nation:new{ ref_name = "mysore", name = _("Mysore") }
nation.adjective = _("Mysorian")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Mysore"), 0x512f91)

nation = Nation:new{ ref_name = "haydarabad", name = _("Haydarabad") }
nation.adjective = _("Haydarabad")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Haydarabad"), 0x512f91)

nation = Nation:new{ ref_name = "awadh", name = _("Awadh") }
nation.adjective = _("Awadh")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Awadh"), 0x512f91)

nation = Nation:new{ ref_name = "bhutan", name = _("Bhutan") }
nation.adjective = _("Bhutan")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Bhutan"), 0x512f91)

nation = Nation:new{ ref_name = "nepal", name = _("Nepal") }
nation.adjective = _("Nepal")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Nepal"), 0x512f91)

nation = Nation:new{ ref_name = "kashmir", name = _("Kashmir") }
nation.adjective = _("Kashmir")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Kashmir"), 0x512f91)

nation = Nation:new{ ref_name = "berah", name = _("Berah") }
nation.adjective = _("Berah")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Berah"), 0xb86339)

nation = Nation:new{ ref_name = "sindh", name = _("Sindh") }
nation.adjective = _("Sindh")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Sindh"), 0x375c38)

nation = Nation:new{ ref_name = "sikh", name = _("Sikh") }
nation.adjective = _("Sikh")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Sikh"), 0xd65718)
