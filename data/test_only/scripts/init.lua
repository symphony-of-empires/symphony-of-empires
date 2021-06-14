require("api")

-- Culture

-- European
english = Culture:create{ ref_name = "english" }
english.name = _("English")
Culture:register(english)
cheese = Culture:create{ ref_name = "cheese" }
cheese.name = _("Cheese")
Culture:register(cheese)

-- Religion
animist = Religion:create{ ref_name = "animist" }
animist.name = _("Animist")
Religion:register(animist)
not_animist = Religion:create{ ref_name = "not_animist" }
not_animist.name = _("Not animist")
Religion:register(not_animist)

-- Pop types
entrepreneur = PopType:create{ ref_name = "entrepreneur" }
entrepreneur.name = _("Entrepreneur")
PopType:register(entrepreneur)
artisan = PopType:create{ ref_name = "artisan" }
artisan.name = _("Artisan")
PopType:register(artisan)
craftsmen = PopType:create{ ref_name = "craftsmen" }
craftsmen.name = _("Craftsmen")
PopType:register(craftsmen)
bureaucrat = PopType:create{ ref_name = "bureaucrat" }
bureaucrat.name = _("Bureaucrat")
PopType:register(bureaucrat)
aristocrat = PopType:create{ ref_name = "aristocrat" }
aristocrat.name = _("Aristocrat")
PopType:register(aristocrat)
clergymen = PopType:create{ ref_name = "clergymen" }
clergymen.name = _("Clergymen")
PopType:register(clergymen)
farmer = PopType:create{ ref_name = "farmer" }
farmer.name = _("Farmer")
PopType:register(farmer)
soldier = PopType:create{ ref_name = "soldier" }
soldier.name = _("Soldier")
PopType:register(soldier)
officer = PopType:create{ ref_name = "officer" }
officer.name = _("Officer")
PopType:register(officer)
laborer = PopType:create{ ref_name = "laborer" }
laborer.name = _("Laborer")
PopType:register(laborer)
slave = PopType:create{ ref_name = "slave" }
slave.name = _("Slave")
PopType:register(slave)

-- Good types

-- Farmable stuff
wheat = Good:create{ ref_name = "wheat" }
wheat.name = _("Wheat")
Good:register(wheat)
grapes = Good:create{ ref_name = "grapes" }
grapes.name = _("Grapes")
grapes.is_edible = true
Good:register(grapes)
rice = Good:create{ ref_name = "rice" }
rice.name = _("Rice")
rice.is_edible = true
Good:register(rice)
cotton = Good:create{ ref_name = "cotton" }
cotton.name = _("Cotton")
Good:register(cotton)
coffe_bean = Good:create{ ref_name = "coffe_bean" }
coffe_bean.name = _("Coffe bean")
Good:register(coffe_bean)
salt_grain = Good:create{ ref_name = "salt_grain" }
salt_grain.name = _("Salt grain")
Good:register(salt_grain)
silk = Good:create{ ref_name = "silk" }
silk.name = _("Silk")
Good:register(silk)

-- Industrial materials
fabric = Good:create{ ref_name = "fabric" }
fabric.name = _("Fabric")
Good:register(fabric)
dye = Good:create{ ref_name = "dye" }
dye.name = _("Dye")
Good:register(dye)
textile = Good:create{ ref_name = "textile" }
textile.name = _("Textile")
Good:register(textile)
plastic = Good:create{ ref_name = "plastic" }
plastic.name = _("Plastic")
Good:register(plastic)
steel = Good:create{ ref_name = "steel" }
steel.name = _("Steel")
Good:register(steel)
charcoal = Good:create{ ref_name = "charcoal" }
charcoal.name = _("Charcoal")
Good:register(charcoal)

