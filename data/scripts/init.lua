require("api")

-- Culture
local german = Culture:create{ ref_name = "german" }
german.name = _("German")
Culture:register(german)

-- Religion
local protestant = Religion:create{ ref_name = "protestant" }
protestant.name = _("Protestant")
Religion:register(protestant)

-- Pop types
local entrepreneur = PopType:create{ ref_name = "entrepreneur" }
entrepreneur.name = _("Entrepreneur")
PopType:register(entrepreneur)
local artisan = PopType:create{ ref_name = "artisan" }
artisan.name = _("Artisan")
PopType:register(artisan)
local craftsmen = PopType:create{ ref_name = "craftsmen" }
craftsmen.name = _("Craftsmen")
PopType:register(craftsmen)
local bureaucrat = PopType:create{ ref_name = "bureaucrat" }
bureaucrat.name = _("Bureaucrat")
PopType:register(bureaucrat)
local aristocrat = PopType:create{ ref_name = "aristocrat" }
aristocrat.name = _("Aristocrat")
PopType:register(aristocrat)
local clergymen = PopType:create{ ref_name = "clergymen" }
clergymen.name = _("Clergymen")
PopType:register(clergymen)
local farmer = PopType:create{ ref_name = "farmer" }
farmer.name = _("Farmer")
PopType:register(farmer)
local soldier = PopType:create{ ref_name = "soldier" }
soldier.name = _("Soldier")
PopType:register(soldier)
local laborer = PopType:create{ ref_name = "laborer" }
laborer.name = _("Laborer")
PopType:register(laborer)
local slave = PopType:create{ ref_name = "slave" }
slave.name = _("Slave")
PopType:register(slave)

-- Good types

-- Farmable stuff
local wheat = Good:create{ ref_name = "wheat" }
wheat.name = _("Wheat")
Good:register(wheat)
local grapes = Good:create{ ref_name = "grapes" }
grapes.name = _("Grapes")
grapes.is_edible = true
Good:register(grapes)
local rice = Good:create{ ref_name = "rice" }
rice.name = _("Rice")
rice.is_edible = true
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
titanium.is_edible = true
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
bread.is_edible = true
Good:register(bread)
local salt = Good:create{ ref_name = "salt" }
salt.name = _("Salt")
salt.is_edible = true
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
IndustryType:add_output(rice_farm, rice)

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
local nation = Nation:create{ ref_name = "argentina", color = 0x00a9ff, default_flag = "flags/argentina_republic.png" }
nation.name = _("Argentina")
Nation:register(nation)

local nation = Nation:create{ ref_name = "brazil", color = 0x4CFF00, default_flag = "flags/brazil_monarchy.png" }
nation.name = _("Brazil")
Nation:register(nation)

local nation = Nation:create{ ref_name = "gran_colombia", color = 0xE1FF91, default_flag = "flags/gran_colombia_republic.png" }
nation.name = _("Gran colombia")
Nation:register(nation)

local nation = Nation:create{ ref_name = "peru", color = 0xFF2158, default_flag = "flags/peru_republic.png" }
nation.name = _("Peru")
Nation:register(nation)

local nation = Nation:create{ ref_name = "bolivia", color = 0xffc4ee, default_flag = "flags/bolivia_republic.png" }
nation.name = _("Bolivia")
Nation:register(nation)

local nation = Nation:create{ ref_name = "paraguay", color = 0xb71cff, default_flag = "flags/paraguay_republic.png" }
nation.name = _("Paraguay")
Nation:register(nation)

local nation = Nation:create{ ref_name = "chile", color = 0x241f3d, default_flag = "flags/chile_republic.png" }
nation.name = _("Chile")
Nation:register(nation)

local nation = Nation:create{ ref_name = "federated_republic_central_america", color = 0x5199ff, default_flag = "flags/federated_republic_central_america_republic.png" }
nation.name = _("Federated Republic of Central America")
Nation:register(nation)

local nation = Nation:create{ ref_name = "mexico", color = 0x92FF60, default_flag = "flags/mexico_republic.png" }
nation.name = _("Mexico")
Nation:register(nation)

local nation = Nation:create{ ref_name = "united_states", color = 0x639FFF, default_flag = "flags/united_states_democracy.png" }
nation.name = _("United states")
Nation:register(nation)

