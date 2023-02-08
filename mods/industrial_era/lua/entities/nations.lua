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
--  	nations.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Nations
local nation = {}

nation = Nation:new{ ref_name = "rebel" }
nation.name = _("Rebel")
nation:register()
nation:add_client_hint(monarchy, _("Monarchist Rebellion"), 0x822987)
nation:add_client_hint(communism, _("Communist Rebellion"), 0xf50000)
nation:add_client_hint(socialism, _("Socialist Rebellion"), 0xf50000)
nation:add_client_hint(fascism, _("Fascist Rebellion"), 0x423d3d)
nation:add_client_hint(republic, _("Democratic Rebellion"), 0x002aff)
nation:add_client_hint(anarchy, _("Anarchist Rebellion"), 0x1c1c1c)
nation:add_client_hint(tribal, _("Primitivism Rebellion"), 0x15a100)


-----------------------------------------------------------------------
-- North Africa
-- Initial nations
tunis = Nation:new{ ref_name = "tunis" }
tunis.name = _("Tunis")
tunis.adjective = _("Tunisian")
tunis:register()
tunis:set_ideology(monarchy, "absolute_monarchy")
tunis:add_accepted_language(arabic)
tunis:add_client_hint(monarchy, _("Tunis"), 0xAC433A)

algiers = Nation:new{ ref_name = "algiers" }
algiers.name = _("Algiers")
algiers.adjective = _("Algerian")
algiers:register()
algiers:set_ideology(monarchy, "absolute_monarchy")
algiers:add_accepted_language(arabic)
algiers:add_client_hint(monarchy, _("Algiers"), 0x509095)

abbyssina = Nation:new{ ref_name = "abbyssina" }
abbyssina.name = _("Abbyssina")
abbyssina:register()
abbyssina:set_ideology(monarchy, "absolute_monarchy")
abbyssina:add_client_hint(monarchy, _("Abbyssina"), 0x4761C2)

egypt = Nation:new{ ref_name = "egypt" }
egypt.name = _("Egypt")
egypt.adjective = _("Egyptians")
egypt:register()
egypt:set_ideology(monarchy, "absolute_monarchy")
egypt:add_client_hint(monarchy, _("Egypt"), 0xC1BC73)

morocco = Nation:new{ ref_name = "morocco" }
morocco.name = _("Morocco")
morocco:register()
morocco:set_ideology(monarchy, "absolute_monarchy")
morocco:add_client_hint(monarchy, _("Morocco"), 0x802249)
morocco:set_ideology(monarchy, "absolute_monarchy")

tripolitania = Nation:new{ ref_name = "tripolitania" }
tripolitania.name = _("Tripolitania")
tripolitania.adjective = _("Tripolitanians")
tripolitania:register()
tripolitania:set_ideology(monarchy, "absolute_monarchy")
tripolitania:add_client_hint(monarchy, _("Tripolitania"), 0x91D38A)

ashantee = Nation:new{ ref_name = "ashantee" }
ashantee.name = _("Ashantee")
ashantee:register()
ashantee:add_client_hint(monarchy, _("Ashantee"), 0x9ba324)

-- Formable nations
arabia = Nation:new{ ref_name = "arabia" }
arabia.name = _("Arabia")
arabia.adjective = "Arabic"
arabia.noun = "Arab"
arabia:register()
arabia:add_accepted_language(arabic)
arabia:add_client_hint(monarchy, _("Arabia"), 0x388713)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Western Africa
-- Initial nations
sokoto = Nation:new{ ref_name = "sokoto" }
sokoto.name = _("Sokoto")
sokoto:register()
sokoto:add_accepted_language(arabic)
sokoto:add_client_hint(monarchy, _("Sokoto"), 0x135910)
sokoto:add_client_hint(fascism, _("Sokoto"), 0x135910)
sokoto:add_client_hint(communism, _("Sokoto"), 0x135910)
sokoto:add_client_hint(socialism, _("Sokoto"), 0x135910)

wadai_empire = Nation:new{ ref_name = "wadai_empire" }
wadai_empire.name = _("Wadai Empire")
wadai_empire:register()
wadai_empire:add_client_hint(monarchy, _("Wadai Empire"), 0x808000)

bornu = Nation:new{ ref_name = "bornu" }
bornu.name = _("Bornu")
bornu:register()
bornu:add_client_hint(monarchy, _("Bornu"), 0x58d69b)

