require("api")

-- Unit traits
local trait = UnitTrait:create{ ref_name = "fanatic", attack_mod = 1.2 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "frugal", supply_consumption_mod = 0.7 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "stupid", attack_mod = 1.5, defense_mod = 0.8, supply_consumption_mod = 1.4 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "heroic", attack_mod = 0.7, defense_mod = 1.5 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "iron_teeth", supply_consumption_mod = 0.7 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "chargers", attack_mod = 2.0 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "zealous", defense_mod = 2.0 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "suicidal", attack_mod = 5.0, defense_mod = 0.1, supply_consumption_mod = 5.0 }
UnitTrait:register(trait)

-- Outpost types
local outpost_type = OutpostType:create{ ref_name = "barracks", is_build_land_units = true, defense_bonus = 1.05 }
OutpostType:register(outpost_type)
outpost_type = OutpostType:create{ ref_name = "fort", defense_bonus = 1.45 }
OutpostType:register(outpost_type)
outpost_type = OutpostType:create{ ref_name = "seaport", is_build_naval_units = true, defense_bonus = 1.05 }
OutpostType:register(outpost_type)
outpost_type = OutpostType:create{ ref_name = "coast_cannons", defense_bonus = 1.01 }
OutpostType:register(outpost_type)

-- Culture

-- European
german = Culture:create{ ref_name = "german" }
german.name = _("German")
Culture:register(german)
french = Culture:create{ ref_name = "french" }
french.name = _("French")
Culture:register(french)
british = Culture:create{ ref_name = "british" }
british.name = _("British")
Culture:register(british)
dutch = Culture:create{ ref_name = "dutch" }
dutch.name = _("Dutch")
Culture:register(dutch)
danish = Culture:create{ ref_name = "danish" }
danish.name = _("Danish")
Culture:register(danish)
icelandic = Culture:create{ ref_name = "icelandic" }
icelandic.name = _("Icelandic")
Culture:register(icelandic)
irish = Culture:create{ ref_name = "irish" }
irish.name = _("Irish")
Culture:register(irish)
portuguese = Culture:create{ ref_name = "portuguese" }
portuguese.name = _("Portuguese")
Culture:register(portuguese)
spanish = Culture:create{ ref_name = "spanish" }
spanish.name = _("Spanish")
Culture:register(spanish)
basque = Culture:create{ ref_name = "basque" }
basque.name = _("Basque")
Culture:register(basque)
galician = Culture:create{ ref_name = "galician" }
galician.name = _("Galician")
Culture:register(galician)
aragonese = Culture:create{ ref_name = "aragonese" }
aragonese.name = _("Aragonese")
Culture:register(aragonese)
catalan = Culture:create{ ref_name = "catalan" }
catalan.name = _("catalan")
Culture:register(catalan)
czech = Culture:create{ ref_name = "czech" }
czech.name = _("Czech")
Culture:register(czech)
slovakian = Culture:create{ ref_name = "slovakian" }
slovakian.name = _("Slovakian")
Culture:register(slovakian)
hungarian = Culture:create{ ref_name = "hungarian" }
hungarian.name = _("Hungarian")
Culture:register(hungarian)
russian = Culture:create{ ref_name = "russian" }
russian.name = _("Russian")
Culture:register(russian)
swedish = Culture:create{ ref_name = "swedish" }
swedish.name = _("Swedish")
Culture:register(swedish)
finnish = Culture:create{ ref_name = "finnish" }
finnish.name = _("Finnish")
Culture:register(finnish)
norwegian = Culture:create{ ref_name = "norwegian" }
norwegian.name = _("Norwegian")
Culture:register(norwegian)
turkish = Culture:create{ ref_name = "turkish" }
turkish.name = _("Turkish")
Culture:register(turkish)
greek = Culture:create{ ref_name = "greek" }
greek.name = _("Greek")
Culture:register(greek)

