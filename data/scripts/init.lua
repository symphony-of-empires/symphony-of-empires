require("api")

-- Culture
local japanesse = Culture:create{ ref_name = "japanesse" }
japanesse.name = _("Japanesse")
Culture:register(japanesse)
local russian = Culture:create{ ref_name = "russian" }
russian.name = _("Russian")
Culture:register(russian)

-- Religion
local good_bois = Religion:create{ ref_name = "good_bois" }
good_bois.name = _("Good bois")
Religion:register(good_bois)
local bad_bois = Religion:create{ ref_name = "bad_bois" }
bad_bois.name = _("Bad bois")
Religion:register(bad_bois)

-- Pop types
function entrepreneur_on_tick()
	--print("hello entrepreneur!")
end
local entrepreneur = PopType:create{ ref_name = "entrepreneur", on_tick_fn = "entrepreneur_on_tick" }
entrepreneur.name = _("Entrepreneur")
PopType:register(entrepreneur)

function artisan_on_tick()
	--print("hello artisan!")
end
local artisan = PopType:create{ ref_name = "artisan", on_tick_fn = "artisan_on_tick" }
artisan.name = _("Artisan")
PopType:register(artisan)

function craftsmen_on_tick()
	--print("hello craftsmen!")
end
local craftsmen = PopType:create{ ref_name = "craftsmen", on_tick_fn = "craftsmen_on_tick" }
craftsmen.name = _("Craftsmen")
PopType:register(craftsmen)

function bureaucrat_on_tick()
	--print("hello bureaucrat!")
end
local bureaucrat = PopType:create{ ref_name = "bureaucrat", on_tick_fn = "bureaucrat_on_tick" }
bureaucrat.name = _("Bureaucrat")
PopType:register(bureaucrat)

function aristocrat_on_tick()
	--print("hello aristocrat!")
end
local aristocrat = PopType:create{ ref_name = "aristocrat", on_tick_fn = "aristocrat_on_tick" }
aristocrat.name = _("Aristocrat")
PopType:register(aristocrat)

function clergymen_on_tick()
	--print("hello clergymen!")
end
local clergymen = PopType:create{ ref_name = "clergymen", on_tick_fn = "clergymen_on_tick" }
clergymen.name = _("Clergymen")
PopType:register(clergymen)

function farmer_on_tick()
	--print("hello farmer!")
end
local farmer = PopType:create{ ref_name = "farmer", on_tick_fn = "farmer_on_tick" }
farmer.name = _("Farmer")
PopType:register(farmer)

function soldier_on_tick()
	--print("hello soldier!")
end
local soldier = PopType:create{ ref_name = "soldier", on_tick_fn = "soldier_on_tick" }
soldier.name = _("Soldier")
PopType:register(soldier)

function laborer_on_tick()
	--print("hello laborer!")
end
local laborer = PopType:create{ ref_name = "laborer", on_tick_fn = "laborer_on_tick" }
laborer.name = _("Laborer")
PopType:register(laborer)

-- Good types

-- Farmable stuff
local wheat = Good:create{ ref_name = "wheat" }
wheat.name = _("Wheat")
Good:register(wheat)
local grapes = Good:create{ ref_name = "grapes" }
grapes.name = _("Grapes")
Good:register(grapes)
local rice = Good:create{ ref_name = "rice" }
rice.name = _("Rice")
Good:register(rice)
local cotton = Good:create{ ref_name = "cotton" }
cotton.name = _("Cotton")
Good:register(cotton)
local coffe_bean = Good:create{ ref_name = "coffe_bean" }
coffe_bean.name = _("Coffe bean")
Good:register(coffe_bean)
local salt_grain = Good:create{ ref_name = "salt_grain" }
salt_grain.name = _("Salt grain")
Good:register(salt_grain)
local silk = Good:create{ ref_name = "silk" }
silk.name = _("Silk")
Good:register(silk)

-- Industrial materials
local fabric = Good:create{ ref_name = "fabric" }
fabric.name = _("Fabric")
Good:register(fabric)
local dye = Good:create{ ref_name = "dye" }
dye.name = _("Dye")
Good:register(dye)
local textile = Good:create{ ref_name = "textile" }
textile.name = _("Textile")
Good:register(textile)
local plastic = Good:create{ ref_name = "plastic" }
plastic.name = _("Plastic")
Good:register(plastic)

-- Minerals
local copper = Good:create{ ref_name = "copper" }
copper.name = _("Copper")
Good:register(copper)
local silica = Good:create{ ref_name = "silica" }
silica.name = _("Silica")
Good:register(silica)
local uranium = Good:create{ ref_name = "uranium" }
uranium.name = _("Uranium")
Good:register(uranium)
local gold = Good:create{ ref_name = "gold" }
gold.name = _("Gold")
Good:register(gold)
local iron = Good:create{ ref_name = "iron" }
iron.name = _("Iron")
Good:register(iron)
local diamond = Good:create{ ref_name = "diamond" }
diamond.name = _("Diamond")
Good:register(diamond)
local titanium = Good:create{ ref_name = "titanium" }
titanium.name = _("Titanium")
Good:register(titanium)
local oil = Good:create{ ref_name = "oil" }
oil.name = _("Oil")
Good:register(oil)
local timber = Good:create{ ref_name = "timber" }
timber.name = _("Timber")
Good:register(timber)
local coal = Good:create{ ref_name = "coal" }
coal.name = _("Coal")
Good:register(coal)

