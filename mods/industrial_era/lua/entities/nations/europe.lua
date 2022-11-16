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
--  	lua/nations/europe.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-----------------------------------------------------------------------
-- Europe (excluding balkans, scandinavia and the mess that Germany was)
-- Initial nations
united_kingdom = Nation:new{ ref_name = "united_kingdom" }
united_kingdom.name = _("United Kingdom")
united_kingdom.adjective = _("British")
united_kingdom:register()
united_kingdom:set_ideology(monarchy, "absolute_monarchy")
united_kingdom:add_accepted_language(english)
united_kingdom:add_accepted_language(irish)
united_kingdom:add_client_hint(monarchy, _("United Kingdom"), 0xa81e11)
united_kingdom:set_flag("westernized", 1.0)

netherlands = Nation:new{ ref_name = "netherlands" }
netherlands.name = _("Netherlands")
netherlands.adjective = _("Dutch")
netherlands:register()
netherlands:set_ideology(monarchy, "absolute_monarchy")
netherlands:add_accepted_language(dutch)
netherlands:add_client_hint(monarchy, _("Netherlands"), 0xf5931b)
netherlands:set_flag("westernized", 1.0)

belgium = Nation:new{ ref_name = "belgium" }
belgium.name = _("Belgium")
belgium.adjective = _("Belgian")
belgium:register()
belgium:set_ideology(monarchy, "absolute_monarchy")
belgium:add_accepted_language(belgian)
belgium:add_client_hint(monarchy, _("Belgium"), 0xe8db2a)
belgium:set_flag("westernized", 1.0)

france = Nation:new{ ref_name = "france" }
france.name = _("France")
france.adjective = _("French")
france.combo_form = _("Franco")
france:register()
france:set_ideology(monarchy, "absolute_monarchy")
france:add_accepted_language(french)
france:add_client_hint(monarchy, _("France"), 0x1b3cb3)
france:set_flag("westernized", 1.0)

spain = Nation:new{ ref_name = "spain" }
spain.name = _("Spain")
spain.adjective = _("Spanish")
spain.noun = _("Spainiard")
spain:register()
spain:set_ideology(monarchy, "absolute_monarchy")
spain:add_accepted_language(spanish)
spain:add_client_hint(monarchy, _("Spain"), 0xdee03a)
spain:set_flag("westernized", 1.0)

portugal = Nation:new{ ref_name = "portugal" }
portugal.name = _("Portugal")
portugal.adjective = _("Portuguese")
portugal:register()
portugal:set_ideology(monarchy, "absolute_monarchy")
portugal:add_accepted_language(portuguese)
portugal:add_client_hint(monarchy, _("Portugal"), 0x0f691f)
portugal:add_client_hint(socialism, _("People's Republic of Portugal"), 0x0f691f)
portugal:set_flag("westernized", 1.0)

-- Formable nations
european_union = Nation:new{ ref_name = "european_union" }
european_union.name = _("European Union")
european_union.adjective = _("European")
european_union:register()
european_union:add_accepted_language(catalan)
european_union:add_accepted_language(basque)
european_union:add_accepted_language(galician)
european_union:add_accepted_language(aragonese)
european_union:add_accepted_language(portuguese)
european_union:add_accepted_language(spanish)
european_union:add_accepted_language(german)
european_union:add_accepted_language(polish)
european_union:add_accepted_language(czech)
european_union:add_accepted_language(slovakian)
european_union:add_accepted_language(french)
european_union:add_accepted_language(italian)
european_union:add_accepted_language(greek)
european_union:add_accepted_language(danish)
european_union:add_accepted_language(english)
european_union:add_accepted_language(irish)
european_union:add_accepted_language(portuguese)
european_union:add_accepted_language(hungarian)
european_union:add_client_hint(republic, _("European Union"), 0x060666)
european_union:add_client_hint(socialism, _("European Commune"), 0x060666)

iberia = Nation:new{ ref_name = "iberia" }
iberia.name = _("Iberia")
iberia.adjective = _("Iberian")
iberia:register()
iberia:add_accepted_language(catalan)
iberia:add_accepted_language(basque)
iberia:add_accepted_language(galician)
iberia:add_accepted_language(aragonese)
iberia:add_accepted_language(portuguese)
iberia:add_accepted_language(spanish)
iberia:add_client_hint(monarchy, _("Iberia"), 0x808000)

-- Releasable nations
brittany = Nation:new{ ref_name = "brittany" }
brittany.name = _("Brittany")
brittany:register()
brittany:add_client_hint(monarchy, _("Brittany"), 0x808000)

normandy = Nation:new{ ref_name = "normandy" }
normandy.name = _("Normandy")
normandy:register()
normandy:add_client_hint(monarchy, _("Normandy"), 0x808000)

occitania = Nation:new{ ref_name = "occitania"}
occitania.name = _("Occitania")
occitania:register()
occitania:add_client_hint(monarchy, _("Occitania"), 0x808000)

catalonia = Nation:new{ ref_name = "catalonia" }
catalonia.name = _("Catalonia")
catalonia.adjective = _("Catalonian")
catalonia:register()
catalonia:add_client_hint(republic, _("Catalonia"), 0xeda65a)
catalonia:add_accepted_language(catalan)

basque_country = Nation:new{ ref_name = "basque" }
basque_country.name = _("Basque")
basque_country.adjective = _("Basque")
basque_country:register()
basque_country:add_client_hint(monarchy, _("Basque"), 0x808000)
basque_country:add_accepted_language(basque)

galicia = Nation:new{ ref_name = "galicia" }
galicia.name = _("Galicia")
galicia.adjective = _("Galician")
galicia:register()
galicia:add_client_hint(monarchy, _("Galicia"), 0x808000)
galicia:add_accepted_language(galician)

