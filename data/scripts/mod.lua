-- Good types
local wheat = Good:create{ ref_name = "wheat" }
wheat.name = _("Wheat")
Good:register(wheat)
local grapes = Good:create{ ref_name = "grapes" }
grapes.name = _("Grapes")
Good:register(grapes)
local copper = Good:create{ ref_name = "copper" }
copper.name = _("Copper")
Good:register(copper)
local silica = Good:create{ ref_name = "silica" }
silica.name = _("Silica")
Good:register(silica)
local uranium = Good:create{ ref_name = "uranium" }
uranium.name = _("Uranium")
Good:register(uranium)
local coffe_bean = Good:create{ ref_name = "coffe_bean" }
coffe_bean.name = _("Coffe bean")
Good:register(coffe_bean)
local salt_grain = Good:create{ ref_name = "salt_grain" }
salt_grain.name = _("Salt grain")
Good:register(salt_grain)
local glass = Good:create{ ref_name = "glass" }
glass.name = _("Glass")
Good:register(glass)
local bread = Good:create{ ref_name = "bread" }
bread.name = _("Bread")
Good:register(bread)
local salt = Good:create{ ref_name = "salt" }
salt.name = _("Salt")
Good:register(salt)
local window = Good:create{ ref_name = "window" }
window.name = _("Window")
Good:register(window)
local wine = Good:create{ ref_name = "wine" }
wine.name = _("Wine")
Good:register(wine)
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
local gasoline = Good:create{ ref_name = "gasoline" }
gasoline.name = _("Gasoline")
Good:register(gasoline)
local wood = Good:create{ ref_name = "wood" }
wood.name = _("Wood")
Good:register(wood)
local chair = Good:create{ ref_name = "chair" }
chair.name = _("Chair")
Good:register(chair)
local bed = Good:create{ ref_name = "bed" }
bed.name = _("Bed")
Good:register(bed)
local table = Good:create{ ref_name = "table" }
table.name = _("Table")
Good:register(table)
local gold_chain = Good:create{ ref_name = "gold_chain" }
gold_chain.name = _("Gold chain")
Good:register(gold_chain)
local silver_chain = Good:create{ ref_name = "silver_chain" }
silver_chain.name = _("Silver chain")
Good:register(silver_chain)

-- Industry types

-- Primary sector industries (the most essential)
local oil_refinery = IndustryType:create{ ref_name = "oil_refinery" }
oil_refinery.name = _("Oil refinery")
IndustryType:register(oil_refinery)
-- thin air oil, i think the air needs some democracy!
IndustryType:add_output(oil_refinery, oil)

local sand_smelter = IndustryType:create{ ref_name = "sand_smelter" }
sand_smelter.name = _("Sand smelter")
IndustryType:register(sand_smelter)
-- glass factories can create glass from thin air
IndustryType:add_output(sand_smelter, glass)

local lumberjack = IndustryType:create{ ref_name = "lumberjack" }
lumberjack.name = _("Lumberjack")
IndustryType:register(lumberjack)
-- same logic here
IndustryType:add_output(lumberjack, wood)

local vineyard = IndustryType:create{ ref_name = "vineyard" }
vineyard.name = _("Vineyard")
IndustryType:register(vineyard)
IndustryType:add_output(vineyard, grapes)

local wheat_farm = IndustryType:create{ ref_name = "wheat_farm" }
wheat_farm.name = _("Wheat farm")
IndustryType:register(wheat_farm)
IndustryType:add_output(wheat_farm, wheat)

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

-- Secondary sector industries (now the fun begins)
local gold_chain_factory = IndustryType:create{ ref_name = "gold_chain_factory" }
gold_chain_factory.name = _("Gold chain factory")
IndustryType:register(gold_chain_factory)
IndustryType:add_input(gold_chain_factory, gold)
IndustryType:add_output(gold_chain_factory, gold_chain)

local chair_manufacturer = IndustryType:create{ ref_name = "chair_manufacturer" }
chair_manufacturer.name = _("Chair manufacturer")
IndustryType:register(chair_manufacturer)
IndustryType:add_input(chair_manufacturer, wood)
IndustryType:add_output(chair_manufacturer, chair)

local bed_manufacturer = IndustryType:create{ ref_name = "bed_manufacturer" }
bed_manufacturer.name = _("Bed manufacturer")
IndustryType:register(bed_manufacturer)
IndustryType:add_input(bed_manufacturer, wood)
IndustryType:add_output(bed_manufacturer, bed)

-- Nations
local japan = Nation:create{ ref_name = "japan", color = 0xb0313f, default_flag = "flags/japan.png" }
japan.name = _("Japan")
Nation:register(japan)
local russia = Nation:create{ ref_name = "russia", color = 0xffcd01, default_flag = "flags/russia.png" }
russia.name = _("Russia")
Nation:register(russia)

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

-- Companies
local generic_transport_company = Company:create{ name = "Generic Air and Gunboat Transport Corporate", money = 10000000, is_transport = true }
Company:register(generic_transport_company)
local generic_retailer_company = Company:create{ name = "Generic Supermarket Corporate", money = 10000000, is_retailer = true }
Company:register(generic_retailer_company)
local generic_industrial_company = Company:create{ name = "Generic Industrial Thingy", money = 10000000, is_industry = true }
Company:register(generic_industrial_company)

print("loaded mod.lua")
