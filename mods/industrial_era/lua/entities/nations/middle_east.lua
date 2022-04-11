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
--  	lua/nations/middle_east.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

nation = Nation:new{ ref_name = "najran", name = _("Najran") }
nation.adjective = _("Najrani")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Najran"), 0xDEED82)
nation:add_client_hint(communism, _("Najran"), 0xDEED82)
nation:add_client_hint(fascism, _("Najran"), 0xDEED82)
nation:add_client_hint(republic, _("Najran"), 0xDEED82)

nation = Nation:new{ ref_name = "zaidi_imamate", name = _("Zaidi Imamate") }
nation.adjective = _("Zaidi")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Zaidi Imamate"), 0x8e332f)
nation:add_client_hint(communism, _("Zaidi Imamate"), 0x8e332f)
nation:add_client_hint(fascism, _("Zaidi Imamate"), 0x8e332f)
nation:add_client_hint(republic, _("Zaidi Imamate"), 0x8e332f)

nation = Nation:new{ ref_name = "oman", name = _("Oman") }
nation.adjective = _("Omani")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Oman"), 0x3B3E69)
nation:add_client_hint(communism, _("Oman"), 0x3B3E69)
nation:add_client_hint(fascism, _("Oman"), 0x3B3E69)
nation:add_client_hint(republic, _("Oman"), 0x3B3E69)

nation = Nation:new{ ref_name = "nejd", name = _("Nejd") }
nation.adjective = _("Nejdi")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Nejd"), 0x314E2A)
nation:add_client_hint(communism, _("Nejd"), 0x314E2A)
nation:add_client_hint(fascism, _("Nejd"), 0x314E2A)
nation:add_client_hint(republic, _("Nejd"), 0x314E2A)

nation = Nation:new{ ref_name = "mahra", name = _("Mahra") }
nation.adjective = _("Mahrai")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Mahra"), 0x52AC3B)
nation:add_client_hint(communism, _("Mahra"), 0x52AC3B)
nation:add_client_hint(fascism, _("Mahra"), 0x52AC3B)
nation:add_client_hint(republic, _("Mahra"), 0x52AC3B)

nation = Nation:new{ ref_name = "muscat", name = _("Muscat") }
nation.adjective = _("Muscati")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Muscat"), 0xE77A35)
nation:add_client_hint(communism, _("Muscat"), 0xE77A35)
nation:add_client_hint(fascism, _("Muscat"), 0xE77A35)
nation:add_client_hint(republic, _("Muscat"), 0xE77A35)

nation = Nation:new{ ref_name = "kasadi", name = _("Kasadi") }
nation.adjective = _("Kasadi")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Kasadi"), 0xE73537)

nation = Nation:new{ ref_name = "mahwali", name = _("Mahwali") }
nation.adjective = _("Mahwali")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Mahwali"), 0x8A35E7)

nation = Nation:new{ ref_name = "wahidi", name = _("Wahidi") }
nation.adjective = _("Wahidi")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Wahidi"), 0x3599E7)

nation = Nation:new{ ref_name = "wadi_hajr", name = _("Wadi Hajr") }
nation.adjective = _("Wadi Hajr")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Wadi Hajr"), 0xC2E735)

nation = Nation:new{ ref_name = "tamim", name = _("Tamim") }
nation.adjective = _("Tamim")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Tamim"), 0xA71256)

nation = Nation:new{ ref_name = "hamum", name = _("Hamum") }
nation.adjective = _("Hamum")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Hamum"), 0x63D03E)

nation = Nation:new{ ref_name = "aulaqi", name = _("Aulaqi") }
nation.adjective = _("Aulaqi")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Aulaqi"), 0xd0cc3e)

nation = Nation:new{ ref_name = "fadhli", name = _("Fadhli") }
nation.adjective = _("Fadhli")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Fadhli"), 0x53f1f6)

nation = Nation:new{ ref_name = "beihan", name = _("Beihan") }
nation.adjective = _("Beihani")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Beihan"), 0x682f8e)

nation = Nation:new{ ref_name = "dhale", name = _("Dhale") }
nation.adjective = _("Dhalian")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Dhale"), 0xff00d0)

nation = Nation:new{ ref_name = "upper_yafa", name = _("Upper Yafa") }
nation.adjective = _("Yafanian")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Upper Yafa"), 0x9D7DED)

nation = Nation:new{ ref_name = "lower_yafa", name = _("Lower Yafa") }
nation.adjective = _("Yafanian")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Lower Yafa"), 0x9D7DED)

nation = Nation:new{ ref_name = "beda", name = _("Beda") }
nation.adjective = _("Bedan")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Beda"), 0x2C5C89)

nation = Nation:new{ ref_name = "audhali", name = _("Audhali") }
nation.adjective = _("Audhali")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Audhali"), 0x2C8931)

nation = Nation:new{ ref_name = "atifi", name = _("Atifi") }
nation.adjective = _("Atifi")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Atifi"), 0x9DB4AD)

nation = Nation:new{ ref_name = "kaluli", name = _("Kaluli") }
nation.adjective = _("Kaluli")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Kaluli"), 0x652A7F)

nation = Nation:new{ ref_name = "aghbari", name = _("Aghbari") }
nation.adjective = _("Aghbari")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Aghbari"), 0xAE28E8)

nation = Nation:new{ ref_name = "barhimi", name = _("Barhimi") }
nation.adjective = _("Barhimi")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Barhimi"), 0x2875E8)

nation = Nation:new{ ref_name = "aqrabi", name = _("Aqrabi") }
nation.adjective = _("Aqrabi")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Aqrabi"), 0xBC4721)

nation = Nation:new{ ref_name = "rujai", name = _("Rujai") }
nation.adjective = _("Rujai")
nation:register()
nation:set_ideology(monarchy)
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Rujai"), 0xE32764)
