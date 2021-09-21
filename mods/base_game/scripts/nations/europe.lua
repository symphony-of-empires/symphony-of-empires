-----------------------------------------------------------------------
-- Europe (excluding balkans, scandinavia and the mess that Germany was)
-- Initial nations
united_kingdom = Nation:new{ ref_name = "united_kingdom" }
united_kingdom.name = _("United Kingdom")
Nation:register(united_kingdom)
Nation:set_ideology(united_kingdom, monarchy)
Nation:add_accepted_culture(united_kingdom, british)
Nation:add_accepted_culture(united_kingdom, irish)
Nation:add_client_hint(united_kingdom, monarchy, _("United Kingdom"), 0xc94c40)

netherlands = Nation:new{ ref_name = "netherlands" }
netherlands.name = _("Netherlands")
Nation:register(netherlands)
Nation:set_ideology(netherlands, monarchy)
Nation:add_accepted_culture(netherlands, dutch)
Nation:add_client_hint(netherlands, monarchy, _("Netherlands"), 0xf5931b)

france = Nation:new{ ref_name = "france" }
france.name = _("France")
Nation:register(france)
Nation:set_ideology(france, monarchy)
Nation:add_accepted_culture(france, french)
Nation:add_client_hint(france, monarchy, _("France"), 0x1b3cb3)

spain = Nation:new{ ref_name = "spain" }
spain.name = _("Spain")
Nation:register(spain)
Nation:set_ideology(spain, monarchy)
Nation:add_accepted_culture(spain, spanish)
Nation:add_client_hint(spain, monarchy, _("Spain"), 0xdee03a)

portugal = Nation:new{ ref_name = "portugal" }
portugal.name = _("Portugal")
Nation:register(portugal)
Nation:set_ideology(portugal, monarchy)
Nation:add_accepted_culture(portugal, portuguese)
Nation:add_client_hint(portugal, monarchy, _("Portugal"), 0x0f691f)
Nation:add_client_hint(portugal, communism, _("People's Republic of Portugal"), 0x0f691f)

-- Formable nations
european_union = Nation:new{ ref_name = "european_union" }
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
Nation:add_client_hint(european_union, democracy, _("European Union"), 0x060666)
Nation:add_client_hint(european_union, communism, _("European Commune"), 0x060666)

iberia = Nation:new{ ref_name = "iberia" }
iberia.name = _("Iberia")
Nation:register(iberia)
Nation:add_accepted_culture(iberia, catalan)
Nation:add_accepted_culture(iberia, basque)
Nation:add_accepted_culture(iberia, galician)
Nation:add_accepted_culture(iberia, aragonese)
Nation:add_accepted_culture(iberia, portuguese)
Nation:add_accepted_culture(iberia, spanish)
Nation:add_client_hint(iberia, communism, _("Iberia"), 0x808000)

-- Releasable nations
brittany = Nation:new{ ref_name = "brittany" }
brittany.name = _("Brittany")
Nation:register(brittany)
Nation:add_client_hint(brittany, communism, _("Brittany"), 0x808000)

normandy = Nation:new{ ref_name = "normandy" }
normandy.name = _("Normandy")
Nation:register(normandy)
Nation:add_client_hint(normandy, communism, _("Normandy"), 0x808000)

occitania = Nation:new{ ref_name = "occitania"}
occitania.name = _("Occitania")
Nation:register(occitania)
Nation:add_client_hint(occitania, communism, _("Occitania"), 0x808000)

catalonia = Nation:new{ ref_name = "catalonia" }
catalonia.name = _("Catalonia")
Nation:register(catalonia)
Nation:add_client_hint(catalonia, communism, _("Catalonia"), 0x808000)

Nation:add_accepted_culture(catalonia, catalan)
basque_country = Nation:new{ ref_name = "basque" }
basque_country.name = _("Basque")
Nation:register(basque_country)
Nation:add_client_hint(basque_country, communism, _("Basque"), 0x808000)

