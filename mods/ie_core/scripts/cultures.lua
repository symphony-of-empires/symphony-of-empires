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
--  	cultures.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Cultures

-- European
german = Culture:new{ ref_name = "german", color = 0x916c53 }
german.name = _("German")
german:register()
french = Culture:new{ ref_name = "french", color = 0x7346ab }
french.name = _("French")
french.combo_form = _("Franco")
french:register()
british = Culture:new{ ref_name = "british", color = 0xc94c40 }
british.name = _("British")
british:register()
dutch = Culture:new{ ref_name = "dutch", color = 0xf5931b }
dutch.name = _("Dutch")
dutch:register()
belgian = Culture:new{ ref_name = "belgian", color = 0xe8db2a }
belgian.name = _("Belgian")
belgian:register()
danish = Culture:new{ ref_name = "danish", color = 0xc27b6d }
danish.name = _("Danish")
danish:register()
icelandic = Culture:new{ ref_name = "icelandic", color = 0x8293d1 }
icelandic.name = _("Icelandic")
icelandic:register()
irish = Culture:new{ ref_name = "irish", name = _("Irish"), color = 0x40b225 }
irish:register()
romanian = Culture:new{ ref_name = "romanian", color = 0xcfa85b }
romanian.name = _("Romanian")
romanian:register()
portuguese = Culture:new{ ref_name = "portuguese", color = 0x0f691f }
portuguese.name = _("Portuguese")
portuguese:register()
spanish = Culture:new{ ref_name = "spanish", color = 0xdee03a }
spanish.name = _("Spanish")
spanish:register()
basque = Culture:new{ ref_name = "basque" }
basque.name = _("Basque")
basque:register()
galician = Culture:new{ ref_name = "galician" }
galician.name = _("Galician")
galician:register()
aragonese = Culture:new{ ref_name = "aragonese" }
aragonese.name = _("Aragonese")
aragonese:register()
catalan = Culture:new{ ref_name = "catalan", name = _("Catalan"), color = 0xeda65a }
catalan:register()
czech = Culture:new{ ref_name = "czech", name = _("Czech"), color = 0x384a8a }
czech.combo_form = _("Czecho")
czech:register()
slovakian = Culture:new{ ref_name = "slovakian" }
slovakian.name = _("Slovakian")
slovakian:register()
croatian = Culture:new{ ref_name = "croatian" }
croatian.name = _("Croatian")
croatian:register()
slovenian = Culture:new{ ref_name = "slovenian" }
slovenian.name = _("Slovenian")
slovenian:register()
hungarian = Culture:new{ ref_name = "hungarian", color = 0x3f7051 }
hungarian.name = _("Hungarian")
hungarian:register()
ukrainian = Culture:new{ ref_name = "ukrainian", name = _("Ukrainian"), color = 0xd6c913 }
ukrainian:register()
russian = Culture:new{ ref_name = "russian", name = _("Russian"), color = 0xbf0000 }
russian.name = _("Russian")
russian.combo_form = _("Russo")
russian:register()
swedish = Culture:new{ ref_name = "swedish", color = 0x3a739e }
swedish.name = _("Swedish")
swedish.noun = _("Swede")
swedish:register()
finnish = Culture:new{ ref_name = "finnish", color = 0xb9ebeb }
finnish.name = _("Finnish")
finnish.combo_form = _("Finno")
finnish:register()
norwegian = Culture:new{ ref_name = "norwegian", color = 0xab1659 }
norwegian.name = _("Norwegian")
norwegian:register()
turkish = Culture:new{ ref_name = "turkish", name = _("Turkish"), color = 0xc9201a }
turkish.noun = _("Turk")
turkish.combo_form = _("Turko")
turkish:register()
greek = Culture:new{ ref_name = "greek", name = _("Greek"), color = 0xaac4e6 }
greek.combo_form = _("Greco")
greek:register()
italian = Culture:new{ ref_name = "italian", name = _("Italian"), color = 0x679e7b }
italian.combo_form = _("Italo")
italian:register()
lithuanian = Culture:new{ ref_name = "lithuanian", name = _("Lithuanian"), color = 0x598c42 }
lithuanian:register()
latvian = Culture:new{ ref_name = "latvian", name = _("Latvian"), color = 0x962820 }
latvian:register()
estonian = Culture:new{ ref_name = "estonian", name = _("Estonian"), color = 0x4c8ed4 }
estonian:register()
polish = Culture:new{ ref_name = "polish", name = _("Polish"), color = 0xbf85d6 }
polish:register()
bulgarian = Culture:new{ ref_name = "bulgarian", color = 0x78cf7d }
bulgarian.name = _("Bulgarian")
bulgarian:register()
serbian = Culture:new{ ref_name = "serbian", name = _("Serbian"), color = 0x905c4b }
serbian.noun = _("Serb")
serbian.combo_form = _("Serbo")
serbian:register()
alemmanic = Culture:new{ ref_name = "alemmanic" }
alemmanic.name = ("Alemmanic")
alemmanic:register()
arpitan = Culture:new{ ref_name = "arpitan" }
arpitan.name = _("Arpitan")
arpitan:register()
frainc_comtou = Culture:new{ ref_name = "frainc_comtou", name = _("Frainc-Comtou") }
frainc_comtou:register()
lombard = Culture:new{ ref_name = "lombard"}
lombard.name = _("Lombard")
lombard:register()
romansh = Culture:new{ ref_name = "romansh", name = _("Romansh") }
romansh:register()
walser = Culture:new{ ref_name = "walser" }
walser.name = _("Walser")
walser:register()

