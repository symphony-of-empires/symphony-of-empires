-----------------------------------------------------------------------
-- Europe (excluding balkans, scandinavia and the mess that Germany was)
-- Initial nations
united_kingdom = Nation:create{ ref_name = "united_kingdom", color = 0xc22a25 }
united_kingdom.name = _("United Kingdom")
Nation:register(united_kingdom)
Nation:add_accepted_culture(united_kingdom, british)
Nation:add_accepted_culture(united_kingdom, irish)
netherlands = Nation:create{ ref_name = "netherlands", color = 0xe6964c }
netherlands.name = _("Netherlands")
Nation:register(netherlands)
Nation:add_accepted_culture(netherlands, dutch)
france = Nation:create{ ref_name = "france", color = 0x7167c9 }
france.name = _("France")
Nation:register(france)
Nation:add_accepted_culture(france, french)
spain = Nation:create{ ref_name = "spain", color = 0xe8c879 }
spain.name = _("Spain")
Nation:register(spain)
Nation:add_accepted_culture(spain, spanish)
portugal = Nation:create{ ref_name = "portugal", color = 0xe8c879 }
portugal.name = _("Portugal")
Nation:register(portugal)
Nation:add_accepted_culture(portugal, portuguese)
-- Formable nations
european_union = Nation:create{ ref_name = "european_union", color = 0xe8c879 }
european_union.name = _("European Union")
Nation:register(european_union)
Nation:add_accepted_culture(european_union, catalan)
Nation:add_accepted_culture(european_union, basque)
Nation:add_accepted_culture(european_union, galician)
Nation:add_accepted_culture(european_union, aragonese)
Nation:add_accepted_culture(european_union, portuguese)
Nation:add_accepted_culture(european_union, spanish)
Nation:add_accepted_culture(european_union, german)
Nation:add_accepted_culture(european_union, polish)
Nation:add_accepted_culture(european_union, czech)
Nation:add_accepted_culture(european_union, slovakian)
Nation:add_accepted_culture(european_union, french)
Nation:add_accepted_culture(european_union, italian)
Nation:add_accepted_culture(european_union, greek)
Nation:add_accepted_culture(european_union, danish)
Nation:add_accepted_culture(european_union, british)
Nation:add_accepted_culture(european_union, irish)
Nation:add_accepted_culture(european_union, portuguese)
Nation:add_accepted_culture(european_union, hungarian)
iberia = Nation:create{ ref_name = "iberia", color = 0xe8c879 }
iberia.name = _("Iberia")
Nation:register(iberia)
Nation:add_accepted_culture(iberia, catalan)
Nation:add_accepted_culture(iberia, basque)
Nation:add_accepted_culture(iberia, galician)
Nation:add_accepted_culture(iberia, aragonese)
Nation:add_accepted_culture(iberia, portuguese)
Nation:add_accepted_culture(iberia, spanish)
-- Releasable nations
brittany = Nation:create{ ref_name = "brittany", color = 0xe8c879 }
brittany.name = _("Brittany")
Nation:register(brittany)
normandy = Nation:create{ ref_name = "normandy", color = 0xe8c879 }
normandy.name = _("Normandy")
Nation:register(normandy)
catalonia = Nation:create{ ref_name = "catalonia", color = 0xe8c879 }
catalonia.name = _("Catalonia")
Nation:register(catalonia)
Nation:add_accepted_culture(catalonia, catalan)
basque_country = Nation:create{ ref_name = "basque", color = 0xe8c879 }
basque_country.name = _("Basque")
Nation:register(basque_country)
Nation:add_accepted_culture(basque_country, basque)
galicia = Nation:create{ ref_name = "galicia", color = 0xe8c879 }
galicia.name = _("Galicia")
Nation:register(galicia)
Nation:add_accepted_culture(galicia, galician)
aragon = Nation:create{ ref_name = "aragon", color = 0xe8c879 }
aragon.name = _("Aragon")
Nation:register(aragon)
Nation:add_accepted_culture(aragon, aragonese)
-- Formable by both czech and slovakia
czechoslovakia = Nation:create{ ref_name = "czechoslovakia", color = 0x79a899 }
czechoslovakia.name = _("Czechoslovakia")
Nation:register(czechoslovakia)
Nation:add_accepted_culture(czechoslovakia, slovakian)
Nation:add_accepted_culture(czechoslovakia, czech)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Scandinavia
-- Initial nations
denmark = Nation:create{ ref_name = "denmark", color = 0x911624 }
denmark.name = _("Denmark")
Nation:register(denmark)
Nation:add_accepted_culture(denmark, danish)
sweden = Nation:create{ ref_name = "sweden", color = 0x4c87e6 }
sweden.name = _("Sweden")
Nation:register(sweden)
Nation:add_accepted_culture(sweden, swedish)
-- Formable nations
scandinavia = Nation:create{ ref_name = "scandinavia", color = 0xb7d2e8 }
scandinavia.name = _("Scandinavia")
Nation:register(scandinavia)
Nation:add_accepted_culture(scandinavia, finnish)
Nation:add_accepted_culture(scandinavia, norwegian)
Nation:add_accepted_culture(scandinavia, swedish)
Nation:add_accepted_culture(scandinavia, danish)
-- Releasable nations
finland = Nation:create{ ref_name = "finland", color = 0xb1e0e6 }
finland.name = _("Finland")
Nation:register(finland)
Nation:add_accepted_culture(finland, finnish)
norway = Nation:create{ ref_name = "norway", color = 0xe64830 }
norway.name = _("Norway")
Nation:register(norway)
Nation:add_accepted_culture(norway, norwegian)
aland = Nation:create{ ref_name = "aland", color = 0x3d83c4 }
aland.name = _("Åland")
Nation:register(aland)
Nation:add_accepted_culture(aland, finnish)
Nation:add_accepted_culture(aland, swedish)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Poland, Baltics and European Russia
-- Initial nations
russia = Nation:create{ ref_name = "russia", color = 0x79a888 }
russia.name = _("Russia")
Nation:register(russia)
Nation:add_accepted_culture(russia, russian)
krakow = Nation:create{ ref_name = "krakow", color = 0x79a888 }
krakow.name = _("Krakow")
Nation:register(krakow)
Nation:add_accepted_culture(krakow, polish)
-- Releasable nations
-- Poland can be formed by krakow
poland = Nation:create{ ref_name = "poland", color = 0x79a888 }
poland.name = _("Poland")
Nation:register(poland)
Nation:add_accepted_culture(poland, polish)
lithuania = Nation:create{ ref_name = "lithuania", color = 0x79a888 }
lithuania.name = _("Lithuania")
Nation:register(lithuania)
Nation:add_accepted_culture(lithuania, lithuanian)
estonia = Nation:create{ ref_name = "estonia", color = 0x79a888 }
estonia.name = _("Estonia")
Nation:register(estonia)
Nation:add_accepted_culture(estonia, estonian)
latvia = Nation:create{ ref_name = "latvia", color = 0x79a888 }
latvia.name = _("Latvia")
Nation:register(latvia)
Nation:add_accepted_culture(latvia, latvian)
-- Lovonia can be formed by either latvia, estonia or lithuania
livonia = Nation:create{ ref_name = "livonia", color = 0x79a888 }
livonia.name = _("Livonia")
Nation:register(livonia)
Nation:add_accepted_culture(livonia, lithuanian)
Nation:add_accepted_culture(livonia, latvian)
Nation:add_accepted_culture(livonia, estonian)
muscovy = Nation:create{ ref_name = "muscovy", color = 0x79a888 }
muscovy.name = _("Muscovy")
Nation:register(muscovy)
Nation:add_accepted_culture(muscovy, russian)
great_horde = Nation:create{ ref_name = "great_horde", color = 0x79a888 }
great_horde.name = _("Great Horde")
Nation:register(great_horde)
Nation:add_accepted_culture(great_horde, turkish)
novgorod = Nation:create{ ref_name = "novgorod", color = 0x79a888 }
novgorod.name = _("Novgorod")
Nation:register(novgorod)
Nation:add_accepted_culture(novgorod, russian)
Nation:add_accepted_culture(novgorod, finnish)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Holy Roman Empire (German part)
-- Initial nations
prussia = Nation:create{ ref_name = "prussia", color = 0x416473 }
prussia.name = _("Prussia")
Nation:register(prussia)
Nation:add_accepted_culture(prussia, german)
salzburg = Nation:create{ ref_name = "salzburg", color = 0xd9674e }
salzburg.name = _("Salzburg")
Nation:register(salzburg)
Nation:add_accepted_culture(salzburg, german)
saxony = Nation:create{ ref_name = "saxony", color = 0xb7d2e8 }
saxony.name = _("Saxony")
Nation:register(saxony)
Nation:add_accepted_culture(saxony, german)
lorraine = Nation:create{ ref_name = "lorraine", color = 0xb7d2e8 }
lorraine.name = _("Lorraine")
Nation:register(lorraine)
Nation:add_accepted_culture(lorraine, german)
Nation:add_accepted_culture(lorraine, french)
brandenburg = Nation:create{ ref_name = "brandenburg", color = 0xb7d2e8 }
brandenburg.name = _("Brandenburg")
Nation:register(brandenburg)
Nation:add_accepted_culture(brandenburg, german)
frankfurt = Nation:create{ ref_name = "frankfurt", color = 0xb7d2e8 }
frankfurt.name = _("Frankfurt")
Nation:register(frankfurt)
Nation:add_accepted_culture(frankfurt, german)
munich = Nation:create{ ref_name = "munich", color = 0xb7d2e8 }
munich.name = _("Munich")
Nation:register(munich)
Nation:add_accepted_culture(munich, german)
holstein = Nation:create{ ref_name = "holstein", color = 0xb7d2e8 }
holstein.name = _("Holstein")
Nation:register(holstein)
Nation:add_accepted_culture(holstein, german)
Nation:add_accepted_culture(holstein, danish)
bavaria = Nation:create{ ref_name = "bavaria", color = 0xb7d2e8 }
bavaria.name = _("Bavaria")
Nation:register(bavaria)
Nation:add_accepted_culture(bavaria, german)
venice = Nation:create{ ref_name = "venice", color = 0x79a888 }
venice.name = _("Venice")
Nation:register(venice)
Nation:add_accepted_culture(venice, italian)
genoa = Nation:create{ ref_name = "genoa", color = 0x79a888 }
genoa.name = _("Genoa")
Nation:register(genoa)
Nation:add_accepted_culture(genoa, italian)
switerzeland = Nation:create{ ref_name = "switerzeland", color = 0x79a888 }
switerzeland.name = _("Switerzeland")
Nation:register(switerzeland)
Nation:add_accepted_culture(switerzeland, french)
Nation:add_accepted_culture(switerzeland, german)
Nation:add_accepted_culture(switerzeland, italian)
papal_states = Nation:create{ ref_name = "papal_states", color = 0x79a888 }
papal_states.name = _("Papal states")
Nation:register(papal_states)
Nation:add_accepted_culture(papal_states, italian)
hannover = Nation:create{ ref_name = "hannover", color = 0x79a888 }
hannover.name = _("Hannover")
Nation:register(hannover)
Nation:add_accepted_culture(hannover, german)
luxembourg = Nation:create{ ref_name = "luxembourg", color = 0x79a888 }
luxembourg.name = _("Luxembourg")
Nation:register(luxembourg)
Nation:add_accepted_culture(luxembourg, french)
Nation:add_accepted_culture(luxembourg, german)
nassau = Nation:create{ ref_name = "nassau", color = 0x79a888 }
nassau.name = _("Nassau")
Nation:register(nassau)
Nation:add_accepted_culture(nassau, german)
baden = Nation:create{ ref_name = "baden", color = 0x79a888 }
baden.name = _("Baden")
Nation:register(baden)
Nation:add_accepted_culture(baden, german)
anhalt = Nation:create{ ref_name = "anhalt", color = 0x79a888 }
anhalt.name = _("Anhalt")
Nation:register(anhalt)
Nation:add_accepted_culture(anhalt, german)
silesia = Nation:create{ ref_name = "silesia", color = 0x79a888 }
silesia.name = _("Silesia")
Nation:register(silesia)
Nation:add_accepted_culture(silesia, polish)
Nation:add_accepted_culture(silesia, german)
danzig = Nation:create{ ref_name = "danzig", color = 0x79a888 }
danzig.name = _("Danzig")
Nation:register(danzig)
Nation:add_accepted_culture(danzig, german)
wurttemberg = Nation:create{ ref_name = "wurttemberg", color = 0x79a888 }
wurttemberg.name = _("Württemberg")
Nation:register(danzig)
Nation:add_accepted_culture(danzig, german)
piedmont_sardinia = Nation:create{ ref_name = "piedmont_sardinia", color = 0x79a888 }
piedmont_sardinia.name = _("Piedmont Sardinia")
Nation:register(piedmont_sardinia)
Nation:add_accepted_culture(piedmont_sardinia, italian)
two_sicilies = Nation:create{ ref_name = "two_sicilies", color = 0x79a888 }
two_sicilies.name = _("Two Sicilies")
Nation:register(two_sicilies)
Nation:add_accepted_culture(two_sicilies, italian)
tuscany = Nation:create{ ref_name = "tuscany", color = 0x79a888 }
tuscany.name = _("Tuscany")
Nation:register(tuscany)
Nation:add_accepted_culture(tuscany, italian)
austria = Nation:create{ ref_name = "austria", color = 0xd9674e }
austria.name = _("Austria")
Nation:register(austria)
Nation:add_accepted_culture(austria, german)
-- Formable nations
greater_prussia = Nation:create{ ref_name = "greater_prussia", color = 0xe8c879 }
greater_prussia.name = _("Greater Prussia")
Nation:register(greater_prussia)
Nation:add_accepted_culture(greater_prussia, german)
germany = Nation:create{ ref_name = "germany", color = 0xe8c879 }
germany.name = _("Germany")
Nation:register(germany)
Nation:add_accepted_culture(germany, german)
holy_roman_empire = Nation:create{ ref_name = "holy_roman_empire", color = 0xe8c879 }
holy_roman_empire.name = _("Holy Roman Empire")
Nation:register(holy_roman_empire)
Nation:add_accepted_culture(holy_roman_empire, german)
Nation:add_accepted_culture(holy_roman_empire, italian)
italy = Nation:create{ ref_name = "italy", color = 0xe8c879 }
italy.name = _("Italy")
Nation:register(italy)
Nation:add_accepted_culture(italy, italian)
roman_empire = Nation:create{ ref_name = "roman_empire", color = 0xe8c879 }
roman_empire.name = _("Roman Empire")
Nation:register(roman_empire)
Nation:add_accepted_culture(roman_empire, italian)
austria_hungary = Nation:create{ ref_name = "austria_hungary", color = 0xb7d2e8 }
austria_hungary.name = _("Austria-Hungary")
Nation:register(austria_hungary)
Nation:add_accepted_culture(austria_hungary, german)
Nation:add_accepted_culture(austria_hungary, hungarian)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Balkans
turkey = Nation:create{ ref_name = "turkey", color = 0x79a888 }
turkey.name = _("Turkey")
Nation:register(turkey)
Nation:add_accepted_culture(turkey, turkish)
greece = Nation:create{ ref_name = "greece", color = 0x79a888 }
greece.name = _("Greece")
Nation:register(greece)
Nation:add_accepted_culture(greece, greek)
-- Releasable nations
kosovo = Nation:create{ ref_name = "kosovo", color = 0x79a888 }
kosovo.name = _("Kosovo")
Nation:register(kosovo)
bosnia_and_herzegovina = Nation:create{ ref_name = "bosnia_and_herzegovina", color = 0x79a888 }
bosnia_and_herzegovina.name = _("Bosnia and Herzegovina")
Nation:register(bosnia_and_herzegovina)
-----------------------------------------------------------------------