local nation = Nation:create{ ref_name = "united_kingdom", color = 0xff3262, default_flag = "flags/united_kingdom_monarchy.png" }
nation.name = _("United Kingdom")
Nation:register(nation)

local nation = Nation:create{ ref_name = "netherlands", color = 0xe79b46, default_flag = "flags/netherlands_monarchy.png" }
nation.name = _("Netherlands")
Nation:register(nation)

local nation = Nation:create{ ref_name = "france", color = 0x2e2bd6, default_flag = "flags/france_republic.png" }
nation.name = _("France")
Nation:register(nation)

local nation = Nation:create{ ref_name = "spain", color = 0xffe811, default_flag = "flags/france_republic.png" }
nation.name = _("Spain")
Nation:register(nation)

local nation = Nation:create{ ref_name = "sokoto", color = 0x008751, default_flag = "flags/sokoto_monarchy.png" }
nation.name = _("Sokoto")
Nation:register(nation)

local nation = Nation:create{ ref_name = "wadai_empire", color = 0xb9bc2a, default_flag = "flags/unknown.png" }
nation.name = _("Wadai Empire")
Nation:register(nation)

local nation = Nation:create{ ref_name = "bornu", color = 0x5f4b32, default_flag = "flags/bornu_monarchy.png" }
nation.name = _("Bornu")
Nation:register(nation)

local nation = Nation:create{ ref_name = "dahomey", color = 0x613478, default_flag = "flags/dahomey_monarchy.png" }
nation.name = _("Dahomey")
Nation:register(nation)

local nation = Nation:create{ ref_name = "austria", color = 0xf8f8f8, default_flag = "flags/austria_monarchy.png" }
nation.name = _("Austria")
Nation:register(nation)

local nation = Nation:create{ ref_name = "prussia", color = 0xffd416, default_flag = "flags/prussia_monarchy.png" }
nation.name = _("Prussia")
Nation:register(nation)
print("nations")

--------------------------------------------------------------------------------------------------------------------
local usa_east_transport = Company:create{ name = "USA East Transport", money = 10000000, is_transport = true }
Company:register(usa_east_transport)
local andes_transport = Company:create{ name = "Andes Transport", money = 10000000, is_transport = true }
Company:register(andes_transport)
local mexico_transport = Company:create{ name = "Mexico Transport", money = 10000000, is_transport = true }
Company:register(mexico_transport)
--------------------------------------------------------------------------------------------------------------------

-- Provinces

--------------------------------------------------------------
-- Continent: America
-- Subcontinent: South America
-- Country: Brazil

local province = Province:create{ ref_name = "ceara", color = 0x00FFFF }
province.name = _("Ceará")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "rio_grande_do_norte", color = 0xFF006E }
province.name = _("Rio Grande do Norte")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "paraiba", color = 0x4CFF00 }
province.name = _("Paraíba")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "pernambuco", color = 0x7F7F }
province.name = _("Pernambuco")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "alagoas", color = 0xB6FF00 }
province.name = _("Alagoas")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "sergipe", color = 0xFF6A00 }
province.name = _("Sergipe")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "bahia", color = 0xA0A0A0 }
province.name = _("Bahia")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "sao_francisco", color = 0xB200FF }
province.name = _("São Francisco")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "piaui", color = 0x49B0FF }
province.name = _("Piauí")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "maranhao", color = 0xC289FF }
province.name = _("Maranhão")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "grao_para", color = 0xFF6DDD }
province.name = _("Grão-Pará")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "roraima", color = 0xFFFD8E }
province.name = _("Roraima")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "mato_grosso", color = 0x9DFF35 }
province.name = _("Mato Grosso")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "goias", color = 0x49FF62 }
province.name = _("Goiás")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "minas_gerais", color = 0x9E0049 }
province.name = _("Minas Gerais")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "sao_paulo", color = 0x7FFF8E }
province.name = _("São Paulo")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "santa_catarina", color = 0xFF545F }
province.name = _("Santa Catarina")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "oeste_catarinense", color = 0xFF26A4 }
province.name = _("Oeste Catarinense")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "porto_alegre", color = 0x96FFF8 }
province.name = _("Porto Alegre")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "bage", color = 0xD089FF }
province.name = _("Bagé")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "cisplatina", color = 0xFFEA84 }
province.name = _("Cisplatina")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Paraguay