jolof = Nation:new{ ref_name = "jolof" }
jolof.name = _("Jolof")
jolof:register()
jolof:add_client_hint(monarchy, _("Jolof"), 0xdce30e)
jolof:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "kayor" }
nation.name = _("Kayor")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Kayor"), 0xb02c20)
nation:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "saloum" }
nation.name = _("Saloum")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Saloum"), 0xd124a9)
nation:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "sine" }
nation.name = _("Sine")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Sine"), 0xe32a19)
nation:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "baol" }
nation.name = _("Baoi")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Baoi"), 0x97d0de)
nation:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "balanta" }
nation.name = _("Balanta")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Balanta"), 0x5b328a)
nation:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "kombo" }
nation.name = _("Kombo")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Kombo"), 0xd9d968)
nation:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "fogny" }
nation.name = _("Fogny")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Fogny"), 0x7ff591)
nation:set_ideology(monarchy, "absolute_monarchy")

dahomey = Nation:new{ ref_name = "dahomey" }
dahomey.name = _("Dahomey")
dahomey:register()
dahomey:add_client_hint(monarchy, _("Dahomey"), 0x3f9bd1)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Eastern Africa
-- Initial nations
burundi = Nation:new{ ref_name = "burundi" }
burundi.name = _("Burundi")
burundi:register()
burundi:add_accepted_language(arabic)
burundi:add_client_hint(monarchy, _("Burundi"), 0x4dd444)
burundi:add_client_hint(fascism, _("Burundi"), 0x4dd444)
burundi:add_client_hint(communism, _("Burundi"), 0x4dd444)
burundi:add_client_hint(socialism, _("Burundi"), 0x4dd444)
burundi:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "buganda" }
nation.name = _("Buganda")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Buganda"), 0x3b495e)
nation:add_client_hint(fascism, _("Buganda"), 0x3b495e)
nation:add_client_hint(communism, _("Buganda"), 0x3b495e)
nation:add_client_hint(socialism, _("Buganda"), 0x3b495e)
nation:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "bunyoro_kitara" }
nation.name = _("Bunyoro-Kitara")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Bunyoro-Kitara"), 0x120096)
nation:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "ankole" }
nation.name = _("Ankole")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Ankole"), 0x634515)
nation:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "rwanda" }
nation.name = _("Rwanda")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Rwanda"), 0xbdf2e6)
nation:set_ideology(monarchy, "absolute_monarchy")

nation = Nation:new{ ref_name = "bokongo" }
nation.name = _("Bokongo")
nation:register()
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Bokongo"), 0xccaa87)
nation:add_client_hint(fascism, _("Bokongo"), 0xccaa87)
nation:add_client_hint(communism, _("Bokongo"), 0xccaa87)
nation:add_client_hint(socialism, _("Bokongo"), 0xccaa87)
nation:set_ideology(monarchy, "absolute_monarchy")

kooki = Nation:new{ ref_name = "kooki" }
kooki.name = _("Kooki")
kooki:register()
kooki:add_accepted_language(arabic)
kooki:add_client_hint(monarchy, _("Kooki"), 0x135910)
kooki:add_client_hint(fascism, _("Kooki"), 0x135910)
kooki:add_client_hint(communism, _("Kooki"), 0x135910)
kooki:add_client_hint(socialism, _("Kooki"), 0x135910)
kooki:set_ideology(monarchy, "absolute_monarchy")
-----------------------------------------------------------------------


--------------------------------------------------------------
-- America
-- Initial nations
argentina = Nation:new{ ref_name = "argentina" }
argentina.name = _("Argentina")
argentina.adjective = "Argentine"
argentina:register()
argentina:set_ideology(republic, "representative_democracy")
argentina:add_accepted_language(spanish)
argentina:add_client_hint(monarchy, _("Argentina"), 0xace3e1)
argentina:add_client_hint(republic, _("Argentina"), 0xace3e1)

brazil = Nation:new{ ref_name = "brazil" }
brazil.name = _("Brazil")
brazil.adjective = "Brazilian"
brazil:register()
brazil:set_ideology(monarchy, "absolute_monarchy")
brazil:add_accepted_language(portuguese)
brazil:add_client_hint(monarchy, _("Brazil"), 0x60c483)

gran_colombia = Nation:new{ ref_name = "gran_colombia" }
gran_colombia.name = _("Gran colombia")
gran_colombia.adjective = "Colombian"
gran_colombia:register()
gran_colombia:set_ideology(republic, "representative_democracy")
gran_colombia:add_accepted_language(spanish)
gran_colombia:add_client_hint(monarchy, _("Gran colombia"), 0xe6e6b5)
gran_colombia:add_client_hint(republic, _("Gran colombia"), 0xe6e6b5)

peru = Nation:new{ ref_name = "peru" }
peru.name = _("Peru")
peru.adjective = "Peruvian"
peru:register()
peru:set_ideology(republic, "representative_democracy")
peru:add_accepted_language(spanish)
peru:add_client_hint(monarchy, _("Peru"), 0xe63d2e)
peru:add_client_hint(republic, _("Peru"), 0xe63d2e)