andorra = Nation:new{ ref_name = "andorra" }
andorra.name = _("Andorra")
andorra.adjective = _("Andorrian")
andorra:register()
andorra:add_client_hint(monarchy, _("Andorra"), 0xdb70d8)
andorra:add_client_hint(republic, _("Andorra"), 0xdb70d8)
andorra:add_accepted_language(spanish)
andorra:add_accepted_language(french)

monaco = Nation:new{ ref_name = "monaco" }
monaco.name = _("Monaco")
monaco.adjective = _("Monacense")
monaco:register()
monaco:add_client_hint(monarchy, _("Monaco"), 0xe3acce)
monaco:add_accepted_language(italian)
monaco:add_accepted_language(french)

aragon = Nation:new{ ref_name = "aragon" }
aragon.name = _("Aragon")
aragon.adjective = _("Aragonesse")
aragon:register()
aragon:add_accepted_language(aragonese)
aragon:add_client_hint(monarchy, _("Aragon"), 0x808000)

-- Formable by both czech and slovakia
czechoslovakia = Nation:new{ ref_name = "czechoslovakia" }
czechoslovakia.name = _("Czechoslovakia")
czechoslovakia.adjective = _("Czechoslovak")
czechoslovakia:register()
czechoslovakia:add_accepted_language(slovakian)
czechoslovakia:add_accepted_language(czech)
czechoslovakia:add_client_hint(socialism, _("Czechoslovakia"), 0x384a8a)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Scandinavia
-- Initial nations
denmark = Nation:new{ ref_name = "denmark" }
denmark.name = _("Denmark")
denmark.adjective = _("Danish")
denmark:register()
denmark:set_ideology(monarchy, "absolute_monarchy")
denmark:add_accepted_language(danish)
denmark:add_client_hint(monarchy, _("Denmark"), 0xc27b6d)
denmark:set_flag("westernized", 1.0)

sweden = Nation:new{ ref_name = "sweden" }
sweden.name = _("Sweden")
sweden.adjective = _("Swedish")
sweden.noun = _("Swede")
sweden:register()
sweden:set_ideology(monarchy, "absolute_monarchy")
sweden:add_accepted_language(swedish)
sweden:add_client_hint(monarchy, _("Sweden"), 0x3a739e)
sweden:add_client_hint(socialism, _("People's Republic of Sweden"), 0x3a739e)
sweden:set_flag("westernized", 1.0)

norway = Nation:new{ ref_name = "norway" }
norway.name = _("Norway")
norway.adjective = _("Norwegian")
norway:register()
norway:set_ideology(monarchy, "absolute_monarchy")
norway:add_accepted_language(norwegian)
norway:add_client_hint(monarchy, _("Norway"), 0xab1659)
norway:add_client_hint(communism, _("Norway"), 0xab1659)
norway:add_client_hint(socialism, _("Norway"), 0xab1659)
norway:add_client_hint(fascism, _("Norway"), 0xab1659)
norway:set_flag("westernized", 1.0)

-- Formable nations
scandinavia = Nation:new{ ref_name = "scandinavia" }
scandinavia.name = _("Scandinavia")
scandinavia.adjective = _("Scandinavian")
scandinavia:register()
scandinavia:set_ideology(monarchy, "absolute_monarchy")
scandinavia:add_accepted_language(finnish)
scandinavia:add_accepted_language(norwegian)
scandinavia:add_accepted_language(swedish)
scandinavia:add_accepted_language(danish)
scandinavia:add_client_hint(monarchy, _("Scandinavia"), 0x3a739e)

-- Releasable nations
finland = Nation:new{ ref_name = "finland" }
finland.name = _("Finland")
finland.adjective = _("Finnish")
finland.combo_form = _("Finno")
finland:register()
finland:set_ideology(monarchy, "absolute_monarchy")
finland:add_accepted_language(finnish)
finland:add_client_hint(monarchy, _("Finland"), 0xb9ebeb)

aland = Nation:new{ ref_name = "aland" }
aland.name = _("Åland")
aland:register()
aland:add_accepted_language(finnish)
aland:add_accepted_language(swedish)
aland:add_client_hint(monarchy, _("Åland"), 0xe6dd45)

iceland = Nation:new{ ref_name = "iceland" }
iceland.name = _("Iceland")
iceland.adjective = _("Icelandic")
iceland:register()
iceland:add_accepted_language(icelandic)
iceland:add_client_hint(monarchy, _("Iceland"), 0x8293d1)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Poland, Baltics and European Russia
-- Initial nations
russia = Nation:new{ ref_name = "russia" }
russia.name = _("Russia")
russia.adjective = _("Russian")
russia.combo_form = _("Russo")
russia:register()
russia:set_ideology(monarchy, "absolute_monarchy")
russia:add_accepted_language(russian)
russia:add_client_hint(monarchy, _("Russia"), 0x318c23)
russia:add_client_hint(socialism, _("Russian SFSR"), 0xbf0000)
russia:add_client_hint(republic, _("Russia"), 0xbf0000)

krakow = Nation:new{ ref_name = "krakow" }
krakow.name = _("Krakow")
krakow:register()
krakow:set_ideology(monarchy, "absolute_monarchy")
krakow:add_accepted_language(polish)
krakow:add_client_hint(monarchy, _("Krakow"), 0xe6f4f7)

-- Russian puppet
congress_poland = Nation:new{ ref_name = "congress_poland" }
congress_poland.name = _("Congress Poland")
congress_poland.adjective = _("Polish")
congress_poland:register()
congress_poland:set_ideology(monarchy, "absolute_monarchy")
congress_poland:add_accepted_language(polish)
congress_poland:add_client_hint(monarchy, _("Congress Poland"), 0xbf85d6)
congress_poland:add_client_hint(fascism, _("Congress Poland"), 0xbf85d6)
congress_poland:add_client_hint(republic, _("Congress Poland"), 0xbf85d6)