local province = Province:create{ ref_name = "asuncion", color = 0x00C7FF }
province.name = _("Asunción")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "vila_sacaron", color = 0xB71CFF }
province.name = _("Vila Sacarón")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "nioaque", color = 0x5174FF }
province.name = _("Nioaque")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Chile + Patagonian Chile + Ex-Bolivian territory

local province = Province:create{ ref_name = "iquique", color = 0xFF7856 }
province.name = _("Iquique")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "calama", color = 0x5157FF }
province.name = _("Calama")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "antofagasta", color = 0xF2FFD8 }
province.name = _("Antofagasta")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "copiapo", color = 0x9849FF }
province.name = _("Copiapó")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "coquimbo", color = 0x8C4FFF }
province.name = _("Coquimbo")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "illapel", color = 0x49F5FF }
province.name = _("Illapel")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "santiago", color = 0xCB23FF }
province.name = _("Santiago")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "curico", color = 0xFFE5C1 }
province.name = _("Curicó")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "chillan", color = 0x1691FF }
province.name = _("Chillán")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "concepcion", color = 0xFF4D3D }
province.name = _("Concepción")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "valdivia", color = 0xA2FF00 }
province.name = _("Valdivia")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "osorno", color = 0xFFE554 }
province.name = _("Osorno")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "chiloe", color = 0xFF00E5 }
province.name = _("Chiloé")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "aysen", color = 0x0090FF }
province.name = _("Aysén")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "punta_arenas", color = 0x3F3B39 }
province.name = _("Punta Arenas")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "san_gregorio", color = 0xFFD800 }
province.name = _("San Gregorio")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "puerto_porvenir", color = 0xFF006E }
province.name = _("Puerto Porvenir")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: North America
-- Country: Mexico

local province = Province:create{ ref_name = "yucatan", color = 0x567bff }
province.name = _("Yucatán")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "veracruz", color = 0xff758e }
province.name = _("Veracruz")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "coahuila_y_texas", color = 0xff7279 }
province.name = _("Coahuila y Texas")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "chihuahua", color = 0x38daff }
province.name = _("Chihuahua")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "alta_california", color = 0xc6cbff }
province.name = _("Alta California")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "baja_california", color = 0xff38e7 }
province.name = _("Baja California")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "chihuahua", color = 0x38daff }
province.name = _("Chihuahua")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "tamaulipas", color = 0xff87bd }
province.name = _("Tamaulipas")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "nuevo_leon", color = 0xbfffce }
province.name = _("Nuevo León")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "sonora", color = 0x9bffe9 }
province.name = _("Sonora")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "sinaloa", color = 0xffad87 }
province.name = _("Sinaloa")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "jalisco", color = 0x361cff }
province.name = _("Jalisco")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "durango", color = 0x92ff60 }
province.name = _("Durango")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "zacatecas", color = 0xff93a0 }
province.name = _("Zacatecas")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "san_luis_potosi", color = 0x3fffe8 }
province.name = _("San Luis Potosi")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "guanajuato", color = 0xf70fff }
province.name = _("Guanajuato")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "colima", color = 0xff7e5e }
province.name = _("Colima")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "michoacan", color = 0xc4ffc4 }
province.name = _("Michoacán")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "queretaro", color = 0xbbb2ff }
province.name = _("Querétaro")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "mexico", color = 0xff32e3 }
province.name = _("México")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "puebla", color = 0x927aff }
province.name = _("Puebla")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "tlaxcala", color = 0x89ffe1 }
province.name = _("Tlaxcala")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "oaxaca", color = 0x8effa8 }
province.name = _("Oaxaca")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "chiapas", color = 0xbf72ff }
province.name = _("Chiapas")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "nuevo_mexico", color = 0x56ffb8 }
province.name = _("Nuevo México")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: North America
-- Country: USA
local province = Province:create{ ref_name = "florida", color = 0x70a6ff }
province.name = _("Florida")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "connecticut", color = 0xe987ff }
province.name = _("Connecticut")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "new_jersey", color = 0xffaea8 }
province.name = _("New Jersey")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "rhode_island", color = 0x7b07ff }
province.name = _("Rhode Island")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "new_york", color = 0xbcfdff }
province.name = _("New York")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "massachusetts", color = 0x639fff }
province.name = _("Massachusetts")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "new_hampshire", color = 0xf830ff }
province.name = _("New Hampshire")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "vermont", color = 0xff4e32 }
province.name = _("Vermont")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "maine", color = 0xff4c7f }
province.name = _("Maine")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "delaware", color = 0xa372ff }
province.name = _("Delaware")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "maryland", color = 0xa5ffed }
province.name = _("Maryland")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "pensylvania", color = 0xff49a7 }
province.name = _("Pensylvania")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "virginia", color = 0xff7579 }
province.name = _("Virginia")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Bolivia + Chaco