-- Military stuff
gunpowder = Good:create{ ref_name = "gunpowder" }
gunpowder.name = _("Gunpowder")
Good:register(gunpowder)
glock = Good:create{ ref_name = "glock" }
glock.name = _("Glock")
Good:register(glock)
cannon = Good:create{ ref_name = "cannon" }
cannon.name = _("Cannon")
Good:register(cannon)
revolver = Good:create{ ref_name = "revolver" }
revolver.name = _("Revolver")
Good:register(revolver)
musket = Good:create{ ref_name = "musket" }
musket.name = _("Musket")
Good:register(musket)
rifle = Good:create{ ref_name = "rifle" }
rifle.name = _("Rifle")
Good:register(rifle)
machinegun = Good:create{ ref_name = "machinegun" }
machinegun.name = _("Machinegun")
Good:register(machinegun)
ammunition = Good:create{ ref_name = "ammunition" }
ammunition.name = _("Ammunition")
Good:register(ammunition)
sail = Good:create{ ref_name = "sail" }
sail.name = _("Sail")
Good:register(sail)

-- Minerals
copper = Good:create{ ref_name = "copper" }
copper.name = _("Copper")
Good:register(copper)
silica = Good:create{ ref_name = "silica" }
silica.name = _("Silica")
Good:register(silica)
uranium = Good:create{ ref_name = "uranium" }
uranium.name = _("Uranium")
Good:register(uranium)
gold = Good:create{ ref_name = "gold" }
gold.name = _("Gold")
Good:register(gold)
iron = Good:create{ ref_name = "iron" }
iron.name = _("Iron")
Good:register(iron)
diamond = Good:create{ ref_name = "diamond" }
diamond.name = _("Diamond")
Good:register(diamond)
titanium = Good:create{ ref_name = "titanium" }
titanium.name = _("Titanium")
Good:register(titanium)
bronze = Good:create{ ref_name = "bronze" }
bronze.name = _("Bronze")
Good:register(bronze)
silver = Good:create{ ref_name = "silver" }
silver.name = _("Silver")
Good:register(silver)
platinum = Good:create{ ref_name = "platinum" }
platinum.name = _("Platinum")
Good:register(platinum)
plutonium = Good:create{ ref_name = "plutonium" }
plutonium.name = _("Plutonium")
Good:register(plutonium)
tin = Good:create{ ref_name = "tin" }
tin.name = _("Tin")
Good:register(tin)
tungsten = Good:create{ ref_name = "tungsten" }
tungsten.name = _("Tungsten")
Good:register(tungsten)
oil = Good:create{ ref_name = "oil" }
oil.name = _("Oil")
Good:register(oil)
sand = Good:create{ ref_name = "sand" }
sand.name = _("Sand")
Good:register(sand)
timber = Good:create{ ref_name = "timber" }
timber.name = _("Timber")
Good:register(timber)
coal = Good:create{ ref_name = "coal" }
coal.name = _("Coal")
Good:register(coal)
brass = Good:create{ ref_name = "brass" }
brass.name = _("Brass")
Good:register(brass)
sulfur = Good:create{ ref_name = "sulfur" }
sulfur.name = _("Sulfur")
Good:register(sulfur)
stainless_steel = Good:create{ ref_name = "stainless_steel" }
stainless_steel.name = _("Stainless steel")
Good:register(stainless_steel)

-- Food
glass = Good:create{ ref_name = "glass" }
glass.name = _("Glass")
Good:register(glass)
bread = Good:create{ ref_name = "bread" }
bread.name = _("Bread")
bread.is_edible = true
Good:register(bread)
salt = Good:create{ ref_name = "salt" }
salt.name = _("Salt")
salt.is_edible = true
Good:register(salt)

-- Commodities
window = Good:create{ ref_name = "window" }
window.name = _("Window")
Good:register(window)
wine = Good:create{ ref_name = "wine" }
wine.name = _("Wine")
Good:register(wine)
gasoline = Good:create{ ref_name = "gasoline" }
gasoline.name = _("Gasoline")
Good:register(gasoline)
chair = Good:create{ ref_name = "chair" }
chair.name = _("Chair")
Good:register(chair)
bed = Good:create{ ref_name = "bed" }
bed.name = _("Bed")
Good:register(bed)
opium = Good:create{ ref_name = "opium" }
opium.name = _("Opium")
Good:register(opium)
spices = Good:create{ ref_name = "spices" }
spices.name = _("Spices")
Good:register(spices)
print("good types")

-- Industry types

-- Primary sector industries (the most essential)
sand_smelter = IndustryType:create{ ref_name = "sand_smelter" }
sand_smelter.name = _("Sand smelter")
IndustryType:register(sand_smelter)
-- glass factories can create glass from thin air
IndustryType:add_output(sand_smelter, glass)