Nation:add_accepted_culture(basque_country, basque)
galicia = Nation:new{ ref_name = "galicia" }
galicia.name = _("Galicia")
Nation:register(galicia)
Nation:add_client_hint(galicia, communism, _("Galicia"), 0x808000)

Nation:add_accepted_culture(galicia, galician)
aragon = Nation:new{ ref_name = "aragon" }
aragon.name = _("Aragon")
Nation:register(aragon)
Nation:add_accepted_culture(aragon, aragonese)
Nation:add_client_hint(aragon, communism, _("Aragon"), 0x808000)

-- Formable by both czech and slovakia
czechoslovakia = Nation:new{ ref_name = "czechoslovakia" }
czechoslovakia.name = _("Czechoslovakia")
Nation:register(czechoslovakia)
Nation:add_accepted_culture(czechoslovakia, slovakian)
Nation:add_accepted_culture(czechoslovakia, czech)
Nation:add_client_hint(czechoslovakia, communism, _("Czechoslovakia"), 0x808000)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Scandinavia
-- Initial nations
denmark = Nation:new{ ref_name = "denmark" }
denmark.name = _("Denmark")
Nation:register(denmark)
Nation:set_ideology(denmark, monarchy)
Nation:add_accepted_culture(denmark, danish)
Nation:add_client_hint(denmark, monarchy, _("Denmark"), 0xc27b6d)

sweden = Nation:new{ ref_name = "sweden" }
sweden.name = _("Sweden")
Nation:register(sweden)
Nation:set_ideology(sweden, monarchy)
Nation:add_accepted_culture(sweden, swedish)
Nation:add_client_hint(sweden, monarchy, _("Sweden"), 0x3a739e)

-- Formable nations
scandinavia = Nation:new{ ref_name = "scandinavia" }
scandinavia.name = _("Scandinavia")
Nation:register(scandinavia)
Nation:set_ideology(scandinavia, monarchy)
Nation:add_accepted_culture(scandinavia, finnish)
Nation:add_accepted_culture(scandinavia, norwegian)
Nation:add_accepted_culture(scandinavia, swedish)
Nation:add_accepted_culture(scandinavia, danish)
Nation:add_client_hint(scandinavia, communism, _("Scandinavia"), 0x808000)

-- Releasable nations
finland = Nation:new{ ref_name = "finland" }
finland.name = _("Finland")
Nation:register(finland)
Nation:add_accepted_culture(finland, finnish)
Nation:add_client_hint(finland, communism, _("Finland"), 0x808000)

norway = Nation:new{ ref_name = "norway" }
norway.name = _("Norway")
Nation:register(norway)
Nation:add_accepted_culture(norway, norwegian)
Nation:add_client_hint(norway, communism, _("Norway"), 0x808000)

aland = Nation:new{ ref_name = "aland" }
aland.name = _("Åland")
Nation:register(aland)
Nation:add_accepted_culture(aland, finnish)
Nation:add_accepted_culture(aland, swedish)
Nation:add_client_hint(aland, communism, _("Åland"), 0x808000)

iceland = Nation:new{ ref_name = "iceland" }
iceland.name = _("Iceland")
Nation:register(iceland)
Nation:add_accepted_culture(iceland, icelandic)
Nation:add_client_hint(iceland, communism, _("Iceland"), 0x808000)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Poland, Baltics and European Russia
-- Initial nations
russia = Nation:new{ ref_name = "russia" }
russia.name = _("Russia")
Nation:register(russia)
Nation:set_ideology(russia, monarchy)
Nation:add_accepted_culture(russia, russian)
Nation:add_client_hint(russia, monarchy, _("Russian Empire"), 0x318c23)
Nation:add_client_hint(russia, communism, _("Russian SFSR"), 0xbf0000)