bolivia = Nation:new{ ref_name = "bolivia" }
bolivia.name = _("Bolivia")
bolivia.adjective = _("Bolivian")
bolivia:register()
bolivia:set_ideology(republic, "representative_democracy")
bolivia:add_accepted_language(spanish)
bolivia:add_client_hint(republic, _("Bolivia"), 0xded6ba)
bolivia:add_client_hint(monarchy, _("Bolivia"), 0xded6ba)

paraguay = Nation:new{ ref_name = "paraguay" }
paraguay.name = _("Paraguay")
paraguay.adjective = _("Paraguayan")
paraguay:register()
paraguay:set_ideology(republic, "representative_democracy")
paraguay:add_accepted_language(spanish)
paraguay:add_client_hint(monarchy, _("Paraguay"), 0x99c2bf)
paraguay:add_client_hint(republic, _("Paraguay"), 0x99c2bf)

chile = Nation:new{ ref_name = "chile" }
chile.name = _("Chile")
chile.adjective = _("Chilean")
chile:register()
chile:set_ideology(republic, "representative_democracy")
chile:add_accepted_language(spanish)
chile:add_client_hint(monarchy, _("Chile"), 0xad6dcf)
chile:add_client_hint(republic, _("Chile"), 0xad6dcf)

upca = Nation:new{ ref_name = "upca" }
upca.name = _("United Provinces of Central America")
upca:register()
upca:set_ideology(republic, "representative_democracy")
upca:add_accepted_language(spanish)
upca:add_client_hint(republic, _("United Provinces of Central America"), 0x5bb2cf)
upca:add_client_hint(communism, _("United Provinces of Central America"), 0x5bb2cf)
upca:add_client_hint(socialism, _("United Provinces of Central America"), 0x5bb2cf)

miskito_coast = Nation:new{ ref_name = "miskito_coast" }
miskito_coast.name = _("Miskito Coast")
miskito_coast:register()
miskito_coast:set_ideology(monarchy, "absolute_monarchy")
miskito_coast:add_accepted_language(spanish)
miskito_coast:add_client_hint(monarchy, _("Miskito Coast"), 0xd49285)

mexico = Nation:new{ ref_name = "mexico" }
mexico.name = _("Mexico")
mexico.adjective = _("Mexican")
mexico:register()
mexico:set_ideology(republic, "representative_democracy")
mexico:add_accepted_language(spanish)
mexico:add_client_hint(republic, _("Mexico"), 0x5ac780)
mexico:add_client_hint(monarchy, _("Mexican Empire"), 0x5ac780)

usa = Nation:new{ ref_name = "usa" }
usa.name = _("United states")
usa.adjective = _("American")
usa.noun = _("American")
usa:register()
usa:set_ideology(republic, "representative_democracy")
usa:add_accepted_language(spanish)
usa:add_accepted_language(english)
usa:add_client_hint(republic, _("United States of America"), 0x6ea5cc)
usa:add_client_hint(fascism, _("United States of America"), 0x6ea5cc)
usa:add_client_hint(monarchy, _("United States of America"), 0x6ea5cc)
usa:add_client_hint(socialism, _("United States of America"), 0x6ea5cc)
usa:add_client_hint(communism, _("United States of America"), 0x6ea5cc)

nation = Nation:new{ ref_name = "apache" }
nation.name = _("Apache")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Apache"), 0xf1f2c7)

nation = Nation:new{ ref_name = "ute" }
nation.name = _("Ute")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Ute"), 0x782656)

nation = Nation:new{ ref_name = "yavapai" }
nation.name = _("Yavapai")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Yavapai"), 0xa51dcf)

nation = Nation:new{ ref_name = "comanche" }
nation.name = _("Comanche")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Comanche"), 0x154fa1)

nation = Nation:new{ ref_name = "cheyenne" }
nation.name = _("Cheyenne")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Cheyenne"), 0x140a80)

nation = Nation:new{ ref_name = "kiowa" }
nation.name = _("Kiowa")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Kiowa"), 0x96410f)

nation = Nation:new{ ref_name = "huvasupai" }
nation.name = _("Huvasupai")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Huvasupai"), 0xa82c3c)

nation = Nation:new{ ref_name = "hopi" }
nation.name = _("Hopi")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Hopi"), 0xf20c2a)

nation = Nation:new{ ref_name = "navajo" }
nation.name = _("Navajo")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Navajo"), 0x38594f)

nation = Nation:new{ ref_name = "tohono_o_odham" }
nation.name = _("Tohono O'odham")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Tohono O'odham"), 0xcf9a70)

nation = Nation:new{ ref_name = "quecha" }
nation.name = _("Quecha")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Quecha"), 0xb6de52)

nation = Nation:new{ ref_name = "mojave" }
nation.name = _("Mojave")
nation:register()
nation:set_ideology(tribal, "nomadic_tribe")
nation:add_accepted_language(spanish)
nation:add_client_hint(tribal, _("Mojave"), 0x8f170a)