oil_refinery = IndustryType:create{ ref_name = "oil_refinery" }
oil_refinery.name = _("Oil refinery")
IndustryType:register(oil_refinery)
-- thin air oil, i think the air needs some democracy!
IndustryType:add_output(oil_refinery, oil)

lumberjack = IndustryType:create{ ref_name = "lumberjack" }
lumberjack.name = _("Lumberjack")
IndustryType:register(lumberjack)
-- same logic here
IndustryType:add_output(lumberjack, timber)

vineyard = IndustryType:create{ ref_name = "vineyard" }
vineyard.name = _("Vineyard")
IndustryType:register(vineyard)
IndustryType:add_output(vineyard, grapes)

wheat_farm = IndustryType:create{ ref_name = "wheat_farm" }
wheat_farm.name = _("Wheat farm")
IndustryType:register(wheat_farm)
IndustryType:add_output(wheat_farm, wheat)

rice_farm = IndustryType:create{ ref_name = "rice_farm" }
rice_farm.name = _("Rice farm")
IndustryType:register(rice_farm)
IndustryType:add_output(rice_farm, rice)

cotton_farm = IndustryType:create{ ref_name = "cotton_farm" }
cotton_farm.name = _("Cotton farm")
IndustryType:register(cotton_farm)
IndustryType:add_output(cotton_farm, wheat)

-- Mines can create stuff from thin air, but don't worry because this is
-- not abuse-ble
diamond_mine = IndustryType:create{ ref_name = "diamond_mine" }
diamond_mine.name = _("Diamond mine")
IndustryType:register(diamond_mine)
IndustryType:add_output(diamond_mine, diamond)

iron_mine = IndustryType:create{ ref_name = "iron_mine" }
iron_mine.name = _("Iron mine")
IndustryType:register(iron_mine)
IndustryType:add_output(iron_mine, iron)

gold_mine = IndustryType:create{ ref_name = "gold_mine" }
gold_mine.name = _("Gold mine")
IndustryType:register(gold_mine)
IndustryType:add_output(gold_mine, gold)

sulfur_mine = IndustryType:create{ ref_name = "sulfur_mine" }
sulfur_mine.name = _("Sulfur mine")
IndustryType:register(sulfur_mine)
IndustryType:add_output(sulfur_mine, sulfur)

titanium_mine = IndustryType:create{ ref_name = "titanium_mine" }
titanium_mine.name = _("Titanium mine")
IndustryType:register(titanium_mine)
IndustryType:add_output(titanium_mine, titanium)
print("primary sector industries")

-- Secondary sector industries (now the fun begins)
chair_manufacturer = IndustryType:create{ ref_name = "chair_manufacturer" }
chair_manufacturer.name = _("Chair manufacturer")
IndustryType:register(chair_manufacturer)
IndustryType:add_input(chair_manufacturer, timber)
IndustryType:add_output(chair_manufacturer, chair)

bed_manufacturer = IndustryType:create{ ref_name = "bed_manufacturer" }
bed_manufacturer.name = _("Bed manufacturer")
IndustryType:register(bed_manufacturer)
IndustryType:add_input(bed_manufacturer, timber)
IndustryType:add_output(bed_manufacturer, bed)

carbon_destroyer = IndustryType:create{ ref_name = "carbon_destroyer" }
carbon_destroyer.name = _("Carbon destroyer")
IndustryType:register(carbon_destroyer)
IndustryType:add_input(carbon_destroyer, coal)
IndustryType:add_output(carbon_destroyer, charcoal)

gunpowder_mixer = IndustryType:create{ ref_name = "gunpowder_mixer" }
gunpowder_mixer.name = _("Gunpowder mixer")
IndustryType:register(gunpowder_mixer)
IndustryType:add_input(gunpowder_mixer, sulfur)
IndustryType:add_input(gunpowder_mixer, charcoal)
IndustryType:add_output(gunpowder_mixer, gunpowder)

