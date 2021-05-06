-- Good types
local wheat = Good:create{ ref_name = "wheat" }
Good:register(wheat)
local grapes = Good:create{ ref_name = "grapes" }
Good:register(grapes)
local coffe_bean = Good:create{ ref_name = "coffe_bean" }
Good:register(coffe_bean)
local salt_grain = Good:create{ ref_name = "salt_grain" }
Good:register(salt_grain)
local glass = Good:create{ ref_name = "glass" }
Good:register(glass)
local bread = Good:create{ ref_name = "bread" }
Good:register(bread)
local salt = Good:create{ ref_name = "salt" }
Good:register(salt)
local window = Good:create{ ref_name = "window" }
Good:register(window)
local wine = Good:create{ ref_name = "wine" }
Good:register(wine)
local gold = Good:create{ ref_name = "gold" }
Good:register(gold)
local iron = Good:create{ ref_name = "iron" }
Good:register(iron)
local diamond = Good:create{ ref_name = "diamond" }
Good:register(diamond)
local titanium = Good:create{ ref_name = "titanium" }
Good:register(titanium)
local wood = Good:create{ ref_name = "wood" }
Good:register(wood)
local chair = Good:create{ ref_name = "chair" }
Good:register(chair)
local bed = Good:create{ ref_name = "bed" }
Good:register(bed)
local table = Good:create{ ref_name = "table" }
Good:register(table)
local gold_chain = Good:create{ ref_name = "gold_chain" }
Good:register(gold_chain)

-- Industry types

-- Primary sector industries (the most essential)
local sand_smelter = IndustryType:create{ ref_name = "sand_smelter" }
IndustryType:register(sand_smelter)
-- glass factories can create glass from thin air
IndustryType:add_output(sand_smelter, glass)

local lumberjack = IndustryType:create{ ref_name = "lumberjack" }
IndustryType:register(lumberjack)
-- same logic here
IndustryType:add_output(lumberjack, wood)

-- Mines can create stuff from thin air, but don't worry because this is
-- not abuse-ble
local diamond_mine = IndustryType:create{ ref_name = "diamond_mine" }
IndustryType:register(diamond_mine)
IndustryType:add_output(diamond_mine, diamond)

local iron_mine = IndustryType:create{ ref_name = "iron_mine" }
IndustryType:register(iron_mine)
IndustryType:add_output(iron_mine, iron)

local gold_mine = IndustryType:create{ ref_name = "gold_mine" }
IndustryType:register(gold_mine)
IndustryType:add_output(gold_mine, gold)

local titanium_mine = IndustryType:create{ ref_name = "titanium_mine" }
IndustryType:register(titanium_mine)
IndustryType:add_output(titanium_mine, titanium)

-- Secondary sector industries (now the fun begins)
local gold_chain_factory = IndustryType:create{ ref_name = "gold_chain_factory" }
IndustryType:register(gold_chain_factory)
IndustryType:add_input(gold_chain_factory, gold)
IndustryType:add_output(gold_chain_factory, gold_chain)

local chair_manufacturer = IndustryType:create{ ref_name = "chair_manufacturer" }
IndustryType:register(chair_manufacturer)
IndustryType:add_input(chair_manufacturer, wood)
IndustryType:add_output(chair_manufacturer, chair)

local bed_manufacturer = IndustryType:create{ ref_name = "bed_manufacturer" }
IndustryType:register(bed_manufacturer)
IndustryType:add_input(bed_manufacturer, wood)
IndustryType:add_output(bed_manufacturer, bed)

-- Nations
local japan = Nation:create{ ref_name = "japan", color = 0xb0313f, default_flag = "data/flags/japan.png" }
Nation:register(japan)
local russia = Nation:create{ ref_name = "russia", color = 0xffcd01, default_flag = "data/flags/russia.png" }
Nation:register(russia)

