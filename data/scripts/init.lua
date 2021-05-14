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

local nation = Nation:create{ ref_name = "sokoto", color = 0x5f4b32, default_flag = "flags/sokoto_monarchy.png" }
nation.name = _("Sokoto")
Nation:register(nation)

local nation = Nation:create{ ref_name = "wadai_empire", color = 0xb9bc2a, default_flag = "flags/unknown.png" }
nation.name = _("Wadai Empire")
Nation:register(nation)

local nation = Nation:create{ ref_name = "bornu", color = 0x589172, default_flag = "flags/bornu_monarchy.png" }
nation.name = _("Bornu")
Nation:register(nation)

local nation = Nation:create{ ref_name = "dahomey", color = 0x613478, default_flag = "flags/dahomey_monarchy.png" }
nation.name = _("Dahomey")
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

local name_of_province = Province:create{ ref_name = "ceara", color = 0x00FFFF }
name_of_province.name = _("Ceará")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "rio_grande_do_norte", color = 0xFF006E }
name_of_province.name = _("Rio Grande do Norte")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "paraiba", color = 0x4CFF00 }
name_of_province.name = _("Paraíba")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "pernambuco", color = 0x7F7F }
name_of_province.name = _("Pernambuco")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "alagoas", color = 0xB6FF00 }
name_of_province.name = _("Alagoas")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "sergipe", color = 0xFF6A00 }
name_of_province.name = _("Sergipe")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "bahia", color = 0xA0A0A0 }
name_of_province.name = _("Bahia")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "sao_francisco", color = 0xB200FF }
name_of_province.name = _("São Francisco")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "piaui", color = 0x49B0FF }
name_of_province.name = _("Piauí")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "maranhao", color = 0xC289FF }
name_of_province.name = _("Maranhão")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "grao_para", color = 0xFF6DDD }
name_of_province.name = _("Grão-Pará")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "roraima", color = 0xFFFD8E }
name_of_province.name = _("Roraima")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "mato_grosso", color = 0x9DFF35 }
name_of_province.name = _("Mato Grosso")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "goias", color = 0x49FF62 }
name_of_province.name = _("Goiás")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "minas_gerais", color = 0x9E0049 }
name_of_province.name = _("Minas Gerais")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "sao_paulo", color = 0x7FFF8E }
name_of_province.name = _("São Paulo")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "santa_catarina", color = 0xFF545F }
name_of_province.name = _("Santa Catarina")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "oeste_catarinense", color = 0xFF26A4 }
name_of_province.name = _("Oeste Catarinense")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "porto_alegre", color = 0x96FFF8 }
name_of_province.name = _("Porto Alegre")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "bage", color = 0xD089FF }
name_of_province.name = _("Bagé")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "cisplatina", color = 0xFFEA84 }
name_of_province.name = _("Cisplatina")
Province:register(name_of_province)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Paraguay

local name_of_province = Province:create{ ref_name = "asuncion", color = 0x00C7FF }
name_of_province.name = _("Asunción")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "vila_sacaron", color = 0xB71CFF }
name_of_province.name = _("Vila Sacarón")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "nioaque", color = 0x5174FF }
name_of_province.name = _("Nioaque")
Province:register(name_of_province)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Chile + Patagonian Chile + Ex-Bolivian territory

local name_of_province = Province:create{ ref_name = "iquique", color = 0xFF7856 }
name_of_province.name = _("Iquique")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "calama", color = 0x5157FF }
name_of_province.name = _("Calama")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "antofagasta", color = 0xF2FFD8 }
name_of_province.name = _("Antofagasta")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "copiapo", color = 0x9849FF }
name_of_province.name = _("Copiapó")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "coquimbo", color = 0x8C4FFF }
name_of_province.name = _("Coquimbo")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "illapel", color = 0x49F5FF }
name_of_province.name = _("Illapel")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "santiago", color = 0xCB23FF }
name_of_province.name = _("Santiago")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "curico", color = 0xFFE5C1 }
name_of_province.name = _("Curicó")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "chillan", color = 0x1691FF }
name_of_province.name = _("Chillán")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "concepcion", color = 0xFF4D3D }
name_of_province.name = _("Concepción")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "valdivia", color = 0xA2FF00 }
name_of_province.name = _("Valdivia")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "osorno", color = 0xFFE554 }
name_of_province.name = _("Osorno")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "chiloe", color = 0xFF00E5 }
name_of_province.name = _("Chiloé")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "aysen", color = 0x0090FF }
name_of_province.name = _("Aysén")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "punta_arenas", color = 0x3F3B39 }
name_of_province.name = _("Punta Arenas")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "san_gregorio", color = 0xFFD800 }
name_of_province.name = _("San Gregorio")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "puerto_porvenir", color = 0xFF006E }
name_of_province.name = _("Puerto Porvenir")
Province:register(name_of_province)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: North America
-- Country: Mexico