-- Food
local glass = Good:create{ ref_name = "glass" }
glass.name = _("Glass")
Good:register(glass)
local bread = Good:create{ ref_name = "bread" }
bread.name = _("Bread")
Good:register(bread)
local salt = Good:create{ ref_name = "salt" }
salt.name = _("Salt")
Good:register(salt)

-- Commodities
local window = Good:create{ ref_name = "window" }
window.name = _("Window")
Good:register(window)
local wine = Good:create{ ref_name = "wine" }
wine.name = _("Wine")
Good:register(wine)
local gasoline = Good:create{ ref_name = "gasoline" }
gasoline.name = _("Gasoline")
Good:register(gasoline)
local chair = Good:create{ ref_name = "chair" }
chair.name = _("Chair")
Good:register(chair)
local bed = Good:create{ ref_name = "bed" }
bed.name = _("Bed")
Good:register(bed)
print("good types")

-- Industry types

-- Primary sector industries (the most essential)
local sand_smelter = IndustryType:create{ ref_name = "sand_smelter" }
sand_smelter.name = _("Sand smelter")
IndustryType:register(sand_smelter)
-- glass factories can create glass from thin air
IndustryType:add_output(sand_smelter, glass)

local oil_refinery = IndustryType:create{ ref_name = "oil_refinery" }
oil_refinery.name = _("Oil refinery")
IndustryType:register(oil_refinery)
-- thin air oil, i think the air needs some democracy!
IndustryType:add_output(oil_refinery, oil)

local lumberjack = IndustryType:create{ ref_name = "lumberjack" }
lumberjack.name = _("Lumberjack")
IndustryType:register(lumberjack)
-- same logic here
IndustryType:add_output(lumberjack, timber)

local vineyard = IndustryType:create{ ref_name = "vineyard" }
vineyard.name = _("Vineyard")
IndustryType:register(vineyard)
IndustryType:add_output(vineyard, grapes)

local wheat_farm = IndustryType:create{ ref_name = "wheat_farm" }
wheat_farm.name = _("Wheat farm")
IndustryType:register(wheat_farm)
IndustryType:add_output(wheat_farm, wheat)

local rice_farm = IndustryType:create{ ref_name = "rice_farm" }
rice_farm.name = _("Rice farm")
IndustryType:register(rice_farm)
IndustryType:add_output(rice_farm, wheat)

local cotton_farm = IndustryType:create{ ref_name = "cotton_farm" }
cotton_farm.name = _("Cotton farm")
IndustryType:register(cotton_farm)
IndustryType:add_output(cotton_farm, wheat)

-- Mines can create stuff from thin air, but don't worry because this is
-- not abuse-ble
local diamond_mine = IndustryType:create{ ref_name = "diamond_mine" }
diamond_mine.name = _("Diamond mine")
IndustryType:register(diamond_mine)
IndustryType:add_output(diamond_mine, diamond)

local iron_mine = IndustryType:create{ ref_name = "iron_mine" }
iron_mine.name = _("Iron mine")
IndustryType:register(iron_mine)
IndustryType:add_output(iron_mine, iron)

local gold_mine = IndustryType:create{ ref_name = "gold_mine" }
gold_mine.name = _("Gold mine")
IndustryType:register(gold_mine)
IndustryType:add_output(gold_mine, gold)

local titanium_mine = IndustryType:create{ ref_name = "titanium_mine" }
titanium_mine.name = _("Titanium mine")
IndustryType:register(titanium_mine)
IndustryType:add_output(titanium_mine, titanium)
print("primary sector industries")

-- Secondary sector industries (now the fun begins)
local chair_manufacturer = IndustryType:create{ ref_name = "chair_manufacturer" }
chair_manufacturer.name = _("Chair manufacturer")
IndustryType:register(chair_manufacturer)
IndustryType:add_input(chair_manufacturer, timber)
IndustryType:add_output(chair_manufacturer, chair)

local bed_manufacturer = IndustryType:create{ ref_name = "bed_manufacturer" }
bed_manufacturer.name = _("Bed manufacturer")
IndustryType:register(bed_manufacturer)
IndustryType:add_input(bed_manufacturer, timber)
IndustryType:add_output(bed_manufacturer, bed)
print("secondary sector industries")

-- Nations
local japan = Nation:create{ ref_name = "japan", color = 0xb0313f, default_flag = "flags/japan.png" }
japan.name = _("Japan")
Nation:register(japan)
local russia = Nation:create{ ref_name = "russia", color = 0xffcd01, default_flag = "flags/russia.png" }
russia.name = _("Russia")
Nation:register(russia)
print("nations")

-- Provinces
-- japan island
local hokkaido = Province:create{ ref_name = "hokkaido", color = 0x2870a8 }
hokkaido.name = _("Hokkaido")
Province:register(hokkaido)

local south_hokkaido = Province:create{ ref_name = "south_hokkaido", color = 0x6e28a8 }
south_hokkaido.name = _("South Hokkaido")
Province:register(south_hokkaido)

local south_east_hokkaido = Province:create{ ref_name = "south_east_hokkaido", color = 0x449893 }
south_east_hokkaido.name = _("South East Hokkaido")
Province:register(south_east_hokkaido)

local kagoshima = Province:create{ ref_name = "kagoshima", color = 0xd4f431 }
kagoshima.name = _("Kagoshima")
Province:register(kagoshima)

local nagasaki = Province:create{ ref_name = "nagasaki", color = 0x3133f4 }
nagasaki.name = _("Nagasaki")
Province:register(nagasaki)