stainless_steel_smelter = IndustryType:create{ ref_name = "stainless_steel_smelter" }
stainless_steel_smelter.name = _("Stainless steel smelter")
IndustryType:register(stainless_steel_smelter)
IndustryType:add_input(stainless_steel_smelter, steel)
IndustryType:add_input(stainless_steel_smelter, coal)
IndustryType:add_output(stainless_steel_smelter, stainless_steel)

brass_smelter = IndustryType:create{ ref_name = "brass_smelter" }
brass_smelter.name = _("Brass smelter")
IndustryType:register(brass_smelter)
IndustryType:add_input(brass_smelter, stainless_steel)
IndustryType:add_input(brass_smelter, iron)
IndustryType:add_input(brass_smelter, coal)
IndustryType:add_output(brass_smelter, brass)

iron_smelter = IndustryType:create{ ref_name = "iron_smelter" }
iron_smelter.name = _("Iron smelter")
IndustryType:register(iron_smelter)
IndustryType:add_input(iron_smelter, iron)
IndustryType:add_input(iron_smelter, coal)
IndustryType:add_output(iron_smelter, steel)

musket_factory = IndustryType:create{ ref_name = "musket_factory" }
musket_factory.name = _("Musket factory")
IndustryType:register(musket_factory)
IndustryType:add_input(musket_factory, timber)
IndustryType:add_input(musket_factory, brass)
IndustryType:add_input(musket_factory, stainless_steel)
IndustryType:add_input(musket_factory, ammunition)
IndustryType:add_output(musket_factory, musket)

machinegun_factory = IndustryType:create{ ref_name = "machinegun_factory" }
machinegun_factory.name = _("Machinegun factory")
IndustryType:register(machinegun_factory)
IndustryType:add_input(machinegun_factory, brass)
IndustryType:add_input(machinegun_factory, stainless_steel)
IndustryType:add_input(machinegun_factory, ammunition)
IndustryType:add_input(machinegun_factory, coal)
IndustryType:add_input(machinegun_factory, musket)
IndustryType:add_output(machinegun_factory, machinegun)

ammunition_factory = IndustryType:create{ ref_name = "ammunition_factory" }
ammunition_factory.name = _("Ammunition factory")
IndustryType:register(ammunition_factory)
IndustryType:add_input(ammunition_factory, gunpowder)
IndustryType:add_input(ammunition_factory, coal)
IndustryType:add_output(ammunition_factory, ammunition)

sails_factory = IndustryType:create{ ref_name = "sails_factory" }
sails_factory.name = _("Sails factory")
IndustryType:register(sails_factory)
IndustryType:add_input(sails_factory, textile)
IndustryType:add_output(sails_factory, sail)
print("secondary sector industries")

-- Unit types
garrison = UnitType:create{ ref_name = "garrison", defense = 5.0, attack = 2.0, health = 100.0 }
garrison.name = _("Garrison")
UnitType:register(garrison)
UnitType:requires_good(garrison, ammunition, 200)
UnitType:requires_good(garrison, musket, 100)
infantry = UnitType:create{ ref_name = "infantry", defense = 5.0, attack = 2.0, health = 100.0 }
infantry.name = _("Infantry")
UnitType:register(infantry)
UnitType:requires_good(infantry, ammunition, 200)
UnitType:requires_good(infantry, musket, 100)
cavalry = UnitType:create{ ref_name = "cavalry", defense = 2.0, attack = 15.0, health = 50.0 }
cavalry.name = _("Cavalry")
UnitType:register(cavalry)
UnitType:requires_good(cavalry, ammunition, 200)
UnitType:requires_good(cavalry, musket, 100)
artillery = UnitType:create{ ref_name = "artillery", defense = 2.0, attack = 15.0, health = 50.0 }
artillery.name = _("Artillery")
UnitType:register(artillery)
UnitType:requires_good(artillery, ammunition, 500)
UnitType:requires_good(artillery, gunpowder, 500)