haiti = Nation:new{ ref_name = "haiti" }
haiti.name = _("Haiti")
haiti:register()
haiti:set_ideology(republic, "representative_democracy")
haiti:add_accepted_language(french)
haiti:add_client_hint(republic, _("Haiti"), 0xb147cc)
haiti:add_client_hint(monarchy, _("Hayti"), 0xb147cc)

mapuche = Nation:new{ ref_name = "mapuche" }
mapuche.name = _("Mapuche")
mapuche:register()
mapuche:set_ideology(tribal, "hierachical_tribe")
mapuche:add_accepted_language(mapuche)
mapuche:add_client_hint(tribal, _("Mapuche"), 0x8c9ebd)

canada = Nation:new{ ref_name = "canada" }
canada.name = _("Canada")
canada:register()
canada:set_ideology(monarchy, "absolute_monarchy")
canada:add_accepted_language(english)
canada:add_client_hint(monarchy, _("Canada"), 0x5f9376)

upper_canada = Nation:new{ ref_name = "upper_canada" }
upper_canada.name = _("Upper Canada")
upper_canada:register()
upper_canada:set_ideology(monarchy, "absolute_monarchy")
upper_canada:add_accepted_language(english)
upper_canada:add_client_hint(monarchy, _("Upper Canada"), 0xaa2913)

lower_canada = Nation:new{ ref_name = "lower_canada" }
lower_canada.name = _("Lower Canada")
lower_canada:register()
lower_canada:set_ideology(monarchy, "absolute_monarchy")
lower_canada:add_accepted_language(english)
lower_canada:add_client_hint(monarchy, _("Lower Canada"), 0xdf1f1f)

prince_edward_island = Nation:new{ ref_name = "prince_edward_island" }
prince_edward_island.name = _("Prince Edward Island")
prince_edward_island:register()
prince_edward_island:set_ideology(monarchy, "absolute_monarchy")
prince_edward_island:add_accepted_language(english)
prince_edward_island:add_client_hint(monarchy, _("Prince Edward Island"), 0xdf1f1f)

nova_scotia = Nation:new{ ref_name = "nova_scotia" }
nova_scotia.name = _("Nova Scotia")
nova_scotia:register()
nova_scotia:set_ideology(monarchy, "absolute_monarchy")
nova_scotia:add_accepted_language(english)
nova_scotia:add_client_hint(monarchy, _("Nova Scotia"), 0xdf1f1f)

new_brunswick = Nation:new{ ref_name = "new_brunswick" }
new_brunswick.name = _("New Brunswick")
new_brunswick:register()
new_brunswick:set_ideology(monarchy, "absolute_monarchy")
new_brunswick:add_accepted_language(english)
new_brunswick:add_client_hint(monarchy, _("New Brunswick"), 0xdf1f1f)

ruperts_land = Nation:new{ ref_name = "ruperts_land" }
ruperts_land.name = _("Rupert's Land")
ruperts_land:register()
ruperts_land:set_ideology(monarchy, "absolute_monarchy")
ruperts_land:add_accepted_language(english)
ruperts_land:add_client_hint(monarchy, _("Rupert's Land"), 0xeb8f8f)

-- Releasable nations
texas = Nation:new{ ref_name = "texas" }
texas.name = _("Texas")
texas.adjective = _("Texan")
texas:register()
texas:add_accepted_language(english)
texas:add_accepted_language(spanish)
texas:add_client_hint(republic, _("Texas"), 0x8be2f7)
texas:add_client_hint(fascism, _("Texas"), 0x8be2f7)
texas:add_client_hint(socialism, _("Texas"), 0x8be2f7)
texas:add_client_hint(communism, _("Texas"), 0x8be2f7)
texas:add_client_hint(monarchy, _("Texas"), 0x8be2f7)

csa = Nation:new{ ref_name = "csa" }
csa.name = _("Confederate States of America")
csa.adjective = _("Confederate")
csa:register()
csa:add_accepted_language(english)
csa:add_client_hint(republic, _("Confederate States of America"), 0x8be2f7)
csa:add_client_hint(monarchy, _("Confederate States of America"), 0x8be2f7)

uruguay = Nation:new{ ref_name = "uruguay" }
uruguay.name = _("Uruguay")
uruguay.adjective = _("Uruguayan")
uruguay:register()
uruguay:add_accepted_language(spanish)
uruguay:add_client_hint(monarchy, _("Uruguay"), 0xdce6e8)
uruguay:add_client_hint(republic, _("Uruguay"), 0xdce6e8)

panama = Nation:new{ ref_name = "panama" }
panama.name = _("Panama")
panama.adjective = _("Panaman")
panama:register()
panama:add_accepted_language(spanish)
panama:add_client_hint(monarchy, _("Panama"), 0xa8a8e3)
panama:add_client_hint(republic, _("Panama"), 0xa8a8e3)
--------------------------------------------------------------