-- Releasable nations
-- Poland can be formed by krakow
poland = Nation:new{ ref_name = "poland" }
poland.name = _("Poland")
poland.adjective = _("Polish")
poland:register()
poland:add_accepted_language(polish)
poland:add_client_hint(monarchy, _("Poland"), 0xbf85d6)
poland:add_client_hint(socialism, _("Poland"), 0xbf85d6)
poland:add_client_hint(republic, _("Poland"), 0xbf85d6)

lithuania = Nation:new{ ref_name = "lithuania" }
lithuania.name = _("Lithuania")
lithuania.adjective = _("Lithuanian")
lithuania:register()
lithuania:add_accepted_language(lithuanian)
lithuania:add_client_hint(republic, _("Lithuania"), 0x598c42)
lithuania:add_client_hint(socialism, _("Lithuania SFSR"), 0x598c42)

estonia = Nation:new{ ref_name = "estonia" }
estonia.name = _("Estonia")
estonia.adjective = _("Estonian")
estonia:register()
estonia:add_accepted_language(estonian)
estonia:add_client_hint(monarchy, _("Estonia"), 0x4c8ed4)
estonia:add_client_hint(republic, _("Estonia"), 0x4c8ed4)
estonia:add_client_hint(socialism, _("Estonia SFSR"), 0x4c8ed4)

latvia = Nation:new{ ref_name = "latvia" }
latvia.name = _("Latvia")
latvia.adjective = _("Latvian")
latvia:register()
latvia:add_accepted_language(latvian)
latvia:add_client_hint(monarchy, _("Latvia"), 0x962820)
latvia:add_client_hint(republic, _("Latvia"), 0x962820)
latvia:add_client_hint(socialism, _("Latvia SFSR"), 0x962820)

-- Lovonia can be formed by either latvia, estonia or lithuania
livonia = Nation:new{ ref_name = "livonia" }
livonia.name = _("Livonia")
livonia.adjective = _("Livonian")
livonia:register()
livonia:add_accepted_language(lithuanian)
livonia:add_accepted_language(latvian)
livonia:add_accepted_language(estonian)
livonia:add_client_hint(monarchy, _("Livonia"), 0x69917f)

romania = Nation:new{ ref_name = "romania" }
romania.name = _("Romania")
romania.adjective = _("Romanian")
romania:register()
romania:add_accepted_language(bulgarian)
romania:add_client_hint(monarchy, _("Romania"), 0xcfa85b)

bulgaria = Nation:new{ ref_name = "bulgaria" }
bulgaria.name = _("Bulgaria")
bulgaria.adjective = _("Bulgarian")
bulgaria:register()
bulgaria:add_accepted_language(bulgarian)
bulgaria:add_client_hint(socialism, _("Bulgaria"), 0x78cf7d)
bulgaria:add_client_hint(monarchy, _("Bulgaria"), 0x78cf7d)

muscovy = Nation:new{ ref_name = "muscovy" }
muscovy.name = _("Muscovy")
muscovy:register()
muscovy:add_accepted_language(russian)
muscovy:add_client_hint(monarchy, _("Muscovy"), 0x808000)

circassia = Nation:new{ ref_name = "circassia" }
circassia.name = _("Circassia")
circassia.adjective = _("Circassian")
circassia:register()
circassia:add_accepted_language(Language:get("circassian"))
circassia:add_client_hint(monarchy, _("Circassia"), 0x91d1db)

mingrelia = Nation:new{ ref_name = "mingrelia" }
mingrelia.name = _("Mingrelia")
mingrelia.adjective = _("Mingrelia")
mingrelia:register()
mingrelia:add_accepted_language(Language:get("circassian"))
mingrelia:add_client_hint(monarchy, _("Mingrelia"), 0x292080)

-- Avar khanate
avar = Nation:new{ ref_name = "avar" }
avar.name = _("Avar")
avar.adjective = _("Avar")
avar:register()
avar:add_accepted_language(Language:get("circassian"))
avar:add_client_hint(monarchy, _("Avar Khanate"), 0xc24094)

great_horde = Nation:new{ ref_name = "great_horde" }
great_horde.name = _("Great Horde")
great_horde:register()
great_horde:add_accepted_language(Language:get("circassian"))
great_horde:add_client_hint(monarchy, _("Great Horde"), 0x808000)

novgorod = Nation:new{ ref_name = "novgorod" }
novgorod.name = _("Novgorod")
novgorod:register()
novgorod:add_accepted_language(russian)
novgorod:add_accepted_language(finnish)
novgorod:add_client_hint(monarchy, _("Novgorod"), 0x808000)

nogai_horde = Nation:new{ ref_name = "nogai_horde" }
nogai_horde.name = _("Nogai Horde")
nogai_horde.adjective = _("Nogai")
nogai_horde:register()
nogai_horde:add_accepted_language(Language:get("nogai"))
nogai_horde:add_client_hint(monarchy, _("Nogai Horde"), 0x808000)

uzbekistan = Nation:new{ ref_name = "uzbekistan" }
uzbekistan.name = _("Uzbekistan")
uzbekistan.adjective = _("Uzbek")
uzbekistan:register()
uzbekistan:add_accepted_language(Language:get("uzbek"))
uzbekistan:add_client_hint(monarchy, _("Uzbekistan"), 0x808000)