-- Eurasia and West Asia
Culture:new{ ref_name = "nogai", name = _("Nogai") }:register()

-- Central Asia
Culture:new{ ref_name = "kazakh", name = _("Kazakh") }:register()
Culture:new{ ref_name = "uzbek", name = _("Uzbek") }:register()
Culture:new{ ref_name = "tajik", name = _("Tajik") }:register()
Culture:new{ ref_name = "kyrgyz", name = _("Kyrgyz") }:register()
Culture:new{ ref_name = "afghan", name = _("Afghan"), combo_form = _("Afghano") }:register()

-- East asian
Culture:new{ ref_name = "korean", name = _("Korean") }:register()

-- American
Culture:new{ ref_name = "american", name = _("American"), adjective = _("American"), color = 0x6ea5cc }:register()
Culture:new{ ref_name = "chilean", name = _("Chilean"), color = 0xad6dcf }:register()
cisplatinian = Culture:new{ ref_name = "cisplatinian", name = _("Cisplatinian") }
cisplatinian:register()
argetine = Culture:new{ ref_name = "argetine", name = _("Argentinian") }
argetine:register()
bolivian = Culture:new{ ref_name = "bolivian", name = _("Bolivian") }
bolivian:register()
paraguayan = Culture:new{ ref_name = "paraguayan", name = _("Paraguayan") }
paraguayan:register()
colombian = Culture:new{ ref_name = "colombian", name = _("Colombian") }
colombian:register()
venezuelan = Culture:new{ ref_name = "venezuelan", name = _("Venezuelan") }
venezuelan:register()
ecuadorian = Culture:new{ ref_name = "ecuadorian", name = _("Ecuadorian") }
ecuadorian:register()
peruvian = Culture:new{ ref_name = "peruvian", name = _("Peruvian") }
peruvian:register()
mexican = Culture:new{ ref_name = "mexican", name = _("Mexican"), color = 0x5ac780 }
mexican:register()
haitian = Culture:new{ ref_name = "haitian", name = _("Haitian") }
haitian:register()
brazilian = Culture:new{ ref_name = "brazilian", name = _("Brazilian") }
brazilian:register()

-- Native american
mapuche = Culture:new{ ref_name = "mapuche" }
mapuche.name = _("Mapuche")
mapuche:register()

-- Oceanic cultures
polynesian = Culture:new{ ref_name = "polynesian" }
polynesian.name = _("Polynesian")
polynesian:register()

-- Middle east
arabic = Culture:new{ ref_name = "arabic" }
arabic.name = _("Arabic")
arabic.noun = _("Arab")
arabic:register()

-- African
Culture:new{ ref_name = "akan", name = _("Akan") }:register()
abyssinian = Culture:new{ ref_name = "abyssinian" }
abyssinian.name = _("Abyssinian")
abyssinian:register()
amhara = Culture:new{ ref_name = "amhara" }
amhara.name = _("Amhara")
amhara:register()
chewa = Culture:new{ ref_name = "chewa" }
chewa.name = _("Chewa")
chewa:register()
fulani = Culture:new{ ref_name = "fulani" }
fulani.name = _("Fulani")
fulani:register()
hausa = Culture:new{ ref_name = "hausa" }
hausa.name = _("Hausa")
hausa:register()
hutu = Culture:new{ ref_name = "hutu" }
hutu.name = _("Hutu")
hutu:register()
igbo = Culture:new{ ref_name = "igbo" }
igbo.name = _("Igbo")
igbo:register()
kanuri = Culture:new{ ref_name = "kanuri" }
kanuri.name = _("Kanuri")
kanuri:register()
kongo = Culture:new{ ref_name = "kongo" }
kongo.name = _("Kongo")
kongo:register()
luba = Culture:new{ ref_name = "luba" }
luba.name = _("Luba")
luba:register()
maghrebis = Culture:new{ ref_name = "maghrebis" }
maghrebis.name = _("Maghrebis")
maghrebis:register()
mongo = Culture:new{ ref_name = "mongo" }
mongo.name = _("Mongo")
mongo:register()
nilotes = Culture:new{ ref_name = "nilotes" }
nilotes.name = _("Nilotes")
nilotes:register()
oromo = Culture:new{ ref_name = "oromo" }
oromo.name = _("Oromo")
oromo:register()
shona = Culture:new{ ref_name = "shona" }
shona.name = _("Shona")
shona:register()
somali = Culture:new{ ref_name = "somali" }
somali.name = _("Somali")
somali:register()
yoruba = Culture:new{ ref_name = "yoruba" }
yoruba.name = _("Yoruba")
yoruba:register()
zulu = Culture:new{ ref_name = "zulu" }
zulu.name = _("Zulu")
zulu:register()