krakow = Nation:new{ ref_name = "krakow" }
krakow.name = _("Krakow")
Nation:register(krakow)
Nation:set_ideology(krakow, monarchy)
Nation:add_accepted_culture(krakow, polish)
Nation:add_client_hint(krakow, communism, _("Krakow"), 0xe6f4f7)
-- Releasable nations
-- Poland can be formed by krakow
poland = Nation:new{ ref_name = "poland" }
poland.name = _("Poland")
Nation:register(poland)
Nation:add_accepted_culture(poland, polish)
Nation:add_client_hint(poland, communism, _("Poland"), 0xbf85d6)

lithuania = Nation:new{ ref_name = "lithuania" }
lithuania.name = _("Lithuania")
Nation:register(lithuania)
Nation:add_accepted_culture(lithuania, lithuanian)
Nation:add_client_hint(lithuania, communism, _("Lithuania"), 0x598c42)

estonia = Nation:new{ ref_name = "estonia" }
estonia.name = _("Estonia")
Nation:register(estonia)
Nation:add_accepted_culture(estonia, estonian)
Nation:add_client_hint(estonia, communism, _("Estonia"), 0x4c8ed4)

latvia = Nation:new{ ref_name = "latvia" }
latvia.name = _("Latvia")
Nation:register(latvia)
Nation:add_accepted_culture(latvia, latvian)
Nation:add_client_hint(latvia, communism, _("Latvia"), 0x962820)

-- Lovonia can be formed by either latvia, estonia or lithuania
livonia = Nation:new{ ref_name = "livonia" }
livonia.name = _("Livonia")
Nation:register(livonia)
Nation:add_accepted_culture(livonia, lithuanian)
Nation:add_accepted_culture(livonia, latvian)
Nation:add_accepted_culture(livonia, estonian)
Nation:add_client_hint(livonia, communism, _("Livonia"), 0x808000)

muscovy = Nation:new{ ref_name = "muscovy" }
muscovy.name = _("Muscovy")
Nation:register(muscovy)
Nation:add_accepted_culture(muscovy, russian)
Nation:add_client_hint(muscovy, communism, _("Muscovy"), 0x808000)

great_horde = Nation:new{ ref_name = "great_horde" }
great_horde.name = _("Great Horde")
Nation:register(great_horde)
Nation:add_accepted_culture(great_horde, turkish)
Nation:add_client_hint(great_horde, communism, _("Great Horde"), 0x808000)

novgorod = Nation:new{ ref_name = "novgorod" }
novgorod.name = _("Novgorod")
Nation:register(novgorod)
Nation:add_accepted_culture(novgorod, russian)
Nation:add_accepted_culture(novgorod, finnish)
Nation:add_client_hint(novgorod, communism, _("Novgorod"), 0x808000)

nogai_horde = Nation:new{ ref_name = "nogai_horde" }
nogai_horde.name = _("Nogai Horde")
Nation:register(nogai_horde)
Nation:add_accepted_culture(nogai_horde, nogai)
Nation:add_client_hint(nogai_horde, communism, _("Nogai Horde"), 0x808000)

uzbekistan = Nation:new{ ref_name = "uzbekistan" }
uzbekistan.name = _("Uzbekistan")
Nation:register(uzbekistan)
Nation:add_accepted_culture(uzbekistan, uzbek)
Nation:add_client_hint(uzbekistan, communism, _("Uzbekistan"), 0x808000)

kazakhistan = Nation:new{ ref_name = "kazakhistan" }
kazakhistan.name = _("Kazakhistan")
Nation:register(kazakhistan)
Nation:add_accepted_culture(kazakhistan, kazakh)
Nation:add_client_hint(kazakhistan, monarchy, _("Kazakhistan"), 0x2276b3)

kyrgyztan = Nation:new{ ref_name = "kyrgyztan" }
kyrgyztan.name = _("Kyrgyztan")
Nation:register(kyrgyztan)
Nation:add_accepted_culture(kyrgyztan, kyrgyz)
Nation:add_client_hint(kyrgyztan, communism, _("Kyrgyztan"), 0x808000)