kazakhistan = Nation:new{ ref_name = "kazakhistan" }
kazakhistan.name = _("Kazakhistan")
kazakhistan.adjective = _("Kazakh")
kazakhistan:register()
kazakhistan:add_accepted_language(Language:get("kazakh"))
kazakhistan:add_client_hint(monarchy, _("Kazakhistan"), 0x2276b3)

kyrgyztan = Nation:new{ ref_name = "kyrgyztan" }
kyrgyztan.name = _("Kyrgyztan")
kyrgyztan.adjective = _("Kyrgyz")
kyrgyztan:register()
kyrgyztan:add_accepted_language(Language:get("kyrgyz"))
kyrgyztan:add_client_hint(monarchy, _("Kyrgyztan"), 0xc44f45)
kyrgyztan:add_client_hint(socialism, _("Kyrgyztan"), 0xc44f45)

afghanistan = Nation:new{ ref_name = "afghanistan" }
afghanistan.name = _("Afghanistan")
afghanistan.adjective = _("Afghan")
afghanistan.noun = _("Afghani")
afghanistan:register()
afghanistan:add_accepted_language(Language:get("afghan"))
afghanistan:add_client_hint(monarchy, _("Afghanistan"), 0x3b6b48)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Holy Roman Empire (German part)
-- Initial nations
prussia = Nation:new{ ref_name = "prussia" }
prussia.name = _("Prussia")
prussia.adjective = _("Prussian")
prussia:register()
prussia:set_ideology(monarchy, "absolute_monarchy")
prussia:add_accepted_language(german)
prussia:add_client_hint(socialism, _("Prussia"), 0x405370)
prussia:add_client_hint(communism, _("Prussia"), 0x405370)
prussia:add_client_hint(fascism, _("Prussia"), 0x405370)
prussia:add_client_hint(monarchy, _("Prussia"), 0x405370)

salzburg = Nation:new{ ref_name = "salzburg" }
salzburg.name = _("Salzburg")
salzburg:register()
salzburg:set_ideology(monarchy, "absolute_monarchy")
salzburg:add_accepted_language(german)
salzburg:add_client_hint(monarchy, _("Salzburg"), 0x808000)

thuringia = Nation:new{ ref_name = "thuringia" }
thuringia.name = _("Thuringia")
thuringia:register()
thuringia:set_ideology(monarchy, "absolute_monarchy")
thuringia:add_accepted_language(german)
thuringia:add_client_hint(monarchy, _("Thuringia"), 0xdae4e6)

lubeck = Nation:new{ ref_name = "lubeck" }
lubeck.name = _("Lubeck")
lubeck:register()
lubeck:set_ideology(monarchy, "absolute_monarchy")
lubeck:add_accepted_language(german)
lubeck:add_client_hint(monarchy, _("Hanseatic City of Lubeck"), 0x755446)

waldeck = Nation:new{ ref_name = "waldeck" }
waldeck.name = _("Waldeck")
waldeck:register()
waldeck:set_ideology(monarchy, "absolute_monarchy")
waldeck:add_accepted_language(german)
waldeck:add_client_hint(monarchy, _("Waldeck"), 0xcbe374)

saxony = Nation:new{ ref_name = "saxony" }
saxony.name = _("Saxony")
saxony:register()
saxony:set_ideology(monarchy, "absolute_monarchy")
saxony:add_accepted_language(german)
saxony:add_client_hint(monarchy, _("Saxony"), 0xbde04a)
saxony:add_client_hint(republic, _("Saxony"), 0xbde04a)
saxony:add_client_hint(communism, _("Saxony"), 0xbde04a)
saxony:add_client_hint(socialism, _("Saxony"), 0xbde04a)
saxony:add_client_hint(fascism, _("Saxony"), 0xbde04a)

hesse_kassel = Nation:new{ ref_name = "hesse_kassel" }
hesse_kassel.name = _("Hesse-Kassel")
hesse_kassel:register()
hesse_kassel:set_ideology(monarchy, "absolute_monarchy")
hesse_kassel:add_accepted_language(german)
hesse_kassel:add_client_hint(monarchy, _("Landgraviate of Hesse-Kassel"), 0xd4b54e)

-- Composes all the Reuss-* provinces on thuringa
reuss_gera = Nation:new{ ref_name = "reuss_gera" }
reuss_gera.name = _("Reuss-Gera")
reuss_gera:register()
reuss_gera:set_ideology(monarchy, "absolute_monarchy")
reuss_gera:add_accepted_language(german)
reuss_gera:add_client_hint(monarchy, _("Reuss-Gera"), 0x7a4bb8)

saxe_weimar = Nation:new{ ref_name = "saxe_weimar" }
saxe_weimar.name = _("Saxe-Weimar")
saxe_weimar:register()
saxe_weimar:set_ideology(monarchy, "absolute_monarchy")
saxe_weimar:add_accepted_language(german)
saxe_weimar:add_client_hint(monarchy, _("Saxe-Weimar"), 0x219114)

saxe_gotha = Nation:new{ ref_name = "saxe_gotha" }
saxe_gotha.name = _("Saxe-Gotha")
saxe_gotha:register()
saxe_gotha:set_ideology(monarchy, "absolute_monarchy")
saxe_gotha:add_accepted_language(german)
saxe_gotha:add_client_hint(monarchy, _("Saxe-Gotha"), 0x23e857)

saxe_coburg = Nation:new{ ref_name = "saxe_coburg" }
saxe_coburg.name = _("Saxe-Coburg")
saxe_coburg:register()
saxe_coburg:set_ideology(monarchy, "absolute_monarchy")
saxe_coburg:add_accepted_language(german)
saxe_coburg:add_client_hint(monarchy, _("Saxe-Coburg"), 0xc161ed)