nation = Nation:new{ ref_name = "joseon", name = _("Joseon") }
nation.adjective = _("Korean")
nation:register()
nation:add_accepted_language(Language:get("korean"))
nation:add_client_hint(monarchy, _("Korea"), 0xa80a3c)

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

nation = Nation:new{ ref_name = "tibet", name = _("Tibet") }
nation.adjective = _("Chinesse")
nation:register()
nation:add_accepted_language(Language:get("chinesse"))
nation:add_client_hint(monarchy, _("Tibet"), 0xe8841a)
nation:add_client_hint(communism, _("Tibet"), 0xe8841a)

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
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("chinesse"))
nation:add_client_hint(monarchy, _("Johor"), 0x896241)
nation:add_client_hint(fascism, _("Johor"), 0x896241)
nation:add_client_hint(republic, _("Johor"), 0x896241)
nation:add_client_hint(socialism, _("Johor"), 0x896241)

nation = Nation:new{ ref_name = "siam", name = _("Siam") }
nation.adjective = _("Siamesse")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("thai"))
nation:add_client_hint(monarchy, _("Siam"), 0x2F54BD)

nation = Nation:new{ ref_name = "burma", name = _("Burma") }
nation.adjective = _("Burmesse")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("burmesse"))
nation:add_client_hint(monarchy, _("Burma"), 0x2FFFBD)

nation = Nation:new{ ref_name = "vietnam", name = _("Vietnam") }
nation.adjective = _("Vietnamesse")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Vietnam"), 0x857732)
nation:add_client_hint(socialism, _("Vietnam"), 0xF57732)
nation:add_client_hint(communism, _("Vietnam"), 0xF57732)

nation = Nation:new{ ref_name = "champa", name = _("Champa") }
nation.adjective = _("Cambodian")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Champa"), 0x512f91)

nation = Nation:new{ ref_name = "east_india_company", name = _("East India Company") }
nation.adjective = _("British Indian")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("East India Company"), 0xdb5656)

nation = Nation:new{ ref_name = "mysore", name = _("Mysore") }
nation.adjective = _("Mysorian")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Mysore"), 0x512f91)

nation = Nation:new{ ref_name = "haydarabad", name = _("Haydarabad") }
nation.adjective = _("Haydarabad")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Haydarabad"), 0x512f91)

nation = Nation:new{ ref_name = "awadh", name = _("Awadh") }
nation.adjective = _("Awadh")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Awadh"), 0x512f91)

nation = Nation:new{ ref_name = "bhutan", name = _("Bhutan") }
nation.adjective = _("Bhutan")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Bhutan"), 0x512f91)

nation = Nation:new{ ref_name = "nepal", name = _("Nepal") }
nation.adjective = _("Nepal")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Nepal"), 0x512f91)

nation = Nation:new{ ref_name = "kashmir", name = _("Kashmir") }
nation.adjective = _("Kashmir")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Kashmir"), 0x512f91)

nation = Nation:new{ ref_name = "berah", name = _("Berah") }
nation.adjective = _("Berah")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Berah"), 0xb86339)

nation = Nation:new{ ref_name = "sindh", name = _("Sindh") }
nation.adjective = _("Sindh")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Sindh"), 0x375c38)

nation = Nation:new{ ref_name = "sikh", name = _("Sikh") }
nation.adjective = _("Sikh")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("vietnamesse"))
nation:add_client_hint(monarchy, _("Sikh"), 0xd65718)

nation = Nation:new{ ref_name = "bukhara", name = _("Bukhara") }
nation.adjective = _("Bukhara")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("uzbek"))
nation:add_client_hint(monarchy, _("Bukhara"), 0x25a825)

nation = Nation:new{ ref_name = "khiva", name = _("Khiva") }
nation.adjective = _("Khiva")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("uzbek"))
nation:add_client_hint(monarchy, _("Khiva"), 0x506d70)

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

nation = Nation:new{ ref_name = "piedmont_sardinia" }
nation.name = _("Piedmont-Sardinia")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(italian)
nation:add_client_hint(monarchy, _("Piedmont-Sardinia"), 0x873cb5)
nation:add_client_hint(fascism, _("Piedmont-Sardinia"), 0x873cb5)
nation:add_client_hint(communism, _("Piedmont-Sardinia"), 0x873cb5)

nation = Nation:new{ ref_name = "two_sicilies" }
nation.name = _("Two Sicilies")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(italian)
nation:add_client_hint(monarchy, _("Two Sicilies"), 0x679e7b)
nation:add_client_hint(communism, _("Two Sicilies"), 0x679e7b)
nation:add_client_hint(fascism, _("Two Sicilies"), 0x679e7b)

nation = Nation:new{ ref_name = "tuscany" }
nation.name = _("Tuscany")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(italian)
nation:add_client_hint(monarchy, _("Tuscany"), 0xd4a88a)
nation:add_client_hint(socialism, _("Tuscany"), 0xd4a88a)