-- Naval unit types
clipper = BoatType:create{ ref_name = "clipper", defense = 10.0, attack = 10.0, health = 100.0, capacity = 4000 }
clipper.name = _("Clipper")
BoatType:register(clipper)
BoatType:requires_good(clipper, timber, 800)
BoatType:requires_good(clipper, sail, 20)
BoatType:requires_good(clipper, gunpowder, 200)
barque = BoatType:create{ ref_name = "barque", defense = 15.0, attack = 10.0, health = 200.0, capacity = 400 }
barque.name = _("Barque")
BoatType:register(barque)
BoatType:requires_good(barque, timber, 800)
BoatType:requires_good(barque, sail, 20)
BoatType:requires_good(barque, gunpowder, 200)
sloop_of_war = BoatType:create{ ref_name = "sloop_of_war", defense = 40.0, attack = 20.0, health = 400.0, capacity = 400 }
sloop_of_war.name = _("Sloop of war")
BoatType:register(sloop_of_war)
BoatType:requires_good(sloop_of_war, timber, 1000)
BoatType:requires_good(sloop_of_war, sail, 20)
BoatType:requires_good(sloop_of_war, gunpowder, 400)
man_of_war = BoatType:create{ ref_name = "man_of_war", defense = 20.0, attack = 20.0, health = 500.0, capacity = 500 }
man_of_war.name = _("Man of war")
BoatType:register(man_of_war)
BoatType:requires_good(man_of_war, timber, 1500)
BoatType:requires_good(man_of_war, sail, 30)
BoatType:requires_good(man_of_war, gunpowder, 500)

-- Nations
cplusplus = Nation:create{ ref_name = "cplusplus", color = 0x008751, default_flag = "flags/cplusplus.png" }
cplusplus.name = _("C-Plus o' Plus")
Nation:register(cplusplus)
assembly = Nation:create{ ref_name = "assembly", color = 0x0ef7f1, default_flag = "flags/assembly.png" }
assembly.name = _("Assembly")
Nation:register(assembly)
someland = Nation:create{ ref_name = "someland", color = 0xf08751, default_flag = "flags/someland.png" }
someland.name = _("Someland")
Nation:register(someland)
aristocrat = Nation:create{ ref_name = "aristocrat", color = 0x471812, default_flag = "flags/aristocrat.png" }
aristocrat.name = _("Aristocrat")
Nation:register(aristocrat)
corner = Nation:create{ ref_name = "corner", color = 0x86728, default_flag = "flags/corner.png" }
corner.name = _("Corner")
Nation:register(corner)
overpowered = Nation:create{ ref_name = "overpowered", color = 0x461812, default_flag = "flags/overpowered.png" }
overpowered.name = _("Overpowered")
Nation:register(overpowered)

cplusplus_transport = Company:create{ name = "C-Plus o' Plus Transport", money = 10000000, is_transport = true }
Company:register(cplusplus_transport)
c_transport = Company:create{ name = "C Transport", money = 10000000, is_transport = true }
Company:register(c_transport)