saxe_hildburghausen = Nation:new{ ref_name = "saxe_hildburghausen" }
saxe_hildburghausen.name = _("Saxe-Hildburghausen")
saxe_hildburghausen:register()
saxe_hildburghausen:set_ideology(monarchy, "absolute_monarchy")
saxe_hildburghausen:add_accepted_language(german)
saxe_hildburghausen:add_client_hint(monarchy, _("Saxe-Hildburghausen"), 0xe3c624)

saxe_meiningen = Nation:new{ ref_name = "saxe_meiningen" }
saxe_meiningen.name = _("Saxe-Meiningen")
saxe_meiningen:register()
saxe_meiningen:set_ideology(monarchy, "absolute_monarchy")
saxe_meiningen:add_accepted_language(german)
saxe_meiningen:add_client_hint(monarchy, _("Saxe-Meiningen"), 0xc91e60)

lippe = Nation:new{ ref_name = "lippe" }
lippe.name = _("Lippe")
lippe:register()
lippe:set_ideology(monarchy, "absolute_monarchy")
lippe:add_accepted_language(german)
lippe:add_client_hint(monarchy, _("Lippe"), 0x9c4e7a)

lorraine = Nation:new{ ref_name = "lorraine" }
lorraine.name = _("Lorraine")
lorraine:register()
lorraine:set_ideology(monarchy, "absolute_monarchy")
lorraine:add_accepted_language(german)
lorraine:add_accepted_language(french)
lorraine:add_client_hint(monarchy, _("Lorraine"), 0xeb6b10)

brandenburg = Nation:new{ ref_name = "brandenburg" }
brandenburg.name = _("Brandenburg")
brandenburg:register()
brandenburg:set_ideology(monarchy, "absolute_monarchy")
brandenburg:add_accepted_language(german)
brandenburg:add_client_hint(monarchy, _("Brandenburg"), 0x2f5236)

frankfurt = Nation:new{ ref_name = "frankfurt" }
frankfurt.name = _("Frankfurt")
frankfurt:register()
frankfurt:set_ideology(monarchy, "absolute_monarchy")
frankfurt:add_accepted_language(german)
frankfurt:add_client_hint(monarchy, _("Frankfurt"), 0x105e69)

munich = Nation:new{ ref_name = "munich" }
munich.name = _("Munich")
munich:register()
munich:set_ideology(monarchy, "absolute_monarchy")
munich:add_accepted_language(german)
munich:add_client_hint(monarchy, _("Munich"), 0x787667)

hamburg = Nation:new{ ref_name = "hamburg" }
hamburg.name = _("Hamburg")
hamburg:register()
hamburg:set_ideology(monarchy, "absolute_monarchy")
hamburg:add_accepted_language(german)
hamburg:add_client_hint(monarchy, _("Hamburg"), 0xc44551)

holstein = Nation:new{ ref_name = "holstein" }
holstein.name = _("Holstein")
holstein:register()
holstein:set_ideology(monarchy, "absolute_monarchy")
holstein:add_accepted_language(german)
holstein:add_accepted_language(danish)
holstein:add_client_hint(monarchy, _("Holstein"), 0xe2e3c3)

bavaria = Nation:new{ ref_name = "bavaria" }
bavaria.name = _("Bavaria")
bavaria:register()
bavaria:set_ideology(monarchy, "absolute_monarchy")
bavaria:add_accepted_language(german)
bavaria:add_client_hint(monarchy, _("Bavaria"), 0x8feff2)
bavaria:add_client_hint(communism, _("Bavaria"), 0x8feff2)
bavaria:add_client_hint(socialism, _("Bavaria"), 0x8feff2)

hannover = Nation:new{ ref_name = "hannover" }
hannover.name = _("Hannover")
hannover:register()
hannover:set_ideology(monarchy, "absolute_monarchy")
hannover:add_accepted_language(german)
hannover:add_client_hint(monarchy, _("Hannover"), 0xe8dea1)

luxembourg = Nation:new{ ref_name = "luxembourg" }
luxembourg.name = _("Luxembourg")
luxembourg:register()
luxembourg:set_ideology(monarchy, "absolute_monarchy")
luxembourg:add_accepted_language(french)
luxembourg:add_accepted_language(german)
luxembourg:add_client_hint(monarchy, _("Luxembourg"), 0x45d1ca)

nassau = Nation:new{ ref_name = "nassau" }
nassau.name = _("Nassau")
nassau:register()
nassau:set_ideology(monarchy, "absolute_monarchy")
nassau:add_accepted_language(german)
nassau:add_client_hint(monarchy, _("Nassau"), 0xcc5f29)

baden = Nation:new{ ref_name = "baden" }
baden.name = _("Baden")
baden:register()
baden:set_ideology(monarchy, "absolute_monarchy")
baden:add_accepted_language(german)
baden:add_client_hint(monarchy, _("Baden"), 0xf57915)
baden:add_client_hint(fascism, _("Baden"), 0xf57915)
baden:add_client_hint(communism, _("Baden"), 0xf57915)

brunswick = Nation:new{ ref_name = "brunswick" }
brunswick.name = _("Brunswick")
brunswick:register()
brunswick:set_ideology(monarchy, "absolute_monarchy")
brunswick:add_accepted_language(german)
brunswick:add_client_hint(monarchy, _("Brunswick"), 0xd8709e)

anhalt = Nation:new{ ref_name = "anhalt" }
anhalt.name = _("Anhalt")
anhalt:register()
anhalt:set_ideology(monarchy, "absolute_monarchy")
anhalt:add_accepted_language(german)
anhalt:add_client_hint(monarchy, _("Anhalt"), 0x30cf9f)