local name_of_province = Province:create{ ref_name = "yucatan", color = 0x567bff }
name_of_province.name = _("Yucatán")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "veracruz", color = 0xff758e }
name_of_province.name = _("Veracruz")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "coahuila_y_texas", color = 0xff7279 }
name_of_province.name = _("Coahuila y Texas")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "chihuahua", color = 0x38daff }
name_of_province.name = _("Chihuahua")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "alta_california", color = 0xc6cbff }
name_of_province.name = _("Alta California")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "baja_california", color = 0xff38e7 }
name_of_province.name = _("Baja California")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "chihuahua", color = 0x38daff }
name_of_province.name = _("Chihuahua")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "tamaulipas", color = 0xff87bd }
name_of_province.name = _("Tamaulipas")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "nuevo_leon", color = 0xbfffce }
name_of_province.name = _("Nuevo León")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "sonora", color = 0x9bffe9 }
name_of_province.name = _("Sonora")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "sinaloa", color = 0xffad87 }
name_of_province.name = _("Sinaloa")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "jalisco", color = 0x361cff }
name_of_province.name = _("Jalisco")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "durango", color = 0x92ff60 }
name_of_province.name = _("Durango")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "zacatecas", color = 0xff93a0 }
name_of_province.name = _("Zacatecas")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "san_luis_potosi", color = 0x3fffe8 }
name_of_province.name = _("San Luis Potosi")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "guanajuato", color = 0xf70fff }
name_of_province.name = _("Guanajuato")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "colima", color = 0xff7e5e }
name_of_province.name = _("Colima")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "michoacan", color = 0xc4ffc4 }
name_of_province.name = _("Michoacán")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "queretaro", color = 0xbbb2ff }
name_of_province.name = _("Querétaro")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "mexico", color = 0xff32e3 }
name_of_province.name = _("México")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "puebla", color = 0x927aff }
name_of_province.name = _("Puebla")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "tlaxcala", color = 0x89ffe1 }
name_of_province.name = _("Tlaxcala")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "oaxaca", color = 0x8effa8 }
name_of_province.name = _("Oaxaca")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "chiapas", color = 0xbf72ff }
name_of_province.name = _("Chiapas")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "nuevo_mexico", color = 0x56ffb8 }
name_of_province.name = _("Nuevo México")
Province:register(name_of_province)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: North America
-- Country: USA
local name_of_province = Province:create{ ref_name = "florida", color = 0x70a6ff }
name_of_province.name = _("Florida")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "connecticut", color = 0xe987ff }
name_of_province.name = _("Connecticut")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "new_jersey", color = 0xffaea8 }
name_of_province.name = _("New Jersey")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "rhode_island", color = 0x7b07ff }
name_of_province.name = _("Rhode Island")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "new_york", color = 0xbcfdff }
name_of_province.name = _("New York")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "massachusetts", color = 0x639fff }
name_of_province.name = _("Massachusetts")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "new_hampshire", color = 0xf830ff }
name_of_province.name = _("New Hampshire")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "vermont", color = 0xff4e32 }
name_of_province.name = _("Vermont")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "maine", color = 0xff4c7f }
name_of_province.name = _("Maine")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "delaware", color = 0xa372ff }
name_of_province.name = _("Delaware")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "maryland", color = 0xa5ffed }
name_of_province.name = _("Maryland")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "pensylvania", color = 0xff49a7 }
name_of_province.name = _("Pensylvania")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "virginia", color = 0xff7579 }
name_of_province.name = _("Virginia")
Province:register(name_of_province)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Bolivia + Chaco

local name_of_province = Province:create{ ref_name = "pando", color = 0x47FBFF }
name_of_province.name = _("Pando")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "santa_cruz", color = 0xFFC4EE }
name_of_province.name = _("Santa Cruz")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "carvoal", color = 0xFF8168 }
name_of_province.name = _("Carvoal")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "corumba", color = 0x8CFBFF }
name_of_province.name = _("Corumbá")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "santissima_trinidade", color = 0x5E1EFF }
name_of_province.name = _("Santíssima Trinidade")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "fuerte_olimpo", color = 0xFF1900 }
name_of_province.name = _("Fuerte Olimpo")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "nueva_burdeos", color = 0xFFD149 }
name_of_province.name = _("Nueva Burdeos")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "la_paz", color = 0xFF2158 }
name_of_province.name = _("La Paz")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "cochabamba", color = 0xCB3FFF }
name_of_province.name = _("Cochabamba")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "oruro", color = 0x75FFCC }
name_of_province.name = _("Oruro")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "potosi", color = 0xFC63FF }
name_of_province.name = _("Potosí")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "chuquisaca", color = 0x4FA7FF }
name_of_province.name = _("Chuquisaca")
Province:register(name_of_province)

local name_of_province = Province:create{ ref_name = "tarija", color = 0x907CFF }
name_of_province.name = _("Tarija")
Province:register(name_of_province)

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

