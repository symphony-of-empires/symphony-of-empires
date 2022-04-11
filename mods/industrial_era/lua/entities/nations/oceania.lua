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
--  	lua/nations/oceania.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

nation = Nation:new{ ref_name = "aceh", name = _("Aceh") }
nation.adjective = _("Aceh")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Aceh"), 0xC82344)
nation:add_client_hint(fascism, _("Aceh"), 0xC82344)
nation:add_client_hint(republic, _("Aceh"), 0xC82344)
nation:add_client_hint(socialism, _("Aceh"), 0xC82344)

nation = Nation:new{ ref_name = "johor", name = _("Johor") }
nation.adjective = _("Johor")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Johor"), 0x896241)
nation:add_client_hint(fascism, _("Johor"), 0x896241)
nation:add_client_hint(republic, _("Johor"), 0x896241)
nation:add_client_hint(socialism, _("Johor"), 0x896241)

nation = Nation:new{ ref_name = "siam", name = _("Siam") }
nation.adjective = _("Siam")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Siam"), 0x2F54BD)

nation = Nation:new{ ref_name = "perak", name = _("Perak") }
nation.adjective = _("Perak")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Perak"), 0x85ACD6)

nation = Nation:new{ ref_name = "pagaruyung", name = _("Pagaruyung") }
nation.adjective = _("Pagaruyung")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Pagaruyung"), 0xB5832F)

nation = Nation:new{ ref_name = "deli", name = _("Deli") }
nation.adjective = _("Deli")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Deli"), 0xB5382F)

nation = Nation:new{ ref_name = "siak", name = _("Siak") }
nation.adjective = _("Siak")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Siak"), 0x7046AF)

nation = Nation:new{ ref_name = "sulu", name = _("Sulu") }
nation.adjective = _("Sulu")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Sulu"), 0x374A82)
nation:add_client_hint(fascism, _("Sulu"), 0x374A82)
nation:add_client_hint(republic, _("Sulu"), 0x374A82)
nation:add_client_hint(socialism, _("Sulu"), 0x374A82)

nation = Nation:new{ ref_name = "bali", name = _("Bali") }
nation.adjective = _("Bali")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Bali"), 0x494B82)

nation = Nation:new{ ref_name = "lombok", name = _("Lombok") }
nation.adjective = _("Lombok")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Lombok"), 0x7D251B)

nation = Nation:new{ ref_name = "bima", name = _("Bima") }
nation.adjective = _("Bima")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Bima"), 0xC31E3A)

nation = Nation:new{ ref_name = "brunei", name = _("Brunei") }
nation.adjective = _("Brunei")
nation:register()
nation:set_ideology(monarchy)
--nation:add_accepted_culture(chinesse)
nation:add_client_hint(monarchy, _("Brunei"), 0xFBFF41)