oldenburg = Nation:new{ ref_name = "oldenburg" }
oldenburg.name = _("Oldenburg")
oldenburg:register()
oldenburg:set_ideology(monarchy, "absolute_monarchy")
oldenburg:add_accepted_language(german)
oldenburg:add_client_hint(monarchy, _("Oldenburg"), 0xf1d986)

silesia = Nation:new{ ref_name = "silesia" }
silesia.name = _("Silesia")
silesia:register()
silesia:set_ideology(monarchy, "absolute_monarchy")
silesia:add_accepted_language(polish)
silesia:add_accepted_language(german)
silesia:add_client_hint(monarchy, _("Silesia"), 0x41bb81)

danzig = Nation:new{ ref_name = "danzig" }
danzig.name = _("Danzig")
danzig:register()
danzig:set_ideology(monarchy, "absolute_monarchy")
danzig:add_accepted_language(german)
danzig:add_client_hint(monarchy, _("Danzig"), 0xb21aa6)

wolfenbuttel = Nation:new{ ref_name = "wolfenbuttel" }
wolfenbuttel.name = _("Wolfenbüttel")
wolfenbuttel:register()
wolfenbuttel:set_ideology(monarchy, "absolute_monarchy")
wolfenbuttel:add_accepted_language(german)
wolfenbuttel:add_client_hint(monarchy, _("Wolfenbüttel"), 0xbd3270)

bremen = Nation:new{ ref_name = "bremen" }
bremen.name = _("Bremen")
bremen:register()
bremen:set_ideology(monarchy, "absolute_monarchy")
bremen:add_accepted_language(german)
bremen:add_client_hint(monarchy, _("Bremen"), 0x70d8bf)

schaumburg_lippe = Nation:new{ ref_name = "schaumburg_lippe" }
schaumburg_lippe.name = _("Schaumburg-Lippe")
schaumburg_lippe:register()
schaumburg_lippe:set_ideology(monarchy, "absolute_monarchy")
schaumburg_lippe:add_accepted_language(german)
schaumburg_lippe:add_client_hint(monarchy, _("Schaumburg-Lippe"), 0x24878c)

ritzebuttel = Nation:new{ ref_name = "ritzebuttel" }
ritzebuttel.name = _("Ritzebüttel")
ritzebuttel:register()
ritzebuttel:set_ideology(monarchy, "absolute_monarchy")
ritzebuttel:add_accepted_language(german)
ritzebuttel:add_client_hint(monarchy, _("Ritzebüttel"), 0xe06b38)

wurttemberg = Nation:new{ ref_name = "wurttemberg" }
wurttemberg.name = _("Württemberg")
wurttemberg:register()
wurttemberg:set_ideology(monarchy, "absolute_monarchy")
wurttemberg:add_accepted_language(german)
wurttemberg:add_client_hint(monarchy, _("Württemberg"), 0xd6677b)

liechtenstein = Nation:new{ ref_name = "liechtenstein" }
liechtenstein.name = _("Liechtenstein")
liechtenstein:register()
liechtenstein:set_ideology(monarchy, "absolute_monarchy")
liechtenstein:add_accepted_language(german)
liechtenstein:add_client_hint(monarchy, _("Liechtenstein"), 0x8a24ff) 

mecklenburg = Nation:new{ ref_name = "mecklenburg" }
mecklenburg.name = _("Mecklenburg")
mecklenburg:register()
mecklenburg:set_ideology(monarchy, "absolute_monarchy")
mecklenburg:add_accepted_language(german)
mecklenburg:add_client_hint(monarchy, _("Mecklenburg"), 0x77a186) 

grand_duchy_of_hesse_and_by_rhine = Nation:new{ ref_name = "grand_duchy_of_hesse_and_by_rhine" }
grand_duchy_of_hesse_and_by_rhine.name = _("Grand Duchy of Hesse and by Rhine")
grand_duchy_of_hesse_and_by_rhine:register()
grand_duchy_of_hesse_and_by_rhine:set_ideology(monarchy, "absolute_monarchy")
grand_duchy_of_hesse_and_by_rhine:add_accepted_language(german)
grand_duchy_of_hesse_and_by_rhine:add_client_hint(monarchy, _("Grand Duchy of Hesse and by Rhine"), 0x3055cf)

switzerland = Nation:new{ ref_name = "switzerland" }
switzerland.name = _("Switzerland")
switzerland.adjective = _("Swiss")
switzerland:register()
switzerland:set_ideology(monarchy, "absolute_monarchy")
switzerland:add_accepted_language(german)
switzerland:add_client_hint(monarchy, _("Switzerland"), 0xc31703)

austria = Nation:new{ ref_name = "austria" }
austria.name = _("Austria")
austria.adjective = _("Austrian")
austria.combo_form = _("Austro")
austria:register()
austria:set_ideology(monarchy, "absolute_monarchy")
austria:add_accepted_language(german)
austria:add_client_hint(monarchy, _("Austria"), 0xe8ebe8)
austria:add_client_hint(republic, _("Austria"), 0xe8ebe8)

-- Releasable nations
hungary = Nation:new{ ref_name = "hungary" }
hungary.name = _("Hungary")
hungary.adjective = _("Hungarian")
hungary:register()
hungary:set_ideology(monarchy, "absolute_monarchy")
hungary:add_accepted_language(hungarian)
hungary:add_client_hint(socialism, _("People's Republic of Hungary"), 0x3f7051)
hungary:add_client_hint(monarchy, _("Hungary"), 0x3f7051)
hungary:add_client_hint(republic, _("Hungary"), 0x3f7051)

ireland = Nation:new{ ref_name = "ireland" }
ireland.name = _("Ireland")
ireland.adjective = _("Irish")
ireland:register()
ireland:add_accepted_language(irish)
ireland:add_client_hint(monarchy, _("Ireland"), 0x40b225)