afghanistan = Nation:new{ ref_name = "afghanistan" }
afghanistan.name = _("Afghanistan")
Nation:register(afghanistan)
Nation:add_accepted_culture(afghanistan, afghan)
Nation:add_client_hint(afghanistan, communism, _("Afghanistan"), 0x808000)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Holy Roman Empire (German part)
-- Initial nations
prussia = Nation:new{ ref_name = "prussia" }
prussia.name = _("Prussia")
Nation:register(prussia)
Nation:set_ideology(prussia, democracy)
Nation:add_accepted_culture(prussia, german)
Nation:add_client_hint(prussia, monarchy, _("Prussia"), 0x405370)

salzburg = Nation:new{ ref_name = "salzburg" }
salzburg.name = _("Salzburg")
Nation:register(salzburg)
Nation:set_ideology(salzburg, democracy)
Nation:add_accepted_culture(salzburg, german)
Nation:add_client_hint(salzburg, communism, _("Salzburg"), 0x808000)

saxony = Nation:new{ ref_name = "saxony" }
saxony.name = _("Saxony")
Nation:register(saxony)
Nation:set_ideology(saxony, democracy)
Nation:add_accepted_culture(saxony, german)
Nation:add_client_hint(saxony, communism, _("Saxony"), 0x808000)

lorraine = Nation:new{ ref_name = "lorraine" }
lorraine.name = _("Lorraine")
Nation:register(lorraine)
Nation:set_ideology(lorraine, democracy)
Nation:add_accepted_culture(lorraine, german)
Nation:add_accepted_culture(lorraine, french)
Nation:add_client_hint(lorraine, communism, _("Lorraine"), 0x808000)

brandenburg = Nation:new{ ref_name = "brandenburg" }
brandenburg.name = _("Brandenburg")
Nation:register(brandenburg)
Nation:set_ideology(brandenburg, democracy)
Nation:add_accepted_culture(brandenburg, german)
Nation:add_client_hint(brandenburg, communism, _("Brandenburg"), 0x808000)

frankfurt = Nation:new{ ref_name = "frankfurt" }
frankfurt.name = _("Frankfurt")
Nation:register(frankfurt)
Nation:set_ideology(frankfurt, democracy)
Nation:add_accepted_culture(frankfurt, german)
Nation:add_client_hint(frankfurt, communism, _("Frankfurt"), 0x808000)

munich = Nation:new{ ref_name = "munich" }
munich.name = _("Munich")
Nation:register(munich)
Nation:set_ideology(munich, democracy)
Nation:add_accepted_culture(munich, german)
Nation:add_client_hint(munich, communism, _("Munich"), 0x808000)

holstein = Nation:new{ ref_name = "holstein" }
holstein.name = _("Holstein")
Nation:register(holstein)
Nation:set_ideology(holstein, democracy)
Nation:add_accepted_culture(holstein, german)
Nation:add_accepted_culture(holstein, danish)
Nation:add_client_hint(holstein, communism, _("Holstein"), 0x808000)

bavaria = Nation:new{ ref_name = "bavaria" }
bavaria.name = _("Bavaria")
Nation:register(bavaria)
Nation:set_ideology(bavaria, democracy)
Nation:add_accepted_culture(bavaria, german)
Nation:add_client_hint(bavaria, communism, _("Bavaria"), 0x808000)

venice = Nation:new{ ref_name = "venice" }
venice.name = _("Venice")
Nation:register(venice)
Nation:set_ideology(venice, democracy)
Nation:add_accepted_culture(venice, italian)
Nation:add_client_hint(venice, communism, _("Venice"), 0x808000)

genoa = Nation:new{ ref_name = "genoa" }
genoa.name = _("Genoa")
Nation:register(genoa)
Nation:set_ideology(genoa, democracy)
Nation:add_accepted_culture(genoa, italian)
Nation:add_client_hint(genoa, communism, _("Genoa"), 0x808000)

switerzeland = Nation:new{ ref_name = "switerzeland" }
switerzeland.name = _("Switerzeland")
Nation:register(switerzeland)
Nation:set_ideology(switerzeland, democracy)
Nation:add_accepted_culture(switerzeland, french)
Nation:add_accepted_culture(switerzeland, german)
Nation:add_accepted_culture(switerzeland, italian)
Nation:add_client_hint(switerzeland, monarchy, _("Switerzeland"), 0xb32609)

