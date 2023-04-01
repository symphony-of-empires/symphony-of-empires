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
--  	languages.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Languages
-- European
german = Language:new{ ref_name = "german", color = 0x916c53 }
german.name = _("German")
german:register()
french = Language:new{ ref_name = "french", color = 0x7346ab }
french.name = _("French")
french.combo_form = _("Franco")
french:register()
english = Language:new{ ref_name = "english", color = 0xc94c40 }
english.name = _("English")
english:register()
dutch = Language:new{ ref_name = "dutch", color = 0xf5931b }
dutch.name = _("Dutch")
dutch:register()
belgian = Language:new{ ref_name = "belgian", color = 0xe8db2a }
belgian.name = _("Belgian")
belgian:register()
danish = Language:new{ ref_name = "danish", color = 0xc27b6d }
danish.name = _("Danish")
danish:register()
icelandic = Language:new{ ref_name = "icelandic", color = 0x8293d1 }
icelandic.name = _("Icelandic")
icelandic:register()
irish = Language:new{ ref_name = "irish", name = _("Irish"), color = 0x40b225 }
irish:register()
romanian = Language:new{ ref_name = "romanian", color = 0xcfa85b }
romanian.name = _("Romanian")
romanian:register()
romanian = Language:new{ ref_name = "armenian", color = 0xcfa85b }
romanian.name = _("Armenian")
romanian:register()
portuguese = Language:new{ ref_name = "portuguese", color = 0x0f691f }
portuguese.name = _("Portuguese")
portuguese:register()
spanish = Language:new{ ref_name = "spanish", color = 0xdee03a }
spanish.name = _("Spanish")
spanish:register()
basque = Language:new{ ref_name = "basque" }
basque.name = _("Basque")
basque:register()
galician = Language:new{ ref_name = "galician" }
galician.name = _("Galician")
galician:register()
aragonese = Language:new{ ref_name = "aragonese" }
aragonese.name = _("Aragonese")
aragonese:register()
catalan = Language:new{ ref_name = "catalan", name = _("Catalan"), color = 0xeda65a }
catalan:register()
czech = Language:new{ ref_name = "czech", name = _("Czech"), color = 0x384a8a }
czech.combo_form = _("Czecho")
czech:register()
slovakian = Language:new{ ref_name = "slovakian", name = _("Slovakian"), color = 0x668394 }
slovakian:register()
croatian = Language:new{ ref_name = "croatian", name = _("Croatian"), color = 0x6a6694 }
croatian:register()
slovenian = Language:new{ ref_name = "slovenian", name = _("Slovenian"), color = 0x2515d4 }
slovenian:register()
hungarian = Language:new{ ref_name = "hungarian", color = 0x3f7051 }
hungarian.name = _("Hungarian")
hungarian:register()
ukrainian = Language:new{ ref_name = "ukrainian", name = _("Ukrainian"), color = 0xd6c913 }
ukrainian:register()
byelorussian = Language:new{ ref_name = "byelorussian", name = _("Byelorussian"), color = 0x308549 }
byelorussian:register()
russian = Language:new{ ref_name = "russian", name = _("Russian"), color = 0xbf0000 }
russian.name = _("Russian")
russian.combo_form = _("Russo")
russian:register()
swedish = Language:new{ ref_name = "swedish", color = 0x3a739e }
swedish.name = _("Swedish")
swedish.noun = _("Swede")
swedish:register()
finnish = Language:new{ ref_name = "finnish", color = 0xb9ebeb }
finnish.name = _("Finnish")
finnish.combo_form = _("Finno")
finnish:register()
norwegian = Language:new{ ref_name = "norwegian", color = 0xab1659 }
norwegian.name = _("Norwegian")
norwegian:register()
turkish = Language:new{ ref_name = "turkish", name = _("Turkish"), color = 0xc9201a }
turkish.noun = _("Turk")
turkish.combo_form = _("Turko")
turkish:register()
greek = Language:new{ ref_name = "greek", name = _("Greek"), color = 0xaac4e6 }
greek.combo_form = _("Greco")
greek:register()
italian = Language:new{ ref_name = "italian", name = _("Italian"), color = 0x679e7b }
italian.combo_form = _("Italo")
italian:register()
lithuanian = Language:new{ ref_name = "lithuanian", name = _("Lithuanian"), color = 0x598c42 }
lithuanian:register()
latvian = Language:new{ ref_name = "latvian", name = _("Latvian"), color = 0x962820 }
latvian:register()
estonian = Language:new{ ref_name = "estonian", name = _("Estonian"), color = 0x4c8ed4 }
estonian:register()
polish = Language:new{ ref_name = "polish", name = _("Polish"), color = 0xbf85d6 }
polish:register()
bulgarian = Language:new{ ref_name = "bulgarian", color = 0x78cf7d }
bulgarian.name = _("Bulgarian")
bulgarian:register()
serbian = Language:new{ ref_name = "serbian", name = _("Serbian"), color = 0x905c4b }
serbian.noun = _("Serb")
serbian.combo_form = _("Serbo")
serbian:register()
laz = Language:new{ ref_name = "laz", name = _("Laz"), color = 0x030057 }
laz:register()