-- Formable nations
greater_prussia = Nation:new{ ref_name = "greater_prussia" }
greater_prussia.name = _("Greater Prussia")
greater_prussia.adjective = _("Prussian")
greater_prussia:register()
greater_prussia:add_accepted_language(german)
greater_prussia:add_client_hint(monarchy, _("Greater Prussia"), 0x0c254d)

germany = Nation:new{ ref_name = "germany" }
germany.name = _("Germany")
germany.adjective = _("German")
germany:register()
germany:add_accepted_language(german)
germany:add_client_hint(monarchy, _("Germany"), 0x6e6e6e)
germany:add_client_hint(republic, _("Federal Republic of Germany"), 0x6e6e6e)
germany:add_client_hint(socialism, _("People's Socialist Sate of Germany"), 0x6e6e6e)

holy_roman_empire = Nation:new{ ref_name = "holy_roman_empire" }
holy_roman_empire.name = _("Holy Roman Empire")
holy_roman_empire:register()
holy_roman_empire:add_accepted_language(german)
holy_roman_empire:add_accepted_language(italian)
holy_roman_empire:add_client_hint(monarchy, _("Holy Roman Empire"), 0xf0e930)

roman_empire = Nation:new{ ref_name = "roman_empire" }
roman_empire.name = _("Roman Empire")
roman_empire.adjective = _("Roman")
roman_empire:register()
roman_empire:add_accepted_language(italian)
roman_empire:add_client_hint(monarchy, _("Roman Empire"), 0xbf4f43)

austria_hungary = Nation:new{ ref_name = "austria_hungary" }
austria_hungary.name = _("Austria-Hungary")
austria_hungary.adjective = _("Austro-Hungarian")
austria_hungary:register()
austria_hungary:add_accepted_language(german)
austria_hungary:add_accepted_language(hungarian)
austria_hungary:add_client_hint(monarchy, _("Austria-Hungary"), 0xe8ebe8)
austria_hungary:add_client_hint(fascism, _("Austria-Hungary"), 0xe8ebe8)
austria_hungary:add_client_hint(communism, _("Austria-Hungary"), 0xe8ebe8)
austria_hungary:add_client_hint(republic, _("Austria-Hungary"), 0xe8ebe8)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Holy Roman Empire (Italian part)
-- Initial nations

venice = Nation:new{ ref_name = "venice" }
venice.name = _("Venice")
venice.adjective = _("Venetian")
venice:register()
venice:set_ideology(monarchy, "absolute_monarchy")
venice:add_accepted_language(italian)
venice:add_client_hint(republic, _("Venice"), 0x41bcc4)

genoa = Nation:new{ ref_name = "genoa" }
genoa.name = _("Genoa")
genoa:register()
genoa:set_ideology(monarchy, "absolute_monarchy")
genoa:add_accepted_language(italian)
genoa:add_client_hint(monarchy, _("Genoa"), 0x808000)

papal_states = Nation:new{ ref_name = "papal_states" }
papal_states.name = _("Papal states")
papal_states:register()
papal_states:set_ideology(monarchy, "absolute_monarchy")
papal_states:add_accepted_language(italian)
papal_states:add_client_hint(monarchy, _("Papal states"), 0xf7f6cd)
papal_states:add_client_hint(communism, _("Papal states"), 0xf7f6cd)
papal_states:add_client_hint(socialism, _("Papal states"), 0xf7f6cd)

piedmont_sardinia = Nation:new{ ref_name = "piedmont_sardinia" }
piedmont_sardinia.name = _("Piedmont-Sardinia")
piedmont_sardinia:register()
piedmont_sardinia:set_ideology(monarchy, "absolute_monarchy")
piedmont_sardinia:add_accepted_language(italian)
piedmont_sardinia:add_client_hint(monarchy, _("Piedmont-Sardinia"), 0x873cb5)
piedmont_sardinia:add_client_hint(fascism, _("Piedmont-Sardinia"), 0x873cb5)
piedmont_sardinia:add_client_hint(communism, _("Piedmont-Sardinia"), 0x873cb5)

two_sicilies = Nation:new{ ref_name = "two_sicilies" }
two_sicilies.name = _("Two Sicilies")
two_sicilies:register()
two_sicilies:set_ideology(monarchy, "absolute_monarchy")
two_sicilies:add_accepted_language(italian)
two_sicilies:add_client_hint(monarchy, _("Two Sicilies"), 0x679e7b)
two_sicilies:add_client_hint(communism, _("Two Sicilies"), 0x679e7b)
two_sicilies:add_client_hint(fascism, _("Two Sicilies"), 0x679e7b)

tuscany = Nation:new{ ref_name = "tuscany" }
tuscany.name = _("Tuscany")
tuscany:register()
tuscany:set_ideology(monarchy, "absolute_monarchy")
tuscany:add_accepted_language(italian)
tuscany:add_client_hint(monarchy, _("Tuscany"), 0xd4a88a)
tuscany:add_client_hint(socialism, _("Tuscany"), 0xd4a88a)

parma = Nation:new{ ref_name = "parma" }
parma.name = _("Parma")
parma:register()
parma:set_ideology(monarchy, "absolute_monarchy")
parma:add_accepted_language(italian)
parma:add_client_hint(monarchy, _("Parma"), 0xd48ad3)

modena_and_reggio = Nation:new{ ref_name = "modena_and_reggio" }
modena_and_reggio.name = _("Modena and Reggio")
modena_and_reggio:register()
modena_and_reggio:set_ideology(monarchy, "absolute_monarchy")
modena_and_reggio:add_accepted_language(italian)
modena_and_reggio:add_client_hint(monarchy, _("Modena and Reggio"), 0xd3f2df)