papal_states = Nation:new{ ref_name = "papal_states" }
papal_states.name = _("Papal states")
Nation:register(papal_states)
Nation:set_ideology(papal_states, democracy)
Nation:add_accepted_culture(papal_states, italian)
Nation:add_client_hint(papal_states, monarchy, _("Papal states"), 0xf7f6cd)

hannover = Nation:new{ ref_name = "hannover" }
hannover.name = _("Hannover")
Nation:register(hannover)
Nation:set_ideology(hannover, democracy)
Nation:add_accepted_culture(hannover, german)
Nation:add_client_hint(hannover, communism, _("Hannover"), 0x808000)

luxembourg = Nation:new{ ref_name = "luxembourg" }
luxembourg.name = _("Luxembourg")
Nation:register(luxembourg)
Nation:set_ideology(luxembourg, democracy)
Nation:add_accepted_culture(luxembourg, french)
Nation:add_accepted_culture(luxembourg, german)
Nation:add_client_hint(luxembourg, monarchy, _("Luxembourg"), 0x45d1ca)

nassau = Nation:new{ ref_name = "nassau" }
nassau.name = _("Nassau")
Nation:register(nassau)
Nation:set_ideology(nassau, democracy)
Nation:add_accepted_culture(nassau, german)
Nation:add_client_hint(nassau, communism, _("Nassau"), 0x808000)

baden = Nation:new{ ref_name = "baden" }
baden.name = _("Baden")
Nation:register(baden)
Nation:set_ideology(baden, democracy)
Nation:add_accepted_culture(baden, german)
Nation:add_client_hint(baden, communism, _("Baden"), 0x808000)

anhalt = Nation:new{ ref_name = "anhalt" }
anhalt.name = _("Anhalt")
Nation:register(anhalt)
Nation:set_ideology(anhalt, democracy)
Nation:add_accepted_culture(anhalt, german)
Nation:add_client_hint(anhalt, communism, _("Anhalt"), 0x808000)

silesia = Nation:new{ ref_name = "silesia" }
silesia.name = _("Silesia")
Nation:register(silesia)
Nation:set_ideology(silesia, democracy)
Nation:add_accepted_culture(silesia, polish)
Nation:add_accepted_culture(silesia, german)
Nation:add_client_hint(silesia, communism, _("Silesia"), 0x808000)

danzig = Nation:new{ ref_name = "danzig" }
danzig.name = _("Danzig")
Nation:register(danzig)
Nation:set_ideology(danzig, democracy)
Nation:add_accepted_culture(danzig, german)
Nation:add_client_hint(danzig, communism, _("Danzig"), 0x808000)

wurttemberg = Nation:new{ ref_name = "wurttemberg" }
wurttemberg.name = _("Württemberg")
Nation:register(wurttemberg)
Nation:set_ideology(wurttemberg, democracy)
Nation:add_accepted_culture(wurttemberg, german)
Nation:add_client_hint(wurttemberg, communism, _("Württemberg"), 0x808000)

piedmont_sardinia = Nation:new{ ref_name = "piedmont_sardinia" }
piedmont_sardinia.name = _("Piedmont-Sardinia")
Nation:register(piedmont_sardinia)
Nation:set_ideology(piedmont_sardinia, democracy)
Nation:add_accepted_culture(piedmont_sardinia, italian)
Nation:add_client_hint(piedmont_sardinia, communism, _("Piedmont-Sardinia"), 0x808000)

two_sicilies = Nation:new{ ref_name = "two_sicilies" }
two_sicilies.name = _("Two Sicilies")
Nation:register(two_sicilies)
Nation:set_ideology(two_sicilies, democracy)
Nation:add_accepted_culture(two_sicilies, italian)
Nation:add_client_hint(two_sicilies, communism, _("Two Sicilies"), 0x808000)