-- Eurasia and West Asia
Language:new{ ref_name = "nogai", name = _("Nogai") }:register()

-- Central Asia
Language:new{ ref_name = "kazakh", name = _("Kazakh") }:register()
Language:new{ ref_name = "circassian", name = _("Circassian") }:register()
Language:new{ ref_name = "uzbek", name = _("Uzbek") }:register()
Language:new{ ref_name = "tajik", name = _("Tajik") }:register()
Language:new{ ref_name = "kyrgyz", name = _("Kyrgyz") }:register()
Language:new{ ref_name = "afghan", name = _("Afghan"), combo_form = _("Afghano") }:register()

-- East asian
korean = Language:new{ ref_name = "korean", name = _("Korean") }
korean:register()
chinesse = Language:new{ ref_name = "chinesse", name = _("Chinesse") }
chinesse:register()
japanesse = Language:new{ ref_name = "japanesse", name = _("Japanesse") }
japanesse:register()
burmesse = Language:new{ ref_name = "burmesse", name = _("Burmesse"), color = 0x2FFFBD }
burmesse:register()
thai = Language:new{ ref_name = "thai", name = _("Thai"), color = 0x2F54BD }
thai:register()
vietnamesse = Language:new{ ref_name = "vietnamesse", name = _("Vietnamesse"), color = 0x857732 }
vietnamesse:register()

-- American
Language:new{ ref_name = "american", name = _("American"), adjective = _("American"), color = 0x6ea5cc }:register()

-- Native american
mapuche = Language:new{ ref_name = "mapuche", color = 0x8c9ebd }
mapuche.name = _("Mapuche")
mapuche:register()

-- Oceanic languages
malay = Language:new{ ref_name = "malay" }
malay.name = _("Malay")
malay:register()
polynesian = Language:new{ ref_name = "polynesian" }
polynesian.name = _("Polynesian")
polynesian:register()

-- Middle east
arabic = Language:new{ ref_name = "arabic", name = _("Arabic"), noun = _("Arab"), color = 0x388713 }
arabic:register()
kurdish = Language:new{ ref_name = "kurdish", name = _("Kurdish"), color = 0x679e7b }
kurdish.combo_form = _("kurdo")
kurdish:register()
coptic = Language:new{ ref_name = "coptic", name = _("Coptic"), noun = _("Copic"), color = 0x18548c }
coptic:register()

-- African
Language:new{ ref_name = "akan", name = _("Akan") }:register()
abyssinian = Language:new{ ref_name = "abyssinian" }
abyssinian.name = _("Abyssinian")
abyssinian:register()
amhara = Language:new{ ref_name = "amhara" }
amhara.name = _("Amhara")
amhara:register()
chewa = Language:new{ ref_name = "chewa" }
chewa.name = _("Chewa")
chewa:register()
fulani = Language:new{ ref_name = "fulani" }
fulani.name = _("Fulani")
fulani:register()
hausa = Language:new{ ref_name = "hausa" }
hausa.name = _("Hausa")
hausa:register()
hutu = Language:new{ ref_name = "hutu" }
hutu.name = _("Hutu")
hutu:register()
igbo = Language:new{ ref_name = "igbo" }
igbo.name = _("Igbo")
igbo:register()
kanuri = Language:new{ ref_name = "kanuri" }
kanuri.name = _("Kanuri")
kanuri:register()
kongo = Language:new{ ref_name = "kongo" }
kongo.name = _("Kongo")
kongo:register()
luba = Language:new{ ref_name = "luba" }
luba.name = _("Luba")
luba:register()
maghrebis = Language:new{ ref_name = "maghrebis" }
maghrebis.name = _("Maghrebis")
maghrebis:register()
mongo = Language:new{ ref_name = "mongo" }
mongo.name = _("Mongo")
mongo:register()
nilotes = Language:new{ ref_name = "nilotes" }
nilotes.name = _("Nilotes")
nilotes:register()
oromo = Language:new{ ref_name = "oromo" }
oromo.name = _("Oromo")
oromo:register()
shona = Language:new{ ref_name = "shona" }
shona.name = _("Shona")
shona:register()
somali = Language:new{ ref_name = "somali" }
somali.name = _("Somali")
somali:register()
yoruba = Language:new{ ref_name = "yoruba" }
yoruba.name = _("Yoruba")
yoruba:register()
zulu = Language:new{ ref_name = "zulu" }
zulu.name = _("Zulu")
zulu:register()