local province = Province:create{ ref_name = "pando", color = 0x47FBFF }
province.name = _("Pando")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "santa_cruz", color = 0xFFC4EE }
province.name = _("Santa Cruz")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "carvoal", color = 0xFF8168 }
province.name = _("Carvoal")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "corumba", color = 0x8CFBFF }
province.name = _("Corumbá")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "santissima_trinidade", color = 0x5E1EFF }
province.name = _("Santíssima Trinidade")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "fuerte_olimpo", color = 0xFF1900 }
province.name = _("Fuerte Olimpo")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "nueva_burdeos", color = 0xFFD149 }
province.name = _("Nueva Burdeos")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "la_paz", color = 0xFF2158 }
province.name = _("La Paz")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "cochabamba", color = 0xCB3FFF }
province.name = _("Cochabamba")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "oruro", color = 0x75FFCC }
province.name = _("Oruro")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "potosi", color = 0xFC63FF }
province.name = _("Potosí")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "chuquisaca", color = 0x4FA7FF }
province.name = _("Chuquisaca")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

local province = Province:create{ ref_name = "tarija", color = 0x907CFF }
province.name = _("Tarija")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000)
Province:add_pop(province, farmer, german, protestant, 5000)
Province:add_pop(province, soldier, german, protestant, 5000)
Province:add_pop(province, craftsmen, german, protestant, 5000)
Province:add_pop(province, bureaucrat, german, protestant, 5000)
Province:add_pop(province, aristocrat, german, protestant, 5000)
Province:add_pop(province, clergymen, german, protestant, 5000)
Province:add_pop(province, laborer, german, protestant, 5000)
Province:add_pop(province, entrepreneur, german, protestant, 500)
Province:add_pop(province, slave, german, protestant, 500)

-- THIS IS NOT PART OF THE FINAL MODDING API

-- Companies

add_op_province_to_company(0, "maryland")
add_op_province_to_company(0, "virginia")
add_op_province_to_company(0, "delaware")
add_op_province_to_company(0, "maine")
add_op_province_to_company(0, "pensylvania")
add_op_province_to_company(0, "vermont")
add_op_province_to_company(0, "new_hampshire")
add_op_province_to_company(0, "massachusetts")
add_op_province_to_company(0, "new_york")
add_op_province_to_company(0, "rhode_island")
add_op_province_to_company(0, "new_jersey")
add_op_province_to_company(0, "connecticut")
add_op_province_to_company(0, "florida")

add_op_province_to_company(1, "sergipe")
add_op_province_to_company(1, "alagoas")
add_op_province_to_company(1, "sao_francisco")
add_op_province_to_company(1, "pernambuco")
add_op_province_to_company(1, "ceara")
add_op_province_to_company(1, "mato_grosso")
add_op_province_to_company(1, "goias")

add_op_province_to_company(2, "durango")
add_op_province_to_company(2, "san_luis_potosi")
add_op_province_to_company(2, "guanajuato")
add_op_province_to_company(2, "michoacan")
add_op_province_to_company(2, "colima")
add_op_province_to_company(2, "mexico")
add_op_province_to_company(2, "puebla")
add_op_province_to_company(2, "queretaro")
add_op_province_to_company(2, "tlaxcala")
add_op_province_to_company(2, "chiapas")
add_op_province_to_company(2, "oaxaca")
add_op_province_to_company(2, "nuevo_mexico")
add_op_province_to_company(2, "sinaloa")
add_op_province_to_company(2, "jalisco")
add_op_province_to_company(2, "baja_california")
add_op_province_to_company(2, "alta_california")
add_op_province_to_company(2, "sonora")
add_op_province_to_company(2, "tamaulipas")
add_op_province_to_company(2, "nuevo_leon")
add_op_province_to_company(2, "veracruz")
add_op_province_to_company(2, "coahuila_y_texas")
add_op_province_to_company(2, "chihuahua")
print("companies")

print("pops in provinces")

print("loaded mod.lua")