tuscany = Nation:new{ ref_name = "tuscany" }
tuscany.name = _("Tuscany")
Nation:register(tuscany)
Nation:set_ideology(tuscany, democracy)
Nation:add_accepted_culture(tuscany, italian)
Nation:add_client_hint(tuscany, communism, _("Tuscany"), 0x808000)

austria = Nation:new{ ref_name = "austria" }
austria.name = _("Austria")
Nation:register(austria)
Nation:set_ideology(austria, democracy)
Nation:add_accepted_culture(austria, german)
Nation:add_client_hint(austria, monarchy, _("Austria"), 0xe8ebe8)

-- Formable nations
greater_prussia = Nation:new{ ref_name = "greater_prussia" }
greater_prussia.name = _("Greater Prussia")
Nation:register(greater_prussia)
Nation:add_accepted_culture(greater_prussia, german)
Nation:add_client_hint(greater_prussia, communism, _("Greater Prussia"), 0x808000)

germany = Nation:new{ ref_name = "germany" }
germany.name = _("Germany")
Nation:register(germany)
Nation:add_accepted_culture(germany, german)
Nation:add_client_hint(germany, communism, _("Germany"), 0x6e6e6e)

holy_roman_empire = Nation:new{ ref_name = "holy_roman_empire" }
holy_roman_empire.name = _("Holy Roman Empire")
Nation:register(holy_roman_empire)
Nation:add_accepted_culture(holy_roman_empire, german)
Nation:add_accepted_culture(holy_roman_empire, italian)
Nation:add_client_hint(holy_roman_empire, monarchy, _("Holy Roman Empire"), 0xf0e930)

italy = Nation:new{ ref_name = "italy" }
italy.name = _("Italy")
Nation:register(italy)
Nation:add_accepted_culture(italy, italian)
Nation:add_client_hint(italy, monarchy, _("Italy"), 0x5fbf5c)

roman_empire = Nation:new{ ref_name = "roman_empire" }
roman_empire.name = _("Roman Empire")
Nation:register(roman_empire)
Nation:add_accepted_culture(roman_empire, italian)
Nation:add_client_hint(roman_empire, communism, _("Roman Empire"), 0x808000)

austria_hungary = Nation:new{ ref_name = "austria_hungary" }
austria_hungary.name = _("Austria-Hungary")
Nation:register(austria_hungary)
Nation:add_accepted_culture(austria_hungary, german)
Nation:add_accepted_culture(austria_hungary, hungarian)
Nation:add_client_hint(austria_hungary, monarchy, _("Austria-Hungary"), 0xe8ebe8)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Balkans
turkey = Nation:new{ ref_name = "turkey" }
turkey.name = _("Turkey")
Nation:register(turkey)
Nation:set_ideology(turkey, monarchy)
Nation:add_accepted_culture(turkey, turkish)
Nation:add_client_hint(chile, monarchy, _("Ottoman Empire"), 0xc9201a)
Nation:add_client_hint(chile, republic, _("Turkey"), 0xc9201a)

greece = Nation:new{ ref_name = "greece" }
greece.name = _("Greece")
Nation:register(greece)
Nation:set_ideology(greece, monarchy)
Nation:add_accepted_culture(greece, greek)
Nation:add_client_hint(chile, monarchy, _("Greece"), 0xaac4e6)

-- Releasable nations
kosovo = Nation:new{ ref_name = "kosovo" }
kosovo.name = _("Kosovo")
Nation:register(kosovo)
Nation:add_client_hint(kosovo, communism, _("Kosovo"), 0x808000)

bosnia_and_herzegovina = Nation:new{ ref_name = "bosnia_and_herzegovina" }
bosnia_and_herzegovina.name = _("Bosnia and Herzegovina")
Nation:register(bosnia_and_herzegovina)
Nation:add_client_hint(bosnia_and_herzegovina, communism, _("Bosnia and Herzegovina"), 0x808000)
-----------------------------------------------------------------------