italian = Culture:create{ ref_name = "italian" }
italian.name = _("Italian")
Culture:register(italian)
lithuanian = Culture:create{ ref_name = "lithuanian" }
lithuanian.name = _("Lithuanian")
Culture:register(lithuanian)
latvian = Culture:create{ ref_name = "latvian" }
latvian.name = _("Latvian")
Culture:register(latvian)
estonian = Culture:create{ ref_name = "estonian" }
estonian.name = _("Estonian")
Culture:register(estonian)
polish = Culture:create{ ref_name = "polish" }
polish.name = _("Polish")
Culture:register(polish)

-- American
yankee = Culture:create{ ref_name = "yankee" }
yankee.name = _("Yankee")
Culture:register(yankee)
chilean = Culture:create{ ref_name = "chilean" }
chilean.name = _("Chilean")
Culture:register(chilean)
cisplatinian = Culture:create{ ref_name = "cisplatinian" }
cisplatinian.name = _("Cisplatinian")
Culture:register(cisplatinian)
argentinian = Culture:create{ ref_name = "argentinian" }
argentinian.name = _("Argentinian")
Culture:register(argentinian)
bolivian = Culture:create{ ref_name = "bolivian" }
bolivian.name = _("Bolivian")
Culture:register(bolivian)
paraguayan = Culture:create{ ref_name = "paraguayan" }
paraguayan.name = _("Paraguayan")
Culture:register(paraguayan)
colombian = Culture:create{ ref_name = "colombian" }
colombian.name = _("Colombian")
Culture:register(colombian)
venezuelan = Culture:create{ ref_name = "venezuelan" }
venezuelan.name = _("Venezuelan")
Culture:register(venezuelan)
ecuadorian = Culture:create{ ref_name = "ecuadorian" }
ecuadorian.name = _("Ecuadorian")
Culture:register(ecuadorian)
peruvian = Culture:create{ ref_name = "peruvian" }
peruvian.name = _("Peruvian")
Culture:register(peruvian)
mexican = Culture:create{ ref_name = "mexican" }
mexican.name = _("Mexican")
Culture:register(mexican)
haitian = Culture:create{ ref_name = "haitian" }
haitian.name = _("Haitian")
Culture:register(haitian)
brazilian = Culture:create{ ref_name = "brazilian" }
brazilian.name = _("Brazilian")
Culture:register(brazilian)

-- Native american
mapuche = Culture:create{ ref_name = "mapuche" }
mapuche.name = _("Mapuche")
Culture:register(mapuche)

-- Oceanic cultures
polynesian = Culture:create{ ref_name = "polynesian" }
polynesian.name = _("Polynesian")
Culture:register(polynesian)

-- Middle east
arabic = Culture:create{ ref_name = "arabic" }
arabic.name = _("Arabic")
Culture:register(arabic)

-- African
akan = Culture:create{ ref_name = "akan" }
akan.name = _("Akan")
Culture:register(akan)
abyssinian = Culture:create{ ref_name = "abyssinian" }
abyssinian.name = _("Abyssinian")
Culture:register(abyssinian)
amhara = Culture:create{ ref_name = "amhara" }
amhara.name = _("Amhara")
Culture:register(amhara)
chewa = Culture:create{ ref_name = "chewa" }
chewa.name = _("Chewa")
Culture:register(chewa)
fulani = Culture:create{ ref_name = "fulani" }
fulani.name = _("Fulani")
Culture:register(fulani)
hausa = Culture:create{ ref_name = "hausa" }
hausa.name = _("Hausa")
Culture:register(hausa)
hutu = Culture:create{ ref_name = "hutu" }
hutu.name = _("Hutu")
Culture:register(hutu)
igbo = Culture:create{ ref_name = "igbo" }
igbo.name = _("Igbo")
Culture:register(igbo)
kanuri = Culture:create{ ref_name = "kanuri" }
kanuri.name = _("Kanuri")
Culture:register(kanuri)
kongo = Culture:create{ ref_name = "kongo" }
kongo.name = _("Kongo")
Culture:register(kongo)
luba = Culture:create{ ref_name = "luba" }
luba.name = _("Luba")
Culture:register(luba)
maghrebis = Culture:create{ ref_name = "maghrebis" }
maghrebis.name = _("Maghrebis")
Culture:register(maghrebis)
mongo = Culture:create{ ref_name = "mongo" }
mongo.name = _("Mongo")
Culture:register(mongo)
nilotes = Culture:create{ ref_name = "nilotes" }
nilotes.name = _("Nilotes")
Culture:register(nilotes)
oromo = Culture:create{ ref_name = "oromo" }
oromo.name = _("Oromo")
Culture:register(oromo)
shona = Culture:create{ ref_name = "shona" }
shona.name = _("Shona")
Culture:register(shona)
somali = Culture:create{ ref_name = "somali" }
somali.name = _("Somali")
Culture:register(somali)
yoruba = Culture:create{ ref_name = "yoruba" }
yoruba.name = _("Yoruba")
Culture:register(yoruba)
zulu = Culture:create{ ref_name = "zulu" }
zulu.name = _("Zulu")
Culture:register(zulu)

