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

korea = Nation:new{ ref_name = "korea", name = _("Korea") }
korea.adjective = _("Korean")
korea:register()
korea:add_accepted_culture(Culture:get("korean"))
korea:add_client_hint(monarchy, _("Korea"), 0x808080)

taiwan = Nation:new{ ref_name = "taiwan", name = _("Taiwan") }
taiwan.adjective = _("Taiwanesse")
taiwan:register()
--taiwan:add_accepted_culture(chinesse)
taiwan:add_client_hint(socialism, _("Taiwan's People's Republic"), 0x808080)
taiwan:add_client_hint(communism, _("Taiwan"), 0x808080)

qing = Nation:new{ ref_name = "qing", name = _("Qing") }
qing.adjective = _("Chinesse")
qing:register()
--qing:add_accepted_culture(chinesse)
qing:add_client_hint(monarchy, _("Qing Dynasty"), 0xF8F180)
qing:add_client_hint(communism, _("Qing's People's Republic"), 0xF8F180)

beyiang = Nation:new{ ref_name = "beyiang", name = _("Beyiang") }
beyiang.adjective = _("Chinesse")
beyiang:register()
--beyiang:add_accepted_culture(chinesse)
beyiang:add_client_hint(monarchy, _("China"), 0x808080)
beyiang:add_client_hint(communism, _("China"), 0x808080)
beyiang:add_client_hint(fascism, _("China"), 0x808080)
beyiang:add_client_hint(socialism, _("China"), 0x808080)
beyiang:add_client_hint(republic, _("China"), 0x808080)

china = Nation:new{ ref_name = "china", name = _("China") }
china.adjective = _("Chinesse")
china:register()
--china:add_accepted_culture(chinesse)
china:add_client_hint(republic, _("China"), 0x808080)
china:add_client_hint(communism, _("China"), 0x808080)

yunnan = Nation:new{ ref_name = "yunnan", name = _("Yunnan") }
yunnan.adjective = _("Chinesse")
yunnan:register()
--yunnan:add_accepted_culture(chinesse)
yunnan:add_client_hint(monarchy, _("Yunnan Dynasty"), 0x808080)

guanxi = Nation:new{ ref_name = "guanxi", name = _("Guanxi") }
guanxi.adjective = _("Chinesse")
guanxi:register()
--guanxi:add_accepted_culture(chinesse)
guanxi:add_client_hint(monarchy, _("Guanxi Clique"), 0x808080)

shanxi = Nation:new{ ref_name = "shanxi", name = _("Shanxi") }
shanxi.adjective = _("Chinesse")
shanxi:register()
--shanxi:add_accepted_culture(chinesse)
shanxi:add_client_hint(monarchy, _("Shanxi Clique"), 0x808080)