nation = Nation:new{ ref_name = "parma" }
nation.name = _("Parma")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(italian)
nation:add_client_hint(monarchy, _("Parma"), 0xd48ad3)

nation = Nation:new{ ref_name = "modena_and_reggio" }
nation.name = _("Modena and Reggio")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(italian)
nation:add_client_hint(monarchy, _("Modena and Reggio"), 0xd3f2df)

nation = Nation:new{ ref_name = "savoie" }
nation.name = _("Savoie")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(italian)
nation:add_client_hint(monarchy, _("Savoie"), 0xad8abd)

nation = Nation:new{ ref_name = "san_marino" }
nation.name = _("San Marino")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(german)
nation:add_client_hint(monarchy, _("San Marino"), 0x34e5eb)
nation:add_client_hint(republic, _("San Marino"), 0x34e5eb)
nation:add_client_hint(communism, _("San Marino"), 0x34e5eb)
nation:add_client_hint(socialism, _("San Marino"), 0x34e5eb)
nation:add_client_hint(fascism, _("San Marino"), 0x34e5eb)

-- Formable nations
nation = Nation:new{ ref_name = "italy" }
nation.name = _("Italy")
nation.adjective = _("Italian")
nation:register()
nation:add_accepted_language(italian)
nation:add_client_hint(monarchy, _("Italy"), 0x5fbf5c)
nation:add_client_hint(republic, _("Italy"), 0x5fbf5c)

-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Balkans
nation = Nation:new{ ref_name = "ottoman" }
nation.name = _("Ottoman")
nation.adjective = _("Ottoman")
nation.noun = _("Ottoman")
nation.combo_form = _("Otto")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(turkish)
nation:add_client_hint(monarchy, _("Sublime Porte"), 0xc9201a)
nation:add_client_hint(republic, _("Ottoman Republic"), 0xc9201a)
nation:add_client_hint(communism, _("People's Ottoman Republic"), 0xc9201a)
nation:add_client_hint(socialism, _("Ottoman"), 0xc9201a)
nation:add_client_hint(fascism, _("Sublime Porte"), 0xc9201a)

nation = Nation:new{ ref_name = "turkey" }
nation.name = _("Turkey")
nation.adjective = _("Turkish")
nation.noun = _("Turk")
nation.combo_form = _("Turco")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(turkish)
nation:add_client_hint(monarchy, _("Turkey"), 0xc9201a)
nation:add_client_hint(republic, _("Turkey"), 0xc9201a)
nation:add_client_hint(communism, _("Turkey"), 0xc9201a)
nation:add_client_hint(socialism, _("Turkey"), 0xc9201a)
nation:add_client_hint(fascism, _("Turkey"), 0xc9201a)

nation = Nation:new{ ref_name = "greece" }
nation.name = _("Greece")
nation.adjective = _("Greek")
nation.combo_form = _("Greco")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(greek)
nation:add_client_hint(monarchy, _("Greece"), 0xaac4e6)

nation = Nation:new{ ref_name = "moldavia" }
nation.name = _("Moldavia")
nation.adjective = _("Moldavian")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(romanian)
nation:add_client_hint(socialism, _("Moldavia"), 0xbdc232)
nation:add_client_hint(monarchy, _("Moldavia"), 0xbdc232)

nation = Nation:new{ ref_name = "wallachia" }
nation.name = _("Wallachia")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(romanian)
nation:add_client_hint(socialism, _("Wallachia"), 0x535096)
nation:add_client_hint(monarchy, _("Wallachia"), 0x535096)

-- Releasable nations
nation = Nation:new{ ref_name = "kosovo" }
nation.name = _("Kosovo")
nation:register()
nation:add_client_hint(monarchy, _("Kosovo"), 0x808000)

nation = Nation:new{ ref_name = "serbia" }
nation.name = _("Serbia")
nation:register()
nation:add_accepted_language(serbian)
nation:add_client_hint(socialism, _("Serbia"), 0x905c4b)
nation:add_client_hint(monarchy, _("Serbia"), 0x905c4b)

nation = Nation:new{ ref_name = "montenegro" }
nation.name = _("Montenegro")
nation:register()
nation:add_accepted_language(serbian)
nation:add_client_hint(socialism, _("Montenegro"), 0x808000)
nation:add_client_hint(monarchy, _("Montenegro"), 0x808000)

nation = Nation:new{ ref_name = "croatia" }
nation.name = _("Croatia")
nation:register()
nation:add_accepted_language(croatian)
nation:add_client_hint(socialism, _("Croatia"), 0x808000)
nation:add_client_hint(monarchy, _("Croatia"), 0x808000)

nation = Nation:new{ ref_name = "bosnia_and_herzegovina" }
nation.name = _("Bosnia and Herzegovina")
nation:register()
nation:add_client_hint(monarchy, _("Bosnia and Herzegovina"), 0x808000)
-----------------------------------------------------------------------