-- Religion
protestant = Religion:create{ ref_name = "protestant" }
protestant.name = _("Protestant")
Religion:register(protestant)
catholic = Religion:create{ ref_name = "catholic" }
catholic.name = _("Catholic")
Religion:register(catholic)
orthodox = Religion:create{ ref_name = "orthodox" }
orthodox.name = _("Orthodox")
Religion:register(orthodox)
islamic = Religion:create{ ref_name = "islamic" }
islamic.name = _("Islamic")
Religion:register(islamic)
buddhist = Religion:create{ ref_name = "buddhist" }
buddhist.name = _("Buddhist")
Religion:register(buddhist)
animist = Religion:create{ ref_name = "animist" }
animist.name = _("Animist")
Religion:register(animist)

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
hawai_i = Nation:create{ ref_name = "hawai_i" }
hawai_i.name = _("Hawai'i")
Nation:register(hawai_i)
Nation:add_accepted_culture(hawai_i, polynesian)

require('nations/africa')
require('nations/america')
require('nations/europe')

--------------------------------------------------------------------------------------------------------------------
usa_east_transport = Company:create{ name = "USA East Transport", money = 10000000, is_transport = true }
Company:register(usa_east_transport)
andes_transport = Company:create{ name = "Andes Transport", money = 10000000, is_transport = true }
Company:register(andes_transport)
mexico_transport = Company:create{ name = "Mexico Transport", money = 10000000, is_transport = true }
Company:register(mexico_transport)
royal_russia_transport = Company:create{ name = "Royal Russia Transport", money = 10000000, is_transport = true }
Company:register(mexico_transport)
--------------------------------------------------------------------------------------------------------------------

-- Provinces

require('provinces/africa/northern')

require('provinces/americas/alaska')
require('provinces/americas/argentina')
require('provinces/americas/brazil_and_pantanal')
require('provinces/americas/canada')
require('provinces/americas/caribbean')
require('provinces/americas/central_america')
require('provinces/americas/guianas')
require('provinces/americas/mexico')
require('provinces/americas/south_america')
require('provinces/americas/united_states')

require('provinces/europe/austria')
require('provinces/europe/bohemia')
require('provinces/europe/denmark')
require('provinces/europe/france')
require('provinces/europe/scandinavia')

require('provinces/oceania/hawaii')

--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Middle Europe
-- Country: Saxony (Germany)
province = Province:create{ ref_name = "dresden", color = 0x8cb2ff }
province.name = _("Dresden")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000, 0.9)
Province:add_pop(province, farmer, german, protestant, 40000, 0.8)
Province:add_pop(province, soldier, german, protestant, 8000, 0.8)
Province:add_pop(province, craftsmen, german, protestant, 5000, 0.9)
Province:add_pop(province, bureaucrat, german, protestant, 5000, 0.9)
Province:add_pop(province, aristocrat, german, protestant, 5000, 0.9)
Province:add_pop(province, clergymen, german, protestant, 5000, 0.9)
Province:add_pop(province, laborer, german, protestant, 50000, 0.8)
Province:add_pop(province, entrepreneur, german, protestant, 800, 0.9)
Province:add_nucleus(province, saxony)
Province:give_to(province, saxony)
Nation:set_capital(saxony, province)

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