local province = Province:get({}, "virginia")
Province:add_pop(province, artisan, japanesse, good_bois, 5000)
Province:add_pop(province, farmer, japanesse, good_bois, 5000)
Province:add_pop(province, soldier, japanesse, good_bois, 5000)
Province:add_pop(province, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(province, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(province, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(province, clergymen, japanesse, good_bois, 5000)
Province:add_pop(province, laborer, japanesse, good_bois, 5000)
Province:add_pop(province, entrepreneur, japanesse, good_bois, 500)

local province = Province:get({}, "vermont")
Province:add_pop(province, artisan, japanesse, good_bois, 5000)
Province:add_pop(province, farmer, japanesse, good_bois, 5000)
Province:add_pop(province, soldier, japanesse, good_bois, 5000)
Province:add_pop(province, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(province, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(province, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(province, clergymen, japanesse, good_bois, 5000)
Province:add_pop(province, laborer, japanesse, good_bois, 5000)
Province:add_pop(province, entrepreneur, japanesse, good_bois, 500)

local province = Province:get({}, "new_jersey")
Province:add_pop(province, artisan, japanesse, good_bois, 5000)
Province:add_pop(province, farmer, japanesse, good_bois, 5000)
Province:add_pop(province, soldier, japanesse, good_bois, 5000)
Province:add_pop(province, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(province, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(province, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(province, clergymen, japanesse, good_bois, 5000)
Province:add_pop(province, laborer, japanesse, good_bois, 5000)
Province:add_pop(province, entrepreneur, japanesse, good_bois, 500)

local province = Province:get({}, "new_york")
Province:add_pop(province, artisan, japanesse, good_bois, 5000)
Province:add_pop(province, farmer, japanesse, good_bois, 5000)
Province:add_pop(province, soldier, japanesse, good_bois, 5000)
Province:add_pop(province, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(province, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(province, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(province, clergymen, japanesse, good_bois, 5000)
Province:add_pop(province, laborer, japanesse, good_bois, 5000)
Province:add_pop(province, entrepreneur, japanesse, good_bois, 500)

local province = Province:get({}, "maine")
Province:add_pop(province, artisan, japanesse, good_bois, 5000)
Province:add_pop(province, farmer, japanesse, good_bois, 5000)
Province:add_pop(province, soldier, japanesse, good_bois, 5000)
Province:add_pop(province, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(province, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(province, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(province, clergymen, japanesse, good_bois, 5000)
Province:add_pop(province, laborer, japanesse, good_bois, 5000)
Province:add_pop(province, entrepreneur, japanesse, good_bois, 500)

local province = Province:get({}, "colima")
Province:add_pop(province, artisan, japanesse, good_bois, 5000)
Province:add_pop(province, farmer, japanesse, good_bois, 5000)
Province:add_pop(province, soldier, japanesse, good_bois, 5000)
Province:add_pop(province, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(province, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(province, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(province, clergymen, japanesse, good_bois, 5000)
Province:add_pop(province, laborer, japanesse, good_bois, 5000)
Province:add_pop(province, entrepreneur, japanesse, good_bois, 500)

local province = Province:get({}, "mexico")
Province:add_pop(province, artisan, japanesse, good_bois, 5000)
Province:add_pop(province, farmer, japanesse, good_bois, 5000)
Province:add_pop(province, soldier, japanesse, good_bois, 5000)
Province:add_pop(province, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(province, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(province, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(province, clergymen, japanesse, good_bois, 5000)
Province:add_pop(province, laborer, japanesse, good_bois, 5000)
Province:add_pop(province, entrepreneur, japanesse, good_bois, 500)

local province = Province:get({}, "rhode_island")
Province:add_pop(province, artisan, japanesse, good_bois, 5000)
Province:add_pop(province, farmer, japanesse, good_bois, 5000)
Province:add_pop(province, soldier, japanesse, good_bois, 5000)
Province:add_pop(province, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(province, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(province, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(province, clergymen, japanesse, good_bois, 5000)
Province:add_pop(province, laborer, japanesse, good_bois, 5000)
Province:add_pop(province, entrepreneur, japanesse, good_bois, 500)

local province = Province:get({}, "oaxaca")
Province:add_pop(province, artisan, japanesse, good_bois, 5000)
Province:add_pop(province, farmer, japanesse, good_bois, 5000)
Province:add_pop(province, soldier, japanesse, good_bois, 5000)
Province:add_pop(province, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(province, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(province, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(province, clergymen, japanesse, good_bois, 5000)
Province:add_pop(province, laborer, japanesse, good_bois, 5000)
Province:add_pop(province, entrepreneur, japanesse, good_bois, 500)

local province = Province:get({}, "tlaxcala")
Province:add_pop(province, artisan, japanesse, good_bois, 5000)
Province:add_pop(province, farmer, japanesse, good_bois, 5000)
Province:add_pop(province, soldier, japanesse, good_bois, 5000)
Province:add_pop(province, craftsmen, japanesse, good_bois, 5000)
Province:add_pop(province, bureaucrat, japanesse, good_bois, 5000)
Province:add_pop(province, aristocrat, japanesse, good_bois, 5000)
Province:add_pop(province, clergymen, japanesse, good_bois, 5000)
Province:add_pop(province, laborer, japanesse, good_bois, 5000)
Province:add_pop(province, entrepreneur, japanesse, good_bois, 500)
print("pops in provinces")

print("loaded mod.lua")