-- Provinces
-- japan island
local hokkaido = Province:create{ ref_name = "hokkaido", color = 0x2870a8 }
Province:register(hokkaido)
local south_hokkaido = Province:create{ ref_name = "south_hokkaido", color = 0x6e28a8 }
Province:register(south_hokkaido)
local south_east_hokkaido = Province:create{ ref_name = "south_east_hokkaido", color = 0x449893 }
Province:register(south_east_hokkaido)
local kagoshima = Province:create{ ref_name = "kagoshima", color = 0xd4f431 }
Province:register(kagoshima)
local nagasaki = Province:create{ ref_name = "nagasaki", color = 0x3133f4 }
Province:register(nagasaki)
local miyazaki = Province:create{ ref_name = "miyazaki", color = 0x984469 }
Province:register(miyazaki)
local yamaguchi = Province:create{ ref_name = "yamaguchi", color = 0x984497 }
Province:register(yamaguchi)
local tokushima = Province:create{ ref_name = "tokushima", color = 0xcd3b8d }
Province:register(tokushima)
local hiroshima = Province:create{ ref_name = "hiroshima", color = 0x449891 }
Province:register(hiroshima)
local okayama = Province:create{ ref_name = "okayama", color = 0x44986f }
Province:register(okayama)
local hyogo = Province:create{ ref_name = "hyogo", color = 0x729844 }
Province:register(hyogo)
local osaka = Province:create{ ref_name = "osaka", color = 0x987f44 }
Province:register(osaka)
local mie = Province:create{ ref_name = "mie", color = 0x709332 }
Province:register(mie)
local kyoto = Province:create{ ref_name = "kyoto", color = 0x4b6d12 }
Province:register(kyoto)
local gifu = Province:create{ ref_name = "gifu", color = 0x10164d }
Province:register(gifu)
local shizuoka = Province:create{ ref_name = "shizuoka", color = 0x90a06e }
Province:register(shizuoka)
local niigata = Province:create{ ref_name = "niigata", color = 0xa828a0 }
Province:register(niigata)
local yamanashi = Province:create{ ref_name = "yamanashi", color = 0x78933b }
Province:register(yamanashi)
local tokyo = Province:create{ ref_name = "tokyo", color = 0x306d11 }
Province:register(tokyo)
local ibaraki = Province:create{ ref_name = "ibaraki", color = 0x8d179b }
Province:register(ibaraki)
local fukushima = Province:create{ ref_name = "fukushima", color = 0x5e6180 }
Province:register(fukushima)
local tochigi = Province:create{ ref_name = "tochigi", color = 0x494498 }
Province:register(tochigi)
local iwate = Province:create{ ref_name = "iwate", color = 0x525d61 }
Province:register(iwate)
local akita = Province:create{ ref_name = "akita", color = 0x447698 }
Province:register(akita)
local aomori = Province:create{ ref_name = "aomori", color = 0x116d14 }
Province:register(aomori)
local hokkaido_tohoku = Province:create{ ref_name = "hokkaido_tohoku", color = 0x449893 }
Province:register(hokkaido_tohoku)
local nagano = Province:create{ ref_name = "nagano", color = 0x6d6111 }
Province:register(nagano)
local nagoya = Province:create{ ref_name = "nagoya", color = 0x938732 }
Province:register(nagoya)
local small_south_japan_islands = Province:create{ ref_name = "small_south_japan_islands", color = 0x31f455 }
Province:register(small_south_japan_islands)

-- islands north east of japan
local iturup = Province:create{ ref_name = "iturup", color = 0xf4ae31 }
Province:register(iturup)
local raycoke = Province:create{ ref_name = "raycoke", color = 0x62f431 }
Province:register(raycoke)
local paramushir = Province:create{ ref_name = "paramushir", color = 0xa131f4 }
Province:register(paramushir)

-- east russia
local kamchatka = Province:create{ ref_name = "kamchatka", color = 0x66b8a5 }
Province:register(kamchatka)
local south_kamchatka = Province:create{ ref_name = "south_kamchatka", color = 0x104d3f }
Province:register(south_kamchatka)
local amur = Province:create{ ref_name = "amur", color = 0x18268d }
Province:register(amur)
local khabarovsk = Province:create{ ref_name = "khabarovsk", color = 0x182ec9 }
Province:register(khabarovsk)
local south_khabarovsk = Province:create{ ref_name = "south_khabarovsk", color = 0xb066b8 }
Province:register(south_khabarovsk)
local primorsky_krai = Province:create{ ref_name = "primorsky_krai", color = 0x1e5431 }
Province:register(primorsky_krai)

-- sakhlin island
local north_sakhlin = Province:create{ ref_name = "north_sakhlin", color = 0x1f0d67 }
Province:register(north_sakhlin)
local central_sakhlin = Province:create{ ref_name = "central_sakhlin", color = 0x9421a2 }
Province:register(central_sakhlin)
local south_sakhlin = Province:create{ ref_name = "south_sakhlin", color = 0xf4316a }
Province:register(south_sakhlin)

-- korean peninsula
local east_north_korea = Province:create{ ref_name = "east_north_korea", color = 0x1e4954 }
Province:register(east_north_korea)
local west_north_korea = Province:create{ ref_name = "west_north_korea", color = 0x544d1e }
Province:register(west_north_korea)
local east_south_korea = Province:create{ ref_name = "east_south_korea", color = 0xca2ec4 }
Province:register(east_south_korea)
local west_south_korea = Province:create{ ref_name = "west_south_korea", color = 0x043440 }
Province:register(west_south_korea)
local southern_korea = Province:create{ ref_name = "southern_korea", color = 0x31f4dd }
Province:register(southern_korea)

print("loaded mod.lua")