nation = Nation:new{ ref_name = "najran", name = _("Najran") }
nation.adjective = _("Najrani")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Najran"), 0xDEED82)
nation:add_client_hint(communism, _("Najran"), 0xDEED82)
nation:add_client_hint(fascism, _("Najran"), 0xDEED82)
nation:add_client_hint(republic, _("Najran"), 0xDEED82)

nation = Nation:new{ ref_name = "zaidi_imamate", name = _("Zaidi Imamate") }
nation.adjective = _("Zaidi")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Zaidi Imamate"), 0x8e332f)
nation:add_client_hint(communism, _("Zaidi Imamate"), 0x8e332f)
nation:add_client_hint(fascism, _("Zaidi Imamate"), 0x8e332f)
nation:add_client_hint(republic, _("Zaidi Imamate"), 0x8e332f)

nation = Nation:new{ ref_name = "oman", name = _("Oman") }
nation.adjective = _("Omani")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Oman"), 0x3B3E69)
nation:add_client_hint(communism, _("Oman"), 0x3B3E69)
nation:add_client_hint(fascism, _("Oman"), 0x3B3E69)
nation:add_client_hint(republic, _("Oman"), 0x3B3E69)

nation = Nation:new{ ref_name = "nejd", name = _("Nejd") }
nation.adjective = _("Nejdi")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Nejd"), 0x314E2A)
nation:add_client_hint(communism, _("Nejd"), 0x314E2A)
nation:add_client_hint(fascism, _("Nejd"), 0x314E2A)
nation:add_client_hint(republic, _("Nejd"), 0x314E2A)

nation = Nation:new{ ref_name = "mahra", name = _("Mahra") }
nation.adjective = _("Mahrai")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Mahra"), 0x52AC3B)
nation:add_client_hint(communism, _("Mahra"), 0x52AC3B)
nation:add_client_hint(fascism, _("Mahra"), 0x52AC3B)
nation:add_client_hint(republic, _("Mahra"), 0x52AC3B)

nation = Nation:new{ ref_name = "muscat", name = _("Muscat") }
nation.adjective = _("Muscati")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Muscat"), 0xE77A35)
nation:add_client_hint(communism, _("Muscat"), 0xE77A35)
nation:add_client_hint(fascism, _("Muscat"), 0xE77A35)
nation:add_client_hint(republic, _("Muscat"), 0xE77A35)

nation = Nation:new{ ref_name = "kasadi", name = _("Kasadi") }
nation.adjective = _("Kasadi")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Kasadi"), 0xE73537)

nation = Nation:new{ ref_name = "mahwali", name = _("Mahwali") }
nation.adjective = _("Mahwali")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Mahwali"), 0x8A35E7)

nation = Nation:new{ ref_name = "wahidi", name = _("Wahidi") }
nation.adjective = _("Wahidi")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Wahidi"), 0x3599E7)

nation = Nation:new{ ref_name = "wadi_hajr", name = _("Wadi Hajr") }
nation.adjective = _("Wadi Hajr")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Wadi Hajr"), 0xC2E735)

nation = Nation:new{ ref_name = "tamim", name = _("Tamim") }
nation.adjective = _("Tamim")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Tamim"), 0xA71256)

nation = Nation:new{ ref_name = "hamum", name = _("Hamum") }
nation.adjective = _("Hamum")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Hamum"), 0x63D03E)

nation = Nation:new{ ref_name = "aulaqi", name = _("Aulaqi") }
nation.adjective = _("Aulaqi")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Aulaqi"), 0xd0cc3e)

nation = Nation:new{ ref_name = "fadhli", name = _("Fadhli") }
nation.adjective = _("Fadhli")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Fadhli"), 0x53f1f6)

nation = Nation:new{ ref_name = "beihan", name = _("Beihan") }
nation.adjective = _("Beihani")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Beihan"), 0x682f8e)

nation = Nation:new{ ref_name = "dhale", name = _("Dhale") }
nation.adjective = _("Dhalian")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Dhale"), 0xff00d0)

nation = Nation:new{ ref_name = "upper_yafa", name = _("Upper Yafa") }
nation.adjective = _("Yafanian")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Upper Yafa"), 0x9D7DED)

nation = Nation:new{ ref_name = "lower_yafa", name = _("Lower Yafa") }
nation.adjective = _("Yafanian")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Lower Yafa"), 0x9D7DED)

nation = Nation:new{ ref_name = "beda", name = _("Beda") }
nation.adjective = _("Bedan")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Beda"), 0x2C5C89)

nation = Nation:new{ ref_name = "audhali", name = _("Audhali") }
nation.adjective = _("Audhali")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Audhali"), 0x2C8931)

