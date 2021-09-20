-- Industry types

-- Primary sector industries (the most essential)
sand_smelter = IndustryType:new{ ref_name = "sand_smelter" }
sand_smelter.name = _("Sand smelter")
IndustryType:register(sand_smelter)
-- glass factories can new glass from thin air
IndustryType:add_output(sand_smelter, glass)

oil_refinery = IndustryType:new{ ref_name = "oil_refinery" }
oil_refinery.name = _("Oil refinery")
IndustryType:register(oil_refinery)
-- thin air oil, i think the air needs some democracy!
IndustryType:add_output(oil_refinery, oil)

lumberjack = IndustryType:new{ ref_name = "lumberjack" }
lumberjack.name = _("Lumberjack")
IndustryType:register(lumberjack)
-- same logic here
IndustryType:add_output(lumberjack, timber)

vineyard = IndustryType:new{ ref_name = "vineyard" }
vineyard.name = _("Vineyard")
IndustryType:register(vineyard)
IndustryType:add_output(vineyard, grapes)

wheat_farm = IndustryType:new{ ref_name = "wheat_farm" }
wheat_farm.name = _("Wheat farm")
IndustryType:register(wheat_farm)
IndustryType:add_output(wheat_farm, wheat)

rice_farm = IndustryType:new{ ref_name = "rice_farm" }
rice_farm.name = _("Rice farm")
IndustryType:register(rice_farm)
IndustryType:add_output(rice_farm, rice)

cotton_farm = IndustryType:new{ ref_name = "cotton_farm" }
cotton_farm.name = _("Cotton farm")
IndustryType:register(cotton_farm)
IndustryType:add_output(cotton_farm, wheat)

-- Mines can new stuff from thin air, but don't worry because this is
-- not abuse-ble
diamond_mine = IndustryType:new{ ref_name = "diamond_mine" }
diamond_mine.name = _("Diamond mine")
IndustryType:register(diamond_mine)
IndustryType:add_output(diamond_mine, diamond)

iron_mine = IndustryType:new{ ref_name = "iron_mine" }
iron_mine.name = _("Iron mine")
IndustryType:register(iron_mine)
IndustryType:add_output(iron_mine, iron)

gold_mine = IndustryType:new{ ref_name = "gold_mine" }
gold_mine.name = _("Gold mine")
IndustryType:register(gold_mine)
IndustryType:add_output(gold_mine, gold)

sulfur_mine = IndustryType:new{ ref_name = "sulfur_mine" }
sulfur_mine.name = _("Sulfur mine")
IndustryType:register(sulfur_mine)
IndustryType:add_output(sulfur_mine, sulfur)

titanium_mine = IndustryType:new{ ref_name = "titanium_mine" }
titanium_mine.name = _("Titanium mine")
IndustryType:register(titanium_mine)
IndustryType:add_output(titanium_mine, titanium)

-- Secondary sector industries (now the fun begins)
chair_manufacturer = IndustryType:new{ ref_name = "chair_manufacturer" }
chair_manufacturer.name = _("Chair manufacturer")
IndustryType:register(chair_manufacturer)
IndustryType:add_input(chair_manufacturer, timber)
IndustryType:add_output(chair_manufacturer, chair)

bed_manufacturer = IndustryType:new{ ref_name = "bed_manufacturer" }
bed_manufacturer.name = _("Bed manufacturer")
IndustryType:register(bed_manufacturer)
IndustryType:add_input(bed_manufacturer, timber)
IndustryType:add_output(bed_manufacturer, bed)

carbon_destroyer = IndustryType:new{ ref_name = "carbon_destroyer" }
carbon_destroyer.name = _("Carbon destroyer")
IndustryType:register(carbon_destroyer)
IndustryType:add_input(carbon_destroyer, coal)
IndustryType:add_output(carbon_destroyer, charcoal)

gunpowder_mixer = IndustryType:new{ ref_name = "gunpowder_mixer" }
gunpowder_mixer.name = _("Gunpowder mixer")
IndustryType:register(gunpowder_mixer)
IndustryType:add_input(gunpowder_mixer, sulfur)
IndustryType:add_input(gunpowder_mixer, charcoal)
IndustryType:add_output(gunpowder_mixer, gunpowder)

stainless_steel_smelter = IndustryType:new{ ref_name = "stainless_steel_smelter" }
stainless_steel_smelter.name = _("Stainless steel smelter")
IndustryType:register(stainless_steel_smelter)
IndustryType:add_input(stainless_steel_smelter, steel)
IndustryType:add_input(stainless_steel_smelter, coal)
IndustryType:add_output(stainless_steel_smelter, stainless_steel)

brass_smelter = IndustryType:new{ ref_name = "brass_smelter" }
brass_smelter.name = _("Brass smelter")
IndustryType:register(brass_smelter)
IndustryType:add_input(brass_smelter, stainless_steel)
IndustryType:add_input(brass_smelter, iron)
IndustryType:add_input(brass_smelter, coal)
IndustryType:add_output(brass_smelter, brass)

iron_smelter = IndustryType:new{ ref_name = "iron_smelter" }
iron_smelter.name = _("Iron smelter")
IndustryType:register(iron_smelter)
IndustryType:add_input(iron_smelter, iron)
IndustryType:add_input(iron_smelter, coal)
IndustryType:add_output(iron_smelter, steel)

musket_factory = IndustryType:new{ ref_name = "musket_factory" }
musket_factory.name = _("Musket factory")
IndustryType:register(musket_factory)
IndustryType:add_input(musket_factory, timber)
IndustryType:add_input(musket_factory, brass)
IndustryType:add_input(musket_factory, stainless_steel)
IndustryType:add_input(musket_factory, ammunition)
IndustryType:add_output(musket_factory, musket)

machinegun_factory = IndustryType:new{ ref_name = "machinegun_factory" }
machinegun_factory.name = _("Machinegun factory")
IndustryType:register(machinegun_factory)
IndustryType:add_input(machinegun_factory, brass)
IndustryType:add_input(machinegun_factory, stainless_steel)
IndustryType:add_input(machinegun_factory, ammunition)
IndustryType:add_input(machinegun_factory, coal)
IndustryType:add_input(machinegun_factory, musket)
IndustryType:add_output(machinegun_factory, machinegun)

ammunition_factory = IndustryType:new{ ref_name = "ammunition_factory" }
ammunition_factory.name = _("Ammunition factory")
IndustryType:register(ammunition_factory)
IndustryType:add_input(ammunition_factory, gunpowder)
IndustryType:add_input(ammunition_factory, coal)
IndustryType:add_output(ammunition_factory, ammunition)

sails_factory = IndustryType:new{ ref_name = "sails_factory" }
sails_factory.name = _("Sails factory")
IndustryType:register(sails_factory)
IndustryType:add_input(sails_factory, textile)
IndustryType:add_output(sails_factory, sail)