savoie = Nation:new{ ref_name = "savoie" }
savoie.name = _("Savoie")
savoie:register()
savoie:set_ideology(monarchy, "absolute_monarchy")
savoie:add_accepted_language(italian)
savoie:add_client_hint(monarchy, _("Savoie"), 0xad8abd)

san_marino = Nation:new{ ref_name = "san_marino" }
san_marino.name = _("San Marino")
san_marino:register()
san_marino:set_ideology(monarchy, "absolute_monarchy")
san_marino:add_accepted_language(german)
san_marino:add_client_hint(monarchy, _("San Marino"), 0x34e5eb)
san_marino:add_client_hint(republic, _("San Marino"), 0x34e5eb)
san_marino:add_client_hint(communism, _("San Marino"), 0x34e5eb)
san_marino:add_client_hint(socialism, _("San Marino"), 0x34e5eb)
san_marino:add_client_hint(fascism, _("San Marino"), 0x34e5eb)

-- Formable nations
italy = Nation:new{ ref_name = "italy" }
italy.name = _("Italy")
italy.adjective = _("Italian")
italy:register()
italy:add_accepted_language(italian)
italy:add_client_hint(monarchy, _("Italy"), 0x5fbf5c)
italy:add_client_hint(republic, _("Italy"), 0x5fbf5c)

-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Balkans
ottoman = Nation:new{ ref_name = "ottoman" }
ottoman.name = _("Ottoman")
ottoman.adjective = _("Ottoman")
ottoman.noun = _("Ottoman")
ottoman.combo_form = _("Otto")
ottoman:register()
ottoman:set_ideology(monarchy, "absolute_monarchy")
ottoman:add_accepted_language(turkish)
ottoman:add_client_hint(monarchy, _("Sublime Porte"), 0xc9201a)
ottoman:add_client_hint(republic, _("Ottoman Republic"), 0xc9201a)
ottoman:add_client_hint(communism, _("People's Ottoman Republic"), 0xc9201a)
ottoman:add_client_hint(socialism, _("Ottoman"), 0xc9201a)
ottoman:add_client_hint(fascism, _("Sublime Porte"), 0xc9201a)

turkey = Nation:new{ ref_name = "turkey" }
turkey.name = _("Turkey")
turkey.adjective = _("Turkish")
turkey.noun = _("Turk")
turkey.combo_form = _("Turco")
turkey:register()
turkey:set_ideology(monarchy, "absolute_monarchy")
turkey:add_accepted_language(turkish)
turkey:add_client_hint(monarchy, _("Turkey"), 0xc9201a)
turkey:add_client_hint(republic, _("Turkey"), 0xc9201a)
turkey:add_client_hint(communism, _("Turkey"), 0xc9201a)
turkey:add_client_hint(socialism, _("Turkey"), 0xc9201a)
turkey:add_client_hint(fascism, _("Turkey"), 0xc9201a)

greece = Nation:new{ ref_name = "greece" }
greece.name = _("Greece")
greece.adjective = _("Greek")
greece.combo_form = _("Greco")
greece:register()
greece:set_ideology(monarchy, "absolute_monarchy")
greece:add_accepted_language(greek)
greece:add_client_hint(monarchy, _("Greece"), 0xaac4e6)

moldavia = Nation:new{ ref_name = "moldavia" }
moldavia.name = _("Moldavia")
moldavia.adjective = _("Moldavian")
moldavia:register()
moldavia:set_ideology(monarchy, "absolute_monarchy")
moldavia:add_accepted_language(romanian)
moldavia:add_client_hint(socialism, _("Moldavia"), 0xbdc232)
moldavia:add_client_hint(monarchy, _("Moldavia"), 0xbdc232)

wallachia = Nation:new{ ref_name = "wallachia" }
wallachia.name = _("Wallachia")
wallachia:register()
wallachia:set_ideology(monarchy, "absolute_monarchy")
wallachia:add_accepted_language(romanian)
wallachia:add_client_hint(socialism, _("Wallachia"), 0x535096)
wallachia:add_client_hint(monarchy, _("Wallachia"), 0x535096)

-- Releasable nations
kosovo = Nation:new{ ref_name = "kosovo" }
kosovo.name = _("Kosovo")
kosovo:register()
kosovo:add_client_hint(monarchy, _("Kosovo"), 0x808000)

serbia = Nation:new{ ref_name = "serbia" }
serbia.name = _("Serbia")
serbia:register()
serbia:add_accepted_language(serbian)
serbia:add_client_hint(socialism, _("Serbia"), 0x905c4b)
serbia:add_client_hint(monarchy, _("Serbia"), 0x905c4b)

montenegro = Nation:new{ ref_name = "montenegro" }
montenegro.name = _("Montenegro")
montenegro:register()
montenegro:add_accepted_language(serbian)
montenegro:add_client_hint(socialism, _("Montenegro"), 0x808000)
montenegro:add_client_hint(monarchy, _("Montenegro"), 0x808000)

croatia = Nation:new{ ref_name = "croatia" }
croatia.name = _("Croatia")
croatia:register()
croatia:add_accepted_language(croatian)
croatia:add_client_hint(socialism, _("Croatia"), 0x808000)
croatia:add_client_hint(monarchy, _("Croatia"), 0x808000)

bosnia_and_herzegovina = Nation:new{ ref_name = "bosnia_and_herzegovina" }
bosnia_and_herzegovina.name = _("Bosnia and Herzegovina")
bosnia_and_herzegovina:register()
bosnia_and_herzegovina:add_client_hint(monarchy, _("Bosnia and Herzegovina"), 0x808000)
-----------------------------------------------------------------------