nation = Nation:new{ ref_name = "atifi", name = _("Atifi") }
nation.adjective = _("Atifi")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Atifi"), 0x9DB4AD)

nation = Nation:new{ ref_name = "kaluli", name = _("Kaluli") }
nation.adjective = _("Kaluli")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Kaluli"), 0x652A7F)

nation = Nation:new{ ref_name = "aghbari", name = _("Aghbari") }
nation.adjective = _("Aghbari")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Aghbari"), 0xAE28E8)

nation = Nation:new{ ref_name = "barhimi", name = _("Barhimi") }
nation.adjective = _("Barhimi")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Barhimi"), 0x2875E8)

nation = Nation:new{ ref_name = "aqrabi", name = _("Aqrabi") }
nation.adjective = _("Aqrabi")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Aqrabi"), 0xBC4721)

nation = Nation:new{ ref_name = "rujai", name = _("Rujai") }
nation.adjective = _("Rujai")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(arabic)
nation:add_client_hint(monarchy, _("Rujai"), 0xE32764)


hawai_i = Nation:new{ ref_name = "hawai_i" }
hawai_i.name = _("Hawai'i")
hawai_i:register()
hawai_i:set_ideology(monarchy, "absolute_monarchy")
hawai_i:add_accepted_language(polynesian)
hawai_i:add_client_hint(monarchy, _("Hawai'i"), 0x219462)
hawai_i:add_client_hint(communism, _("Hawai'i"), 0x219462)
hawai_i:add_client_hint(socialism, _("Hawai'i"), 0x219462)
hawai_i:add_client_hint(fascism, _("Hawai'i"), 0x219462)
hawai_i:add_client_hint(republic, _("Hawai'i"), 0x219462)

nation = Nation:new{ ref_name = "aceh", name = _("Aceh") }
nation.adjective = _("Aceh")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Aceh"), 0xC82344)
nation:add_client_hint(fascism, _("Aceh"), 0xC82344)
nation:add_client_hint(republic, _("Aceh"), 0xC82344)
nation:add_client_hint(socialism, _("Aceh"), 0xC82344)

nation = Nation:new{ ref_name = "perak", name = _("Perak") }
nation.adjective = _("Perak")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Perak"), 0x85ACD6)

nation = Nation:new{ ref_name = "pagaruyung", name = _("Pagaruyung") }
nation.adjective = _("Pagaruyung")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Pagaruyung"), 0xB5832F)

nation = Nation:new{ ref_name = "deli", name = _("Deli") }
nation.adjective = _("Deli")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Deli"), 0xB5382F)

nation = Nation:new{ ref_name = "siak", name = _("Siak Sri Indrapura") }
nation.adjective = _("Siak")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Siak Sri Indrapura"), 0x7046AF)

nation = Nation:new{ ref_name = "sulu", name = _("Sulu") }
nation.adjective = _("Sulu")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Sulu"), 0x374A82)
nation:add_client_hint(fascism, _("Sulu"), 0x374A82)
nation:add_client_hint(republic, _("Sulu"), 0x374A82)
nation:add_client_hint(socialism, _("Sulu"), 0x374A82)

nation = Nation:new{ ref_name = "bali", name = _("Bali") }
nation.adjective = _("Bali")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Kingdom of Bali"), 0x494B82)

nation = Nation:new{ ref_name = "lombok", name = _("Lombok") }
nation.adjective = _("Lombok")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Lombok"), 0x7D251B)

nation = Nation:new{ ref_name = "bima", name = _("Bima") }
nation.adjective = _("Bima")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Bima"), 0xC31E3A)

nation = Nation:new{ ref_name = "brunei", name = _("Brunei") }
nation.adjective = _("Brunei")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Brunei"), 0xFBFF41)

nation = Nation:new{ ref_name = "selangor", name = _("Selangor") }
nation.adjective = _("Selangor")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("malay"))
nation:add_client_hint(monarchy, _("Selangor"), 0xD62282)

nation = Nation:new{ ref_name = "tonga", name = _("Tonga") }
nation.adjective = _("Tonga")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("polynesian"))
nation:add_client_hint(monarchy, _("Tonga"), 0xD62282)

nation = Nation:new{ ref_name = "alo", name = _("Alo") }
nation.adjective = _("Alo")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("polynesian"))
nation:add_client_hint(monarchy, _("Alo"), 0xD62282)

nation = Nation:new{ ref_name = "uvea", name = _("Uvea") }
nation.adjective = _("Uvea")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("polynesian"))
nation:add_client_hint(monarchy, _("Uvea"), 0xD62282)

nation = Nation:new{ ref_name = "samoa", name = _("Samoa") }
nation.adjective = _("Samoa")
nation:register()
nation:set_ideology(monarchy, "absolute_monarchy")
nation:add_accepted_language(Language:get("polynesian"))
nation:add_client_hint(monarchy, _("Samoa"), 0xD62282)