local miyazaki = Province:create{ ref_name = "miyazaki", color = 0x984469 }
miyazaki.name = _("Miyazaki")
Province:register(miyazaki)

local yamaguchi = Province:create{ ref_name = "yamaguchi", color = 0x984497 }
yamaguchi.name = _("Yamaguchi")
Province:register(yamaguchi)

local tokushima = Province:create{ ref_name = "tokushima", color = 0xcd3b8d }
tokushima.name = _("Tokushima")
Province:register(tokushima)

local hiroshima = Province:create{ ref_name = "hiroshima", color = 0x449891 }
hiroshima.name = _("Hiroshima")
Province:register(hiroshima)

local okayama = Province:create{ ref_name = "okayama", color = 0x44986f }
okayama.name = _("Okayama")
Province:register(okayama)

local hyogo = Province:create{ ref_name = "hyogo", color = 0x729844 }
hyogo.name = _("Hyogo")
Province:register(hyogo)

local osaka = Province:create{ ref_name = "osaka", color = 0x987f44 }
osaka.name = _("Osaka")
Province:register(osaka)

local mie = Province:create{ ref_name = "mie", color = 0x709332 }
mie.name = _("Mie")
Province:register(mie)

local kyoto = Province:create{ ref_name = "kyoto", color = 0x4b6d12 }
kyoto.name = _("Kyoto")
Province:register(kyoto)

local gifu = Province:create{ ref_name = "gifu", color = 0x10164d }
gifu.name = _("Gifu")
Province:register(gifu)

local shizuoka = Province:create{ ref_name = "shizuoka", color = 0x90a06e }
shizuoka.name = _("Shizuoka")
Province:register(shizuoka)

local niigata = Province:create{ ref_name = "niigata", color = 0xa828a0 }
niigata.name = _("Niigata")
Province:register(niigata)

local yamanashi = Province:create{ ref_name = "yamanashi", color = 0x78933b }
yamanashi.name = _("Yamanashi")
Province:register(yamanashi)

local tokyo = Province:create{ ref_name = "tokyo", color = 0x306d11 }
tokyo.name = _("Tokyo")
Province:register(tokyo)

local ibaraki = Province:create{ ref_name = "ibaraki", color = 0x8d179b }
ibaraki.name = _("Ibaraki")
Province:register(ibaraki)

local fukushima = Province:create{ ref_name = "fukushima", color = 0x5e6180 }
fukushima.name = _("Fukushima")
Province:register(fukushima)

local tochigi = Province:create{ ref_name = "tochigi", color = 0x494498 }
tochigi.name = _("Tochigi")
Province:register(tochigi)

local iwate = Province:create{ ref_name = "iwate", color = 0x525d61 }
iwate.name = _("Iwate")
Province:register(iwate)

local akita = Province:create{ ref_name = "akita", color = 0x447698 }
akita.name = _("Akita")
Province:register(akita)

local aomori = Province:create{ ref_name = "aomori", color = 0x116d14 }
aomori.name = _("Aomori")
Province:register(aomori)

local hokkaido_tohoku = Province:create{ ref_name = "hokkaido_tohoku", color = 0x449893 }
hokkaido_tohoku.name = "Hokaido-Tohoku"
Province:register(hokkaido_tohoku)

local nagano = Province:create{ ref_name = "nagano", color = 0x6d6111 }
nagano.name = _("Nagano")
Province:register(nagano)

local nagoya = Province:create{ ref_name = "nagoya", color = 0x938732 }
nagoya.name = _("Nagoya")
Province:register(nagoya)

local small_south_japan_islands = Province:create{ ref_name = "small_south_japan_islands", color = 0x31f455 }
small_south_japan_islands.name = _("Small south Japan islands")
Province:register(small_south_japan_islands)

-- islands north east of japan
local iturup = Province:create{ ref_name = "iturup", color = 0xf4ae31 }
iturup.name = "Iturup"
Province:register(iturup)

local raycoke = Province:create{ ref_name = "raycoke", color = 0x62f431 }
raycoke.name = "Raycoke"
Province:register(raycoke)

local paramushir = Province:create{ ref_name = "paramushir", color = 0xa131f4 }
paramushir.name = "Paramushir"
Province:register(paramushir)

-- east russia
local kamchatka = Province:create{ ref_name = "kamchatka", color = 0x66b8a5 }
kamchatka.name = "Kamchatka"
Province:register(kamchatka)

local south_kamchatka = Province:create{ ref_name = "south_kamchatka", color = 0x104d3f }
south_kamchatka.name = "South kamchatka"
Province:register(south_kamchatka)

local amur = Province:create{ ref_name = "amur", color = 0x18268d }
amur.name = "Amur"
Province:register(amur)

local khabarovsk = Province:create{ ref_name = "khabarovsk", color = 0x182ec9 }
khabarovsk.name = "Khabarovks"
Province:register(khabarovsk)

local south_khabarovsk = Province:create{ ref_name = "south_khabarovsk", color = 0xb066b8 }
south_khabarovsk.name = "South khabarovsk"
Province:register(south_khabarovsk)

local primorsky_krai = Province:create{ ref_name = "primorsky_krai", color = 0x1e5431 }
primorsky_krai.name = "Primorsky krai"
Province:register(primorsky_krai)

-- sakhlin island
local north_sakhlin = Province:create{ ref_name = "north_sakhlin", color = 0x1f0d67 }
north_sakhlin.name = "North sakhlin"
Province:register(north_sakhlin)