--------------------------------------------------------------
province = Province:create{ ref_name = "mistake", color = 0x42e22b }
province.name = _("Mistake")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, someland)
Province:give_to(province, someland)
Nation:set_capital(someland, province)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "cake", color = 0x2be2d1 }
province.name = _("Cake")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, someland)
Province:give_to(province, someland)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "point_cake", color = 0xe28b2b }
province.name = _("Point Cake")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Nation:set_capital(cplusplus, province)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "not_cake", color = 0xe2e12b }
province.name = _("Not cake")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "wounded", color = 0xd0ca9e }
province.name = _("Wounded")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, corner)
Province:give_to(province, corner)
Nation:set_capital(corner, province)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "life", color = 0x87d869 }
province.name = _("Life")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, corner)
Province:give_to(province, corner)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "city_of_gold", color = 0x6971d8 }
province.name = _("City of gold")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, corner)
Province:give_to(province, corner)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "greenland", color = 0x69b6d8 }
province.name = _("Greenland")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, aristocrat)
Province:give_to(province, aristocrat)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "not_greenland", color = 0xd0b29e }
province.name = _("Not greenland")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, aristocrat)
Province:give_to(province, aristocrat)
Nation:set_capital(aristocrat, province)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "north_lands", color = 0xd66ab5 }
province.name = _("North lands")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, aristocrat)
Province:give_to(province, aristocrat)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "disputa_asuncion", color = 0x28c045 }
province.name = _("Disputa asuncion")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "ascender", color = 0x5228c0 }
province.name = _("Ascender")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "detruir", color = 0x2862c0 }
province.name = _("Detruir")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "pastel", color = 0x28b9c0 }
province.name = _("Pastel")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "isla", color = 0x9828c0 }
province.name = _("Isla")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "puerto_vivo", color = 0xb9ff00 }
province.name = _("Puerto vivo")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "puerto_si_vivo", color = 0xa7d376 }
province.name = _("Puerto si vivo")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "nombre_no", color = 0xc0a628 }
province.name = _("Nombre no")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "senior", color = 0xd3c976 }
province.name = _("Senior")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "io_bus", color = 0x9bc028 }
province.name = _("Io bus")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "day_land", color = 0x75c028 }
province.name = _("Day land")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "daisies", color = 0x2cbcff }
province.name = _("Daisies")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "south_senior", color = 0x859d00 }
province.name = _("South senior")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "port_wellingston", color = 0x3c5fff }
province.name = _("Port wellingston")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "ham", color = 0xe45fff }
province.name = _("Ham")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "supergreenland", color = 0xd69c6a }
province.name = _("Supergreenland")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "bread", color = 0x52cfff }
province.name = _("Bread")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "port_ol_coast", color = 0x4657ff }
province.name = _("Port ol' coast")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "dice", color = 0x8cc200 }
province.name = _("Dice")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "sea_of_land", color = 0xb961ff }
province.name = _("Sea of land")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "neutron", color = 0x4676ff }
province.name = _("Neutron")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "helium", color = 0x2993ff }
province.name = _("Helium")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "hydrogen", color = 0x78e5ff }
province.name = _("Hydrogen")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "mercury", color = 0x6778ff }
province.name = _("Mercury")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "offensive", color = 0x67c6ff }
province.name = _("Offensive")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "complete", color = 0x68d9ff }
province.name = _("Complete")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "alpha", color = 0xb9c1ff }
province.name = _("Alpha")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, aristocrat)
Province:give_to(province, aristocrat)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "delta", color = 0x94ff00 }
province.name = _("Delta")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "kept", color = 0xd09eac }
province.name = _("Kept")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "stonks", color = 0x2bbae2 }
province.name = _("Stonks")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, aristocrat)
Province:give_to(province, aristocrat)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "pain", color = 0xe22b8b }
province.name = _("Pain")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "long_island", color = 0xe2a92b }
province.name = _("Long island")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "west_copper", color = 0xffd600 }
province.name = _("West copper")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "copper", color = 0xff8000 }
province.name = _("Copper")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "top_hat_coast", color = 0x28c09c }
province.name = _("Top hat coast")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "credit", color = 0x29ff00 }
province.name = _("Credit")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "debit", color = 0xd8b969 }
province.name = _("Debit")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "behaviour", color = 0x51ff00 }
province.name = _("Behaviour")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "stack", color = 0x00ffaf }
province.name = _("Stack")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "red_zone", color = 0x69d8d1 }
province.name = _("Red zone")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "null", color = 0xd89d69 }
province.name = _("Null")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "unregistered", color = 0xbc84ff }
province.name = _("Unregistered")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, assembly)
Province:give_to(province, assembly)
Province:add_industry(province, rice_farm, cplusplus_transport)

province = Province:create{ ref_name = "risky_land", color = 0x6aaed6 }
province.name = _("Risky land")
Province:register(province)
Province:add_pop(province, artisan, english, animist, 1000, 0.6)
Province:add_pop(province, farmer, english, animist, 5000, 0.2)
Province:add_pop(province, soldier, english, animist, 1000, 0.6)
Province:add_pop(province, craftsmen, english, animist, 1000, 0.4)
Province:add_pop(province, bureaucrat, english, animist, 1000, 0.8)
Province:add_pop(province, aristocrat, english, animist, 1000, 0.7)
Province:add_pop(province, clergymen, english, animist, 1000, 0.6)
Province:add_pop(province, laborer, english, animist, 1000, 0.4)
Province:add_pop(province, entrepreneur, english, animist, 1000, 0.9)
Province:add_nucleus(province, cplusplus)
Province:give_to(province, cplusplus)
Province:add_industry(province, rice_farm, cplusplus_transport)

print("pops in provinces")

print("loaded mod.lua")