local central_sakhlin = Province:create{ ref_name = "central_sakhlin", color = 0x9421a2 }
central_sakhlin.name = "Central sakhlin"
Province:register(central_sakhlin)

local south_sakhlin = Province:create{ ref_name = "south_sakhlin", color = 0xf4316a }
south_sakhlin.name = "South sakhlin"
Province:register(south_sakhlin)

-- korean peninsula
local east_north_korea = Province:create{ ref_name = "east_north_korea", color = 0x1e4954 }
east_north_korea.name = "East north korea"
Province:register(east_north_korea)

local west_north_korea = Province:create{ ref_name = "west_north_korea", color = 0x544d1e }
west_north_korea.name = "West north korea"
Province:register(west_north_korea)

local east_south_korea = Province:create{ ref_name = "east_south_korea", color = 0xca2ec4 }
east_south_korea.name = "East south korea"
Province:register(east_south_korea)

local west_south_korea = Province:create{ ref_name = "west_south_korea", color = 0x043440 }
west_south_korea.name = "West south korea"
Province:register(west_south_korea)

local southern_korea = Province:create{ ref_name = "southern_korea", color = 0x31f4dd }
southern_korea.name = "Southern korea"
Province:register(southern_korea)
print("provinces")

Province:add_pop(tokyo, artisan, japanesse, good_bois, 5000)
Province:add_pop(tokyo, farmer, japanesse, good_bois, 5000)
Province:add_pop(tokyo, soldier, japanesse, good_bois, 5000)
Province:add_pop(tokyo, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(tokyo, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(tokyo, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(tokyo, clergymen, japanesse, good_bois, 5000)
Province:add_pop(tokyo, laborer, japanesse, good_bois, 5000)
Province:add_pop(tokyo, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(ibaraki, artisan, japanesse, good_bois, 5000)
Province:add_pop(ibaraki, farmer, japanesse, good_bois, 5000)
Province:add_pop(ibaraki, soldier, japanesse, good_bois, 5000)
Province:add_pop(ibaraki, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(ibaraki, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(ibaraki, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(ibaraki, clergymen, japanesse, good_bois, 5000)
Province:add_pop(ibaraki, laborer, japanesse, good_bois, 5000)
Province:add_pop(ibaraki, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(fukushima, artisan, japanesse, good_bois, 5000)
Province:add_pop(fukushima, farmer, japanesse, good_bois, 5000)
Province:add_pop(fukushima, soldier, japanesse, good_bois, 5000)
Province:add_pop(fukushima, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(fukushima, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(fukushima, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(fukushima, clergymen, japanesse, good_bois, 5000)
Province:add_pop(fukushima, laborer, japanesse, good_bois, 5000)
Province:add_pop(fukushima, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(tochigi, artisan, japanesse, good_bois, 5000)
Province:add_pop(tochigi, farmer, japanesse, good_bois, 5000)
Province:add_pop(tochigi, soldier, japanesse, good_bois, 5000)
Province:add_pop(tochigi, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(tochigi, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(tochigi, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(tochigi, clergymen, japanesse, good_bois, 5000)
Province:add_pop(tochigi, laborer, japanesse, good_bois, 5000)
Province:add_pop(tochigi, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(iwate, artisan, japanesse, good_bois, 5000)
Province:add_pop(iwate, farmer, japanesse, good_bois, 5000)
Province:add_pop(iwate, soldier, japanesse, good_bois, 5000)
Province:add_pop(iwate, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(iwate, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(iwate, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(iwate, clergymen, japanesse, good_bois, 5000)
Province:add_pop(iwate, laborer, japanesse, good_bois, 5000)
Province:add_pop(iwate, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(akita, artisan, japanesse, good_bois, 5000)
Province:add_pop(akita, farmer, japanesse, good_bois, 5000)
Province:add_pop(akita, soldier, japanesse, good_bois, 5000)
Province:add_pop(akita, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(akita, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(akita, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(akita, clergymen, japanesse, good_bois, 5000)
Province:add_pop(akita, laborer, japanesse, good_bois, 5000)
Province:add_pop(akita, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(aomori, artisan, japanesse, good_bois, 5000)
Province:add_pop(aomori, farmer, japanesse, good_bois, 5000)
Province:add_pop(aomori, soldier, japanesse, good_bois, 5000)
Province:add_pop(aomori, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(aomori, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(aomori, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(aomori, clergymen, japanesse, good_bois, 5000)
Province:add_pop(aomori, laborer, japanesse, good_bois, 5000)
Province:add_pop(aomori, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(hokkaido_tohoku, artisan, japanesse, good_bois, 5000)
Province:add_pop(hokkaido_tohoku, farmer, japanesse, good_bois, 5000)
Province:add_pop(hokkaido_tohoku, soldier, japanesse, good_bois, 5000)
Province:add_pop(hokkaido_tohoku, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(hokkaido_tohoku, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(hokkaido_tohoku, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(hokkaido_tohoku, clergymen, japanesse, good_bois, 5000)
Province:add_pop(hokkaido_tohoku, laborer, japanesse, good_bois, 5000)
Province:add_pop(hokkaido_tohoku, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(nagano, artisan, japanesse, good_bois, 5000)
Province:add_pop(nagano, farmer, japanesse, good_bois, 5000)
Province:add_pop(nagano, soldier, japanesse, good_bois, 5000)
Province:add_pop(nagano, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(nagano, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(nagano, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(nagano, clergymen, japanesse, good_bois, 5000)
Province:add_pop(nagano, laborer, japanesse, good_bois, 5000)
Province:add_pop(nagano, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(nagoya, artisan, japanesse, good_bois, 5000)
Province:add_pop(nagoya, farmer, japanesse, good_bois, 5000)
Province:add_pop(nagoya, soldier, japanesse, good_bois, 5000)
Province:add_pop(nagoya, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(nagoya, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(nagoya, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(nagoya, clergymen, japanesse, good_bois, 5000)
Province:add_pop(nagoya, laborer, japanesse, good_bois, 5000)
Province:add_pop(nagoya, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(raycoke, artisan, japanesse, good_bois, 5000)
Province:add_pop(raycoke, farmer, japanesse, good_bois, 5000)
Province:add_pop(raycoke, soldier, japanesse, good_bois, 5000)
Province:add_pop(raycoke, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(raycoke, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(raycoke, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(raycoke, clergymen, japanesse, good_bois, 5000)
Province:add_pop(raycoke, laborer, japanesse, good_bois, 5000)
Province:add_pop(raycoke, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(paramushir, artisan, japanesse, good_bois, 5000)
Province:add_pop(paramushir, farmer, japanesse, good_bois, 5000)
Province:add_pop(paramushir, soldier, japanesse, good_bois, 5000)
Province:add_pop(paramushir, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(paramushir, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(paramushir, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(paramushir, clergymen, japanesse, good_bois, 5000)
Province:add_pop(paramushir, laborer, japanesse, good_bois, 5000)
Province:add_pop(paramushir, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(kamchatka, artisan, japanesse, good_bois, 5000)
Province:add_pop(kamchatka, farmer, japanesse, good_bois, 5000)
Province:add_pop(kamchatka, soldier, japanesse, good_bois, 5000)
Province:add_pop(kamchatka, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(kamchatka, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(kamchatka, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(kamchatka, clergymen, japanesse, good_bois, 5000)
Province:add_pop(kamchatka, laborer, japanesse, good_bois, 5000)
Province:add_pop(kamchatka, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(south_kamchatka, artisan, japanesse, good_bois, 5000)
Province:add_pop(south_kamchatka, farmer, japanesse, good_bois, 5000)
Province:add_pop(south_kamchatka, soldier, japanesse, good_bois, 5000)
Province:add_pop(south_kamchatka, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(south_kamchatka, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(south_kamchatka, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(south_kamchatka, clergymen, japanesse, good_bois, 5000)
Province:add_pop(south_kamchatka, laborer, japanesse, good_bois, 5000)
Province:add_pop(south_kamchatka, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(small_south_japan_islands, artisan, japanesse, good_bois, 5000)
Province:add_pop(small_south_japan_islands, farmer, japanesse, good_bois, 5000)
Province:add_pop(small_south_japan_islands, soldier, japanesse, good_bois, 5000)
Province:add_pop(small_south_japan_islands, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(small_south_japan_islands, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(small_south_japan_islands, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(small_south_japan_islands, clergymen, japanesse, good_bois, 5000)
Province:add_pop(small_south_japan_islands, laborer, japanesse, good_bois, 5000)
Province:add_pop(small_south_japan_islands, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(gifu, artisan, japanesse, good_bois, 5000)
Province:add_pop(gifu, farmer, japanesse, good_bois, 5000)
Province:add_pop(gifu, soldier, japanesse, good_bois, 5000)
Province:add_pop(gifu, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(gifu, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(gifu, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(gifu, clergymen, japanesse, good_bois, 5000)
Province:add_pop(gifu, laborer, japanesse, good_bois, 5000)
Province:add_pop(gifu, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(shizuoka, artisan, japanesse, good_bois, 5000)
Province:add_pop(shizuoka, farmer, japanesse, good_bois, 5000)
Province:add_pop(shizuoka, soldier, japanesse, good_bois, 5000)
Province:add_pop(shizuoka, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(shizuoka, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(shizuoka, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(shizuoka, clergymen, japanesse, good_bois, 5000)
Province:add_pop(shizuoka, laborer, japanesse, good_bois, 5000)
Province:add_pop(shizuoka, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(niigata, artisan, japanesse, good_bois, 5000)
Province:add_pop(niigata, farmer, japanesse, good_bois, 5000)
Province:add_pop(niigata, soldier, japanesse, good_bois, 5000)
Province:add_pop(niigata, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(niigata, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(niigata, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(niigata, clergymen, japanesse, good_bois, 5000)
Province:add_pop(niigata, laborer, japanesse, good_bois, 5000)
Province:add_pop(niigata, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(yamanashi, artisan, japanesse, good_bois, 5000)
Province:add_pop(yamanashi, farmer, japanesse, good_bois, 5000)
Province:add_pop(yamanashi, soldier, japanesse, good_bois, 5000)
Province:add_pop(yamanashi, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(yamanashi, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(yamanashi, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(yamanashi, clergymen, japanesse, good_bois, 5000)
Province:add_pop(yamanashi, laborer, japanesse, good_bois, 5000)
Province:add_pop(yamanashi, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(iturup, artisan, japanesse, good_bois, 5000)
Province:add_pop(iturup, farmer, japanesse, good_bois, 5000)
Province:add_pop(iturup, soldier, japanesse, good_bois, 5000)
Province:add_pop(iturup, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(iturup, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(iturup, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(iturup, clergymen, japanesse, good_bois, 5000)
Province:add_pop(iturup, laborer, japanesse, good_bois, 5000)
Province:add_pop(iturup, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(central_sakhlin, artisan, japanesse, good_bois, 5000)
Province:add_pop(central_sakhlin, farmer, japanesse, good_bois, 5000)
Province:add_pop(central_sakhlin, soldier, japanesse, good_bois, 5000)
Province:add_pop(central_sakhlin, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(central_sakhlin, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(central_sakhlin, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(central_sakhlin, clergymen, japanesse, good_bois, 5000)
Province:add_pop(central_sakhlin, laborer, japanesse, good_bois, 5000)
Province:add_pop(central_sakhlin, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(south_sakhlin, artisan, japanesse, good_bois, 5000)
Province:add_pop(south_sakhlin, farmer, japanesse, good_bois, 5000)
Province:add_pop(south_sakhlin, soldier, japanesse, good_bois, 5000)
Province:add_pop(south_sakhlin, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(south_sakhlin, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(south_sakhlin, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(south_sakhlin, clergymen, japanesse, good_bois, 5000)
Province:add_pop(south_sakhlin, laborer, japanesse, good_bois, 5000)
Province:add_pop(south_sakhlin, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(amur, artisan, japanesse, good_bois, 5000)
Province:add_pop(amur, farmer, japanesse, good_bois, 5000)
Province:add_pop(amur, soldier, japanesse, good_bois, 5000)
Province:add_pop(amur, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(amur, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(amur, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(amur, clergymen, japanesse, good_bois, 5000)
Province:add_pop(amur, laborer, japanesse, good_bois, 5000)
Province:add_pop(amur, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(khabarovsk, artisan, japanesse, good_bois, 5000)
Province:add_pop(khabarovsk, farmer, japanesse, good_bois, 5000)
Province:add_pop(khabarovsk, soldier, japanesse, good_bois, 5000)
Province:add_pop(khabarovsk, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(khabarovsk, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(khabarovsk, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(khabarovsk, clergymen, japanesse, good_bois, 5000)
Province:add_pop(khabarovsk, laborer, japanesse, good_bois, 5000)
Province:add_pop(khabarovsk, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(south_khabarovsk, artisan, japanesse, good_bois, 5000)
Province:add_pop(south_khabarovsk, farmer, japanesse, good_bois, 5000)
Province:add_pop(south_khabarovsk, soldier, japanesse, good_bois, 5000)
Province:add_pop(south_khabarovsk, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(south_khabarovsk, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(south_khabarovsk, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(south_khabarovsk, clergymen, japanesse, good_bois, 5000)
Province:add_pop(south_khabarovsk, laborer, japanesse, good_bois, 5000)
Province:add_pop(south_khabarovsk, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(primorsky_krai, artisan, japanesse, good_bois, 5000)
Province:add_pop(primorsky_krai, farmer, japanesse, good_bois, 5000)
Province:add_pop(primorsky_krai, soldier, japanesse, good_bois, 5000)
Province:add_pop(primorsky_krai, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(primorsky_krai, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(primorsky_krai, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(primorsky_krai, clergymen, japanesse, good_bois, 5000)
Province:add_pop(primorsky_krai, laborer, japanesse, good_bois, 5000)
Province:add_pop(primorsky_krai, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(north_sakhlin, artisan, japanesse, good_bois, 5000)
Province:add_pop(north_sakhlin, farmer, japanesse, good_bois, 5000)
Province:add_pop(north_sakhlin, soldier, japanesse, good_bois, 5000)
Province:add_pop(north_sakhlin, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(north_sakhlin, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(north_sakhlin, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(north_sakhlin, clergymen, japanesse, good_bois, 5000)
Province:add_pop(north_sakhlin, laborer, japanesse, good_bois, 5000)
Province:add_pop(north_sakhlin, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(east_north_korea, artisan, japanesse, good_bois, 5000)
Province:add_pop(east_north_korea, farmer, japanesse, good_bois, 5000)
Province:add_pop(east_north_korea, soldier, japanesse, good_bois, 5000)
Province:add_pop(east_north_korea, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(east_north_korea, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(east_north_korea, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(east_north_korea, clergymen, japanesse, good_bois, 5000)
Province:add_pop(east_north_korea, laborer, japanesse, good_bois, 5000)
Province:add_pop(east_north_korea, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(hyogo, artisan, japanesse, good_bois, 5000)
Province:add_pop(hyogo, farmer, japanesse, good_bois, 5000)
Province:add_pop(hyogo, soldier, japanesse, good_bois, 5000)
Province:add_pop(hyogo, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(hyogo, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(hyogo, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(hyogo, clergymen, japanesse, good_bois, 5000)
Province:add_pop(hyogo, laborer, japanesse, good_bois, 5000)
Province:add_pop(hyogo, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(osaka, artisan, japanesse, good_bois, 5000)
Province:add_pop(osaka, farmer, japanesse, good_bois, 5000)
Province:add_pop(osaka, soldier, japanesse, good_bois, 5000)
Province:add_pop(osaka, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(osaka, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(osaka, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(osaka, clergymen, japanesse, good_bois, 5000)
Province:add_pop(osaka, laborer, japanesse, good_bois, 5000)
Province:add_pop(osaka, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(mie, artisan, japanesse, good_bois, 5000)
Province:add_pop(mie, farmer, japanesse, good_bois, 5000)
Province:add_pop(mie, soldier, japanesse, good_bois, 5000)
Province:add_pop(mie, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(mie, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(mie, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(mie, clergymen, japanesse, good_bois, 5000)
Province:add_pop(mie, laborer, japanesse, good_bois, 5000)
Province:add_pop(mie, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(kyoto, artisan, japanesse, good_bois, 5000)
Province:add_pop(kyoto, farmer, japanesse, good_bois, 5000)
Province:add_pop(kyoto, soldier, japanesse, good_bois, 5000)
Province:add_pop(kyoto, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(kyoto, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(kyoto, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(kyoto, clergymen, japanesse, good_bois, 5000)
Province:add_pop(kyoto, laborer, japanesse, good_bois, 5000)
Province:add_pop(kyoto, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(west_north_korea, artisan, japanesse, good_bois, 5000)
Province:add_pop(west_north_korea, farmer, japanesse, good_bois, 5000)
Province:add_pop(west_north_korea, soldier, japanesse, good_bois, 5000)
Province:add_pop(west_north_korea, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(west_north_korea, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(west_north_korea, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(west_north_korea, clergymen, japanesse, good_bois, 5000)
Province:add_pop(west_north_korea, laborer, japanesse, good_bois, 5000)
Province:add_pop(west_north_korea, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(east_south_korea, artisan, japanesse, good_bois, 5000)
Province:add_pop(east_south_korea, farmer, japanesse, good_bois, 5000)
Province:add_pop(east_south_korea, soldier, japanesse, good_bois, 5000)
Province:add_pop(east_south_korea, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(east_south_korea, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(east_south_korea, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(east_south_korea, clergymen, japanesse, good_bois, 5000)
Province:add_pop(east_south_korea, laborer, japanesse, good_bois, 5000)
Province:add_pop(east_south_korea, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(west_south_korea, artisan, japanesse, good_bois, 5000)
Province:add_pop(west_south_korea, farmer, japanesse, good_bois, 5000)
Province:add_pop(west_south_korea, soldier, japanesse, good_bois, 5000)
Province:add_pop(west_south_korea, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(west_south_korea, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(west_south_korea, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(west_south_korea, clergymen, japanesse, good_bois, 5000)
Province:add_pop(west_south_korea, laborer, japanesse, good_bois, 5000)
Province:add_pop(west_south_korea, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(southern_korea, artisan, japanesse, good_bois, 5000)
Province:add_pop(southern_korea, farmer, japanesse, good_bois, 5000)
Province:add_pop(southern_korea, soldier, japanesse, good_bois, 5000)
Province:add_pop(southern_korea, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(southern_korea, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(southern_korea, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(southern_korea, clergymen, japanesse, good_bois, 5000)
Province:add_pop(southern_korea, laborer, japanesse, good_bois, 5000)
Province:add_pop(southern_korea, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(hokkaido, artisan, japanesse, good_bois, 5000)
Province:add_pop(hokkaido, farmer, japanesse, good_bois, 5000)
Province:add_pop(hokkaido, soldier, japanesse, good_bois, 5000)
Province:add_pop(hokkaido, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(hokkaido, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(hokkaido, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(hokkaido, clergymen, japanesse, good_bois, 5000)
Province:add_pop(hokkaido, laborer, japanesse, good_bois, 5000)
Province:add_pop(hokkaido, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(south_hokkaido, artisan, japanesse, good_bois, 5000)
Province:add_pop(south_hokkaido, farmer, japanesse, good_bois, 5000)
Province:add_pop(south_hokkaido, soldier, japanesse, good_bois, 5000)
Province:add_pop(south_hokkaido, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(south_hokkaido, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(south_hokkaido, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(south_hokkaido, clergymen, japanesse, good_bois, 5000)
Province:add_pop(south_hokkaido, laborer, japanesse, good_bois, 5000)
Province:add_pop(south_hokkaido, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(south_east_hokkaido, artisan, japanesse, good_bois, 5000)
Province:add_pop(south_east_hokkaido, farmer, japanesse, good_bois, 5000)
Province:add_pop(south_east_hokkaido, soldier, japanesse, good_bois, 5000)
Province:add_pop(south_east_hokkaido, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(south_east_hokkaido, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(south_east_hokkaido, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(south_east_hokkaido, clergymen, japanesse, good_bois, 5000)
Province:add_pop(south_east_hokkaido, laborer, japanesse, good_bois, 5000)
Province:add_pop(south_east_hokkaido, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(kagoshima, artisan, japanesse, good_bois, 5000)
Province:add_pop(kagoshima, farmer, japanesse, good_bois, 5000)
Province:add_pop(kagoshima, soldier, japanesse, good_bois, 5000)
Province:add_pop(kagoshima, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(kagoshima, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(kagoshima, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(kagoshima, clergymen, japanesse, good_bois, 5000)
Province:add_pop(kagoshima, laborer, japanesse, good_bois, 5000)
Province:add_pop(kagoshima, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(nagasaki, artisan, japanesse, good_bois, 5000)
Province:add_pop(nagasaki, farmer, japanesse, good_bois, 5000)
Province:add_pop(nagasaki, soldier, japanesse, good_bois, 5000)
Province:add_pop(nagasaki, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(nagasaki, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(nagasaki, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(nagasaki, clergymen, japanesse, good_bois, 5000)
Province:add_pop(nagasaki, laborer, japanesse, good_bois, 5000)
Province:add_pop(nagasaki, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(miyazaki, artisan, japanesse, good_bois, 5000)
Province:add_pop(miyazaki, farmer, japanesse, good_bois, 5000)
Province:add_pop(miyazaki, soldier, japanesse, good_bois, 5000)
Province:add_pop(miyazaki, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(miyazaki, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(miyazaki, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(miyazaki, clergymen, japanesse, good_bois, 5000)
Province:add_pop(miyazaki, laborer, japanesse, good_bois, 5000)
Province:add_pop(miyazaki, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(yamaguchi, artisan, japanesse, good_bois, 5000)
Province:add_pop(yamaguchi, farmer, japanesse, good_bois, 5000)
Province:add_pop(yamaguchi, soldier, japanesse, good_bois, 5000)
Province:add_pop(yamaguchi, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(yamaguchi, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(yamaguchi, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(yamaguchi, clergymen, japanesse, good_bois, 5000)
Province:add_pop(yamaguchi, laborer, japanesse, good_bois, 5000)
Province:add_pop(yamaguchi, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(tokushima, artisan, japanesse, good_bois, 5000)
Province:add_pop(tokushima, farmer, japanesse, good_bois, 5000)
Province:add_pop(tokushima, soldier, japanesse, good_bois, 5000)
Province:add_pop(tokushima, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(tokushima, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(tokushima, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(tokushima, clergymen, japanesse, good_bois, 5000)
Province:add_pop(tokushima, laborer, japanesse, good_bois, 5000)
Province:add_pop(tokushima, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(hiroshima, artisan, japanesse, good_bois, 5000)
Province:add_pop(hiroshima, farmer, japanesse, good_bois, 5000)
Province:add_pop(hiroshima, soldier, japanesse, good_bois, 5000)
Province:add_pop(hiroshima, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(hiroshima, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(hiroshima, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(hiroshima, clergymen, japanesse, good_bois, 5000)
Province:add_pop(hiroshima, laborer, japanesse, good_bois, 5000)
Province:add_pop(hiroshima, entrepreneur, japanesse, good_bois, 500)
Province:add_pop(okayama, artisan, japanesse, good_bois, 5000)
Province:add_pop(okayama, farmer, japanesse, good_bois, 5000)
Province:add_pop(okayama, soldier, japanesse, good_bois, 5000)
Province:add_pop(okayama, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(okayama, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(okayama, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(okayama, clergymen, japanesse, good_bois, 5000)
Province:add_pop(okayama, laborer, japanesse, good_bois, 5000)
Province:add_pop(okayama, entrepreneur, japanesse, good_bois, 500)

-- THIS IS NOT PART OF THE FINAL MODDING API

-- Companies
local generic_transport_company = Company:create{ name = "Generic Transport Corporate", money = 10000000, is_transport = true }
Company:register(generic_transport_company)
add_op_province_to_company(0, south_east_hokkaido.ref_name)
add_op_province_to_company(0, kagoshima.ref_name)
add_op_province_to_company(0, nagasaki.ref_name)
add_op_province_to_company(0, miyazaki.ref_name)
add_op_province_to_company(0, yamaguchi.ref_name)
add_op_province_to_company(0, tokushima.ref_name)
add_op_province_to_company(0, hiroshima.ref_name)
add_op_province_to_company(0, hokkaido.ref_name)
add_op_province_to_company(0, south_hokkaido.ref_name)
add_op_province_to_company(0, okayama.ref_name)
add_op_province_to_company(0, hyogo.ref_name)
add_op_province_to_company(0, osaka.ref_name)
add_op_province_to_company(0, mie.ref_name)
add_op_province_to_company(0, kyoto.ref_name)
add_op_province_to_company(0, gifu.ref_name)
add_op_province_to_company(0, shizuoka.ref_name)
add_op_province_to_company(0, niigata.ref_name)
add_op_province_to_company(0, yamanashi.ref_name)
add_op_province_to_company(0, tokyo.ref_name)
add_op_province_to_company(0, ibaraki.ref_name)
add_op_province_to_company(0, fukushima.ref_name)
add_op_province_to_company(0, tochigi.ref_name)
add_op_province_to_company(0, iwate.ref_name)
add_op_province_to_company(0, akita.ref_name)
add_op_province_to_company(0, aomori.ref_name)

local rival_transport_company = Company:create{ name = "Rival Transport Corporate", money = 10000000, is_transport = true }
Company:register(rival_transport_company)
add_op_province_to_company(1, hokkaido_tohoku.ref_name)
add_op_province_to_company(1, nagano.ref_name)
add_op_province_to_company(1, nagoya.ref_name)
add_op_province_to_company(1, small_south_japan_islands.ref_name)
add_op_province_to_company(1, iturup.ref_name)
add_op_province_to_company(1, raycoke.ref_name)
add_op_province_to_company(1, paramushir.ref_name)
add_op_province_to_company(1, kamchatka.ref_name)
add_op_province_to_company(1, south_kamchatka.ref_name)
add_op_province_to_company(1, amur.ref_name)
add_op_province_to_company(1, khabarovsk.ref_name)
add_op_province_to_company(1, south_khabarovsk.ref_name)
add_op_province_to_company(1, east_north_korea.ref_name)
add_op_province_to_company(1, west_north_korea.ref_name)
add_op_province_to_company(1, east_south_korea.ref_name)
add_op_province_to_company(1, southern_korea.ref_name)
add_op_province_to_company(1, west_south_korea.ref_name)
add_op_province_to_company(1, south_sakhlin.ref_name)
add_op_province_to_company(1, central_sakhlin.ref_name)
add_op_province_to_company(1, north_sakhlin.ref_name)
add_op_province_to_company(1, primorsky_krai.ref_name)

print("companies")

print("loaded mod